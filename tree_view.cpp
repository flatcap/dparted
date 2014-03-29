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

#include "gui_app.h"
#include "log_trace.h"
#include "theme.h"
#include "tree_view.h"
#include "utils.h"
#include "window.h"

TreeView::TreeView (void)
{
	setup_popup();

	set_has_tooltip (true);
	set_activate_on_single_click (true);
}

TreeView::~TreeView()
{
}


bool
TreeView::on_button_press_event (GdkEventButton* event)
{
	bool return_value = false;

	//Call base class, to allow normal handling,
	//such as allowing the row to be selected by the right-click:
	return_value = Gtk::TreeView::on_button_press_event (event);

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
		log_debug ("Selection: %s\n", c->dump());
	}
#endif

	return return_value;
}

Glib::RefPtr<Gdk::Pixbuf>
get_colour_as_pixbuf (int size, Gdk::RGBA colour)
{
	//XXX circle, of specified colour, transparent background
	//XXX get size from size of treerow (minus a small margin)
	//XXX what if one row is double height?

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
TreeView::tree_add_row (GfxContainerPtr& c, Gtk::TreeModel::Row* parent)
{
	Gtk::TreeModel::Row row;

	for (auto x : c->children) {
		std::string type = x->type;

		bool display = false;

		if (x->treeview == "always")
			display = true;

		if ((x->treeview == "empty") && (x->children.empty()))
			display = true;

		if (display) {
			if (parent) {
				row = *(m_refTreeModel->append (parent->children()));
			} else {
				row = *(m_refTreeModel->append());
			}

			x->treepath = m_refTreeModel->get_string (row);
			//log_debug ("%s\t%s\n", x->treepath.c_str(), x->dump());

			//row[col_icon]      = render_icon_pixbuf (Gtk::Stock::DND, Gtk::ICON_SIZE_MENU);

#if 0
			ModColPtr& m = mod_cols[0];
			Gtk::TreeModelColumnBase& base = *m;
			row[base] = x;
#endif

#if 0
			std::string dev = x->device;
			std::size_t pos = dev.find_last_of ('/');
			if (pos != std::string::npos) {
				dev = dev.substr (pos+1);
			}
			row[col_container] = dev;

			row[col_colour]    = get_colour_as_pixbuf (16, x->colour);
			row[col_type]      = x->type;
			row[col_name]      = x->name;

			row[col_size]        = x->bytes_size / 1024 / 1024;
			row[col_size_suffix] = "MiB";

			row[col_used]        = x->bytes_used / 1024 / 1024;
			row[col_used_suffix] = "MiB";

			row[col_free]        = (x->bytes_size - x->bytes_used) / 1024 / 1024;
			row[col_free_suffix] = "MiB";
#endif
		} else {
			if (parent)
				row = *parent;
		}

		if (x->children.size() > 0) {
			tree_add_row (x, &row);
		}
	}
}


#if 0
template<class T_ModelColumnType> inline
void TreeViewColumn::pack_start(const TreeModelColumn<T_ModelColumnType>& column, bool expand)
{
	//Generate appropriate Renderer for the column:
	CellRenderer* pCellRenderer = manage( CellRenderer_Generation::generate_cellrenderer<T_ModelColumnType>() );

	//Use the renderer:
	pack_start(*pCellRenderer, expand);
	set_renderer(*pCellRenderer, column);
}
#endif

void
TreeView::init_treeview (GfxContainerPtr& c)
{
	theme = gui_app->get_theme();

#if 0
	std::string tvcols = theme->get_config ("App", "", "treeview_columns");
	std::vector<std::string> tvc;
	explode (",", tvcols, tvc);
	for (auto i : tvc) {
		log_debug ("%s\n", i.c_str());
	}
#endif
	std::string name = "App.treeview_cols";
	std::vector<std::string> vec = theme->get_children (name);
	log_debug ("%s children\n", name.c_str());
	for (auto i : vec) {
		std::string key = name + "." + i;
		log_debug ("\t%s = ", i.c_str());
		try {
			std::string t = theme->get_config (key, "", "type", false);
			log_debug ("%s\n", t.c_str());
		} catch (...) {
			log_debug ("string");
		}
		log_debug ("\n");
	}

	mod_cols.push_back (ModColPtr (new Gtk::TreeModelColumn<GfxContainerPtr>));		//  0 gfx_container
	mod_cols.push_back (ModColPtr (new Gtk::TreeModelColumn<std::string>));			//  1 container
	mod_cols.push_back (ModColPtr (new Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>>));	//  2 colour
	mod_cols.push_back (ModColPtr (new Gtk::TreeModelColumn<std::string>));			//  3 type
	mod_cols.push_back (ModColPtr (new Gtk::TreeModelColumn<std::string>));			//  4 name
	mod_cols.push_back (ModColPtr (new Gtk::TreeModelColumn<uint64_t>));			//  5 size
	mod_cols.push_back (ModColPtr (new Gtk::TreeModelColumn<std::string>));			//  6 size_suffix
	mod_cols.push_back (ModColPtr (new Gtk::TreeModelColumn<uint64_t>));			//  7 used
	mod_cols.push_back (ModColPtr (new Gtk::TreeModelColumn<std::string>));			//  8 used_suffix
	mod_cols.push_back (ModColPtr (new Gtk::TreeModelColumn<uint64_t>));			//  9 free
	mod_cols.push_back (ModColPtr (new Gtk::TreeModelColumn<std::string>));			// 10 free_suffix
	mod_cols.push_back (ModColPtr (new Gtk::TreeModelColumn<std::string>));			// 11 empty

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

	m_Columns.add (*mod_cols[ 0]);		//  0 gfx_container
	m_Columns.add (*mod_cols[ 1]);		//  1 container
	m_Columns.add (*mod_cols[ 2]);		//  2 colour
	m_Columns.add (*mod_cols[ 3]);		//  3 type
	m_Columns.add (*mod_cols[ 4]);		//  4 name
	m_Columns.add (*mod_cols[ 5]);		//  5 size
	m_Columns.add (*mod_cols[ 6]);		//  6 size_suffix
	m_Columns.add (*mod_cols[ 7]);		//  7 used
	m_Columns.add (*mod_cols[ 8]);		//  8 used_suffix
	m_Columns.add (*mod_cols[ 9]);		//  9 free
	m_Columns.add (*mod_cols[10]);		// 10 free_suffix
	m_Columns.add (*mod_cols[11]);		// 11 empty

	//Add the TreeView's view columns:
	Gtk::TreeView::Column* col = nullptr;

	col = Gtk::manage (new Gtk::TreeView::Column ("Container"));
	col->pack_start (col_container,  true);
	col->set_alignment (0.0);
	append_column (*col);

#if 0
	col = Gtk::manage (new Gtk::TreeView::Column ("Type"));
	col->pack_start (col_colour, false);
	col->pack_start (col_type,   false);
	col->set_alignment (0.0);
	append_column (*col);

	col = Gtk::manage (new Gtk::TreeView::Column ("Label"));
	col->pack_start (col_name,   false);
	col->set_alignment (0.0);
	append_column (*col);

	col = Gtk::manage (new Gtk::TreeView::Column ("Size"));
	col->pack_start (col_size,        false);
	col->pack_start (col_size_suffix, false);
	col->get_first_cell()->set_alignment (1.0, 0.5);
	col->set_alignment (1.0);
	append_column (*col);

	col = Gtk::manage (new Gtk::TreeView::Column ("Used"));
	col->pack_start (col_used,        false);
	col->pack_start (col_used_suffix, false);
	col->get_first_cell()->set_alignment (1.0, 0.5);
	col->set_alignment (1.0);
	append_column (*col);

	col = Gtk::manage (new Gtk::TreeView::Column ("Free"));
	col->pack_start (col_free,        false);
	col->pack_start (col_free_suffix, false);
	col->get_first_cell()->set_alignment (1.0, 0.5);
	col->set_alignment (1.0);
	append_column (*col);

	col = Gtk::manage (new Gtk::TreeView::Column (""));
	col->pack_start (col_empty, true);
	append_column (*col);
#endif

	/* col->pack_start (col_icon1,      false); */
	/* col->pack_start (col_icon2,      false); */
#if 0
	append_column ("Mount", col_mount);
	append_column ("Label", col_label);
	append_column ("Size",  col_size);
	append_column ("Used",  col_used);
	append_column ("Free",  col_free);
	append_column ("Flags", col_flags);
#endif

	//Create the Tree model:
	m_refTreeModel = Gtk::TreeStore::create (m_Columns);
	//set_model (m_refTreeModel);

	set_level_indentation (10);

	//Connect signal:
	signal_query_tooltip().connect (sigc::mem_fun (*this, &TreeView::on_query_tooltip));

	treeselection = get_selection();
	treeselection->signal_changed().connect (sigc::mem_fun (*this, &TreeView::on_selection_changed));

	tree_add_row (c, nullptr);

	set_model (m_refTreeModel);
	expand_all();
}

bool
TreeView::on_query_tooltip (int x, int y, bool keyboard_tooltip, const Glib::RefPtr<Gtk::Tooltip>& tooltip)
{
#if 1
	//log_debug ("qtt: %d\n", menu_active);
	if (keyboard_tooltip)
		return false;

	if (menu_active)
		return false;

	int tx = 0;
	int ty = 0;

	convert_widget_to_tree_coords (x, y, tx, ty);

	//log_debug ("  tooltip at (x,y) %d,%d -- (tx,ty) %d, %d\n", x, y, tx, ty);

	Gtk::TreeModel::Path path;

	if (get_path_at_pos (tx, ty, path)) {
#if 1
		tooltip->set_text (path.to_string());
#else
		Gtk::TreeModel::iterator iter = m_refTreeModel->get_iter (path);
		Gtk::TreeModel::Row row = *iter;
		tooltip->set_text (row[col_name] + ":" + row[col_type]);
#endif
	} else {
		// implies mouse over non-data part of textview, e.g. headers, or dead space below
		tooltip->set_text ("wibble"); // "Click to sort"?
	}
#endif
	return true;
}

void
TreeView::set_focus (GfxContainerPtr& c)
{
	//log_debug ("focus: %s\n", c->treepath.c_str());
	if (!c)
		return;

	if (c->treepath.empty()) {
		//log_debug ("TreeView: not visible\n");
		treeselection->unselect_all();
		return;
	}

	Gtk::TreeModel::iterator it = treeselection->get_selected();
	if (it) {
		std::string current_path = m_refTreeModel->get_string (*it);
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
	//LOG_TRACE;

	Gtk::TreeModel::iterator it = treeselection->get_selected();
	if (!it)
		return;

#if 0
	const Gtk::TreeModel::Row& row = *it;
	GfxContainerPtr c = row[col_gfx_container];
	//log_debug ("sel: %s\n", c->dump());

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
		m_Menu_Popup.append (*item);
	}

	m_Menu_Popup.accelerate (*this);
	m_Menu_Popup.show_all();

	m_Menu_Popup.signal_key_press_event().connect (sigc::mem_fun (*this, &TreeView::popup_on_keypress));

	// Lambdas to let us know when the popup menu is in use.
	m_Menu_Popup.signal_show().connect ([this] { menu_active = true;  });
	m_Menu_Popup.signal_hide().connect ([this] { menu_active = false; });

	signal_key_press_event().connect (sigc::mem_fun (*this, &TreeView::on_keypress));
}

void
TreeView::on_menu_select (int UNUSED(option))
{
	log_debug ("A popup menu item was selected.\n");

	Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
	if (refSelection) {
		Gtk::TreeModel::iterator iter = refSelection->get_selected();
		if (iter) {
			log_debug ("selected something\n");
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
		log_debug ("No Window\n");
		return false;
	}

	GfxContainerPtr c = dp->get_focus();
	if (!c) {
		//log_debug ("No focus\n");
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
	log_debug ("size: %d,%d\n", allocation.get_width(), allocation.get_height());
#endif
	int ccx = 0;
	int ccy = 0;
	convert_tree_to_widget_coords (0, 0, ccx, ccy);

	int tx = 0;
	int ty = 0;
	Gtk::TreeModel::iterator it = treeselection->get_selected();
	if (it) {
		Gtk::TreeModel::Path path = m_refTreeModel->get_path (it);
		Gdk::Rectangle rect;
		get_cell_area (path, *get_column(0), rect);
		log_debug ("rect: %d,%d,%d,%d\n", rect.get_x(), rect.get_y(), rect.get_width(), rect.get_height());

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
	m_Menu_Popup.popup ([x,y] (int& xc, int& yc, bool& in) { xc = x; yc = y; in = false; }, 0, gtk_get_current_event_time());
}

bool
TreeView::popup_on_keypress (GdkEventKey* ev)
{
	if (ev->keyval == GDK_KEY_Menu) {
		m_Menu_Popup.popdown();
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

