/* Copyright (c) 2014 Richard Russon (FlatCap)
 *
 * This file is part of DParted.
 *
 * DParted is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DParted is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DParted.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cstdlib>
#include <iomanip>

#include <gtkmm/cellrenderertext.h>
#include <gtkmm/cellrendererprogress.h>

#include "gui_app.h"
#include "theme.h"
#include "tree_view.h"
#include "utils.h"
#include "window.h"
#include "log.h"
#include "property.h"

enum ColType {
	ct_colour,
	ct_float,
	ct_graph,
	ct_human,
	ct_icon,
	ct_integer,
	ct_string
};

TreeView::TreeView (void)
{
	log_ctor ("ctor TreeView");
	setup_popup();

	set_has_tooltip (true);
	set_activate_on_single_click (true);

	gfx_listener   = GfxContainerListenerPtr ((IGfxContainerListener*) this, [](IGfxContainerListener*){});
	theme_listener = ThemeListenerPtr        ((IThemeListener*)        this, [](IThemeListener*){});
}

TreeView::~TreeView()
{
	log_dtor ("dtor TreeView");
}


bool
TreeView::on_button_press_event (GdkEventButton* event)
{
	bool retval = false;

	//Call base class, to allow normal handling,
	//such as allowing the row to be selected by the right-click:
	retval = Gtk::TreeView::on_button_press_event (event);

	//Then do our custom stuff:
	if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3)) {
		popup_menu (event->x_root, event->y_root);
	}

#if 0
	//Then do our custom stuff:
	if ((event->type == GDK_DOUBLE_BUTTON_PRESS) && (event->button == 1)) {
		Glib::RefPtr<Gtk::TreeSelection> s1 = get_selection();
		Gtk::TreeModel::iterator s2 = s1->get_selected();
		const Gtk::TreeModel::Row& s3 = *s2;
		GfxContainerPtr c = s3[col_container];
		log_debug ("Selection: %s", c->dump());
	}
#endif

	return retval;
}

Glib::RefPtr<Gdk::Pixbuf>
TreeView::get_colour_as_pixbuf (int size, const std::string& colstr)
{
	//circle, of specified colour, transparent background
	Gdk::RGBA colour = theme->get_colour (colstr);

	Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create (Gdk::COLORSPACE_RGB, true, 8, size, size);
	Cairo::RefPtr<Cairo::ImageSurface> surface = Cairo::ImageSurface::create (Cairo::FORMAT_ARGB32, size, size);
	Cairo::RefPtr<Cairo::Context> cr = Cairo::Context::create (surface);

	cr->save();
	//XXX note the order: ImageSurface (RGBA) and Pixbuf (BGRA)
	cr->set_source_rgba (colour.get_blue(), colour.get_green(), colour.get_red(), colour.get_alpha());
	cr->arc (size/2, size/2, (size+2)/2, 0, 2*M_PI);
	cr->fill();
	cr->restore();

	memcpy (pixbuf->get_pixels(), surface->get_data(), size*size*4);

	return pixbuf;
}


void
TreeView::tree_add_row (GfxContainerPtr& gfx, Gtk::TreeModel::Row* parent /*=nullptr*/)
{
	return_if_fail (gfx);

	Gtk::TreeModel::Row row;

	for (auto x : gfx->children) {
		bool display = false;

		if (x->treeview == "always")
			display = true;

		if ((x->treeview == "empty") && (x->children.empty()))
			display = true;

		if (display) {
			if (parent) {
				row = *(tree_model->append (parent->children()));
			} else {
				row = *(tree_model->append());
			}

			x->treepath = tree_model->get_string (row);

			std::string dev = x->device;
			std::size_t pos = dev.find_last_of ('/');
			if (pos != std::string::npos) {
				dev = dev.substr (pos+1);
			}

			row.set_value (0, x);		// Column zero is always the GfxContainer

			log_debug ("Columns:");
			for (auto i : col_list) {
				int index = -1;
				int type = ct_string;
				float align = 0;
				int precision = 3;
				int size = 0;

				std::tie (index, type, align, precision, size) = i.second;

				ContainerPtr c = x->get_container();
				if (!c) {
					log_debug ("\tNO CONTAINER");
					continue;
				}

				PPtr prop = c->get_prop (i.first);
				if (!prop) {
					log_debug ("\tMISSING");	// Not an error
					continue;
				}

				log_debug ("\tType: %s", prop->get_type_name().c_str());

				switch (type) {
					case ct_colour:
						row.set_value (index, get_colour_as_pixbuf (size, (std::string) *prop));
						break;
					case ct_float:
						//XXX check type
						row.set_value (index, (double) *prop);
						break;
					case ct_graph:
						//XXX check type
						row.set_value (index, (std::uint8_t) *prop);
						break;
					case ct_human:
						{
							//XXX check type
							std::string num = (std::string) *prop;
							std::string suf;
							std::size_t pos = num.find_first_of (" ");
							if (pos != std::string::npos) {
								std::size_t end = num.find_first_not_of (" ", pos+1);
								if (end != std::string::npos) {
									suf = num.substr (end);
								}
								num = num.substr (0, pos);
							}
							row.set_value (index,   num);
							row.set_value (index+1, suf);
						}
						break;
					case ct_icon:
						row.set_value (index, theme->get_icon ((std::string) *prop));
						break;
					case ct_integer:
						//XXX this isn't 64-bit clean
						{
							std::int64_t  val;
							std::uint64_t u;
							switch (prop->type) {
								case BaseProperty::Tag::t_bool:
								case BaseProperty::Tag::t_u8:
								case BaseProperty::Tag::t_u16:
								case BaseProperty::Tag::t_u32:
									val = (std::uint32_t) *prop;		// Unsigned, but fit into std::int64_t
									break;

								case BaseProperty::Tag::t_u64:
									u = (std::uint64_t) *prop;
									if (u & (1LL<<63)) {
										log_error ("value too large: %lu", u);
										break;
									}
									val = (std::int64_t) u;
									break;

								case BaseProperty::Tag::t_s16:
								case BaseProperty::Tag::t_s32:
								case BaseProperty::Tag::t_s64:
								case BaseProperty::Tag::t_s8:
									val = (std::int64_t) *prop;			// Signed and fit into std::int64_t
									break;

								default:
									log_error ("wrong type");
									break;
							}
							row.set_value (index, val);
						}
						break;
					case ct_string:
						row.set_value (index, (std::string) *prop);
						break;
					default:
						log_error ("notimpl");
				}
			}
		} else {
			if (parent)
				row = *parent;
		}

		if (x->children.size() > 0) {
			tree_add_row (x, &row);
		}
	}
}


ColType
parse_type (const std::string& type)
{
	if (type.empty())
		return ct_string;

	if ((type == "colour") || (type == "color"))
		return ct_colour;

	if (type == "float")
		return ct_float;

	if (type == "graph")
		return ct_graph;

	if (type == "human")
		return ct_human;

	if (type == "icon")
		return ct_icon;

	if (type == "integer")
		return ct_integer;

	if (type == "string")
		return ct_string;

	log_error ("Unknown type %s, defaulting to string", type.c_str());
	return ct_string;
}

float
parse_alignment (const std::string& align, float def)
{
	if (align.empty())
		return def;

	if ((align == "left") || (align == "start"))
		return 0.0;

	if ((align == "middle") || (align == "center") || (align == "centre"))
		return 0.5;

	if ((align == "right") || (align == "end"))
		return 1.0;

	log_error ("Unknown alignment: %s", align.c_str());
	return def;
}

int
parse_precision (const std::string& prec, int def)
{
	if (prec.empty())
		return def;

	size_t pos = prec.find_first_not_of ("0123456789");
	if (pos != std::string::npos) {
		log_error ("Invalid precision: %s", prec.c_str());
		return def;
	}

	StringNum s (prec);
	int val = (int) s;
	if (val > 10) {
		log_error ("Invalid precision: %s", prec.c_str());
		return def;
	}

	return val;
}

int
parse_size (const std::string& size, int def)
{
	if (size.empty())
		return def;

	size_t pos = size.find_first_not_of ("0123456789");
	if (pos != std::string::npos) {
		log_error ("Invalid size: %s", size.c_str());
		return def;
	}

	StringNum s (size);
	int val = (int) s;
	if (val > 4096) {		// Arbitrary
		log_error ("Invalid size: %s", size.c_str());
		return def;
	}

	return val;
}


template <>
int
TreeView::add_column<Glib::RefPtr<Gdk::Pixbuf>> (Gtk::TreeModel::ColumnRecord& col_rec, Gtk::TreeView::Column* col, float align, int size)
{
	auto* tmc = new Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>>;
	auto* cell = Gtk::manage (new Gtk::CellRendererPixbuf);
	mod_cols.push_back (ModColPtr (tmc));
	col_rec.add (*tmc);
	col->pack_start (*cell, false);
	col->add_attribute (cell->property_pixbuf(), *tmc);

	cell->set_alignment (align, 0.5);
	if (size > 0) {
		cell->set_fixed_size (size, -1);
	}

	return (col_rec.size()-1);
}

void
TreeView::init_treeview (GfxContainerPtr& gfx)
{
	top_level = gfx;

	gfx->add_listener (gfx_listener);

	theme = gui_app->get_theme();
	theme->add_listener (theme_listener);	//XXX if theme

	/*	DEVICE	COLOUR	TYPE	NAME	DISPLAY
	 *	loop0	none	block	loop0	empty
	 *	loop0	none	table	gpt	always
	 *	loop0p1	none	part	loop0p1	empty
	 *	loop0p1	red	fs	ext4	always
	 *	loop0p2	green	fs	vfat	always
	 *
	 *	CONTAINER		TYPE	COLOUR	NAME
	 *	loop0			table	-	gpt
	 *	|-- loop0p1		fs	red	ext4
	 *	`-- loop0p2		fs	green	vfat
	 *
	 *	loop0			block	-	-
	 *	`-- loop0		table	-	gpt
	 *	    |-- loop0p1		part	-	primary
	 *	    |	`-- loop0p1	ext4	red	ext4_label
	 *	    `-- loop0p2		part	-	primary
	 *	        `-- loop0p2	vfat	green	vfat_label
	 */

	Gtk::TreeModel::ColumnRecord col_rec;
	Gtk::TreeView::Column* col = nullptr;

	// Dummy column to store the container ptr
	Gtk::TreeModelColumn<GfxContainerPtr>* tmc = new Gtk::TreeModelColumn<GfxContainerPtr>;
	mod_cols.push_back (ModColPtr (tmc));
	col_rec.add (*tmc);

	std::string name = "App.treeview_cols";
	std::string display = theme->get_config (name, "", "display", false);
	std::vector<std::string> parts;
	explode (",", display, parts);
	for (auto i : parts) {
		std::vector<std::string> multi;
		explode ("+", i, multi);

		std::string key = name + "." + multi[0];
		std::string title = theme->get_config (key, "", "title", false);
		if (title.empty()) {
			title = multi[0];	// Replace abc_def with Abc Def?
		}

		col = Gtk::manage (new Gtk::TreeView::Column (title));

		Gtk::TreeModelColumn<int>* tmc = nullptr;
		Gtk::CellRendererProgress* cell = nullptr;
		for (auto j : multi) {
			int index = -1;
			int type = ct_string;
			float align = 0.0;
			int precision = 3;
			int size = 0;

			key = name + "." + j;
			type  = parse_type (theme->get_config (key, "", "type", false));
			switch (type) {
				case ct_colour:
					size      = parse_size (theme->get_config (key, "", "size", false), 16);
					index     = add_column<Glib::RefPtr<Gdk::Pixbuf>> (col_rec, col, 0.0, size);
					break;

				case ct_float:
					align     = parse_alignment (theme->get_config (key, "", "align",     false), 1.0);
					precision = parse_precision (theme->get_config (key, "", "precision", false), 3);
					size      = parse_size (theme->get_config (key, "", "size", false), 0);
					index     = add_column<double> (col_rec, col, align, size);
					break;

				case ct_graph:
					tmc = new Gtk::TreeModelColumn<int>;
					cell = Gtk::manage (new Gtk::CellRendererProgress);
					mod_cols.push_back (ModColPtr (tmc));
					col_rec.add (*tmc);
					col->pack_start (*cell, false);
					col->add_attribute (cell->property_value(), *tmc);

					index     = (col_rec.size()-1);
					size      = parse_size (theme->get_config (key, "", "size", false), 0);
					break;

				case ct_icon:
					size      = parse_size (theme->get_config (key, "", "size", false), 16);
					index     = add_column<Glib::RefPtr<Gdk::Pixbuf>> (col_rec, col, 0.0, size);
					break;

				case ct_integer:
					align     = parse_alignment (theme->get_config (key, "", "align", false), 1.0);
					size      = parse_size (theme->get_config (key, "", "size", false), 0);
					index     = add_column<std::int64_t> (col_rec, col, align, size);
					break;

				case ct_human:
					align     = parse_alignment (theme->get_config (key, "", "align", false), 1.0);
					size      = parse_size (theme->get_config (key, "", "size", false), 0);
					index     = add_column<std::string> (col_rec, col, 1.0, size);
					            add_column<std::string> (col_rec, col, 0.0, 0);
					break;

				default:
					log_error ("unknown type '%d'", type);

				case ct_string:
					align     = parse_alignment (theme->get_config (key, "", "align", false), 0.0);
					size      = parse_size (theme->get_config (key, "", "size", false), 0);
					index     = add_column<std::string> (col_rec, col, align, size);
					break;
			}

			col_list[j] = std::make_tuple (index, type, align, precision, size);

			col->set_alignment (align);
			if (size > 0) {
				col->set_fixed_width (size);
			}
		}
		append_column (*col);
	}

	// Dummy empty column to pad out treeview
	col = Gtk::manage (new Gtk::TreeView::Column (""));
	add_column<std::string> (col_rec, col, 0.0, 0);
	append_column (*col);

	tree_model = Gtk::TreeStore::create (col_rec);
	set_model (tree_model);

	set_level_indentation(0);
	set_enable_tree_lines (true);
	set_show_expanders (true);

	//Connect signal:
	signal_query_tooltip().connect (sigc::mem_fun (*this, &TreeView::on_query_tooltip));

	treeselection = get_selection();
	treeselection->signal_changed().connect (sigc::mem_fun (*this, &TreeView::on_selection_changed));

	tree_add_row (gfx);

	expand_all();
}


bool
TreeView::on_query_tooltip (int x, int y, bool keyboard_tooltip, const Glib::RefPtr<Gtk::Tooltip>& tooltip)
{
	log_debug ("qtt: %d", menu_active);
	if (keyboard_tooltip)
		return false;

	if (menu_active)
		return false;

	int tx = 0;
	int ty = 0;

	convert_widget_to_tree_coords (x, y, tx, ty);

	log_debug ("  tooltip at (x,y) %d,%d -- (tx,ty) %d, %d", x, y, tx, ty);

	Gtk::TreeModel::Path path;

	if (get_path_at_pos (tx, ty, path)) {
#if 1
		tooltip->set_text (path.to_string());
#else
		Gtk::TreeModel::iterator iter = tree_model->get_iter (path);
		Gtk::TreeModel::Row row = *iter;
		tooltip->set_text (row[col_name] + ":" + row[col_type]);
#endif
	} else {
		// implies mouse over non-data part of textview, e.g. headers, or dead space below
		tooltip->set_text ("wibble"); // "Click to sort"?
	}
	return true;
}

void
TreeView::set_focus (GfxContainerPtr& c)
{
	log_debug ("focus: %s", c->treepath.c_str());
	return_if_fail (c);

	if (c->treepath.empty()) {
		log_debug ("TreeView: not visible");
		treeselection->unselect_all();
		return;
	}

	Gtk::TreeModel::iterator it = treeselection->get_selected();
	if (it) {
		std::string current_path = tree_model->get_string (*it);
		if (c->treepath == current_path)
			return;
	}

	Gtk::TreeModel::Path path (c->treepath);
	set_cursor (path);
	scroll_to_row (path);
}

void
TreeView::on_selection_changed()
{
	LOG_TRACE;

	Gtk::TreeModel::iterator it = treeselection->get_selected();
	if (!it)
		return;

#if 0
	const Gtk::TreeModel::Row& row = *it;
	GfxContainerPtr c = row[col_gfx_container];
	log_debug ("sel: %s", c->dump());

	Window *dp = reinterpret_cast<Window*> (get_toplevel());
	dp->set_focus (c);
#endif
}


void
TreeView::setup_popup (void)
{
	std::vector<std::string> list = { "_Edit", "_Process", "_Remove" };

	Gtk::MenuItem* item = nullptr;
	for (unsigned int i = 0; i < list.size(); ++i) {
		item = Gtk::manage (new Gtk::MenuItem (list[i], true));
		item->signal_activate().connect (sigc::bind<int> (sigc::mem_fun (*this, &TreeView::on_menu_select), i));
		menu_popup.append (*item);
	}

	menu_popup.accelerate (*this);
	menu_popup.show_all();

	menu_popup.signal_key_press_event().connect (sigc::mem_fun (*this, &TreeView::popup_on_keypress));

	// Lambdas to let us know when the popup menu is in use.
	menu_popup.signal_show().connect ([this] { menu_active = true;  });
	menu_popup.signal_hide().connect ([this] { menu_active = false; });

	signal_key_press_event().connect (sigc::mem_fun (*this, &TreeView::on_keypress));
}

void
TreeView::on_menu_select (int UNUSED(option))
{
	log_debug ("A popup menu item was selected.");

	Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
	if (refSelection) {
		Gtk::TreeModel::iterator iter = refSelection->get_selected();
		if (iter) {
			log_debug ("selected something");
		}
	}
}

/**
 * get_coords
 *
 * Get the absolute screen coordinates of the highlighted container.
 */
bool
TreeView::get_coords (int& x, int& y)
{
	Window *dp = reinterpret_cast<Window*> (get_toplevel());
	if (!dp) {
		log_debug ("No Window");
		return false;
	}

	GfxContainerPtr c = dp->get_focus();
	if (!c) {
		log_debug ("No focus");
		return false;
	}

	Glib::RefPtr<Gdk::Window> w = get_window();
	if (!w) {
		return false;
	}

	int ox = 0;
	int oy = 0;
	w->get_origin (ox, oy);		// Coords of Window's main window (inside chrome)

	Gtk::Widget* window = dynamic_cast<Gtk::Widget*> (get_toplevel());
	if (!window) {
		return false;
	}

#if 0
	Gtk::Allocation allocation = get_allocation();
	log_debug ("size: %d,%d", allocation.get_width(), allocation.get_height());
#endif
	int ccx = 0;
	int ccy = 0;
	convert_tree_to_widget_coords (0, 0, ccx, ccy);

	int tx = 0;
	int ty = 0;
	Gtk::TreeModel::iterator it = treeselection->get_selected();
	if (it) {
		Gtk::TreeModel::Path path = tree_model->get_path (it);
		Gdk::Rectangle rect;
		get_cell_area (path, *get_column(0), rect);
		log_debug ("rect: %d,%d,%d,%d", rect.get_x(), rect.get_y(), rect.get_width(), rect.get_height());

		tx += rect.get_x() + rect.get_width();
		ty += rect.get_y() + rect.get_height();
	}

	x = ox + tx + ccx;
	y = oy + ty + ccy;
	return true;
}

void
TreeView::popup_menu (int x, int y)
{
	// Lamba to position popup menu
	menu_popup.popup ([x, y] (int& xc, int& yc, bool& in) { xc = x; yc = y; in = false; }, 0, gtk_get_current_event_time());
}

bool
TreeView::popup_on_keypress (GdkEventKey* ev)
{
	if (ev->keyval == GDK_KEY_Menu) {
		menu_popup.popdown();
		return true;
	}

	return false;
}

bool
TreeView::on_keypress (GdkEventKey* event)
{
	if (event->keyval == GDK_KEY_Menu) {	// 65383 (0xFF67)
		int x = 0;
		int y = 0;
		//XXX scroll row into view (somehow)
		// scroll_to_row (path); doesn't seem to work
		get_coords (x, y);
		popup_menu (x, y);
		return true;
	}

	return false;
}


void
TreeView::gfx_container_added (const GfxContainerPtr& cont, const GfxContainerPtr& parent)
{
	// LOG_TRACE;
	std::string c = "NULL";
	std::string p = "NULL";

	if (cont)   c = cont->name;
	if (parent) p = parent->name;

	log_debug ("TREEVIEW gfx_container_added: %s to %s", c.c_str(), p.c_str());
}

void
TreeView::gfx_container_busy (const GfxContainerPtr& UNUSED(cont), int UNUSED(busy))
{
	LOG_TRACE;
}

void
TreeView::gfx_container_changed (const GfxContainerPtr& UNUSED(cont))
{
	LOG_TRACE;
}

void
TreeView::gfx_container_deleted (const GfxContainerPtr& UNUSED(cont))
{
	LOG_TRACE;
}

void
TreeView::gfx_container_resync (const GfxContainerPtr& UNUSED(cont))
{
	LOG_TRACE;
}


void
TreeView::theme_changed (const ThemePtr& UNUSED(theme))
{
	LOG_TRACE;
}

void
TreeView::theme_dead (const ThemePtr& UNUSED(theme))
{
	LOG_TRACE;
}

