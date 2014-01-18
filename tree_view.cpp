/* Copyright (c) 2014 Richard Russon (FlatCap)
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Library General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <cstdlib>
#include <iostream>

#include "tree_view.h"
#include "log_trace.h"
#include "dparted.h"

/**
 * TreeView
 */
TreeView::TreeView()
{
#if 1
	//Fill popup menu:
	Gtk::MenuItem* item = Gtk::manage (new Gtk::MenuItem ("_Edit", true));
	item->signal_activate().connect (sigc::mem_fun (*this, &TreeView::on_menu_file_popup_generic));
	m_Menu_Popup.append (*item);

	item = Gtk::manage (new Gtk::MenuItem ("_Process", true));
	item->signal_activate().connect (sigc::mem_fun (*this, &TreeView::on_menu_file_popup_generic));
	m_Menu_Popup.append (*item);

	item = Gtk::manage (new Gtk::MenuItem ("_Remove", true));
	item->signal_activate().connect (sigc::mem_fun (*this, &TreeView::on_menu_file_popup_generic));
	m_Menu_Popup.append (*item);

	m_Menu_Popup.accelerate (*this);
	m_Menu_Popup.show_all();

	m_Menu_Popup.signal_key_press_event().connect (sigc::mem_fun (*this, &TreeView::popup_on_keypress));

	// Lambdas to let us know when the popup menu is in use.
	m_Menu_Popup.signal_show().connect ([this] { menu_active = true;  });
	m_Menu_Popup.signal_hide().connect ([this] { menu_active = false; });

	set_has_tooltip (true);
	set_activate_on_single_click (true);
#endif
}

/**
 * ~TreeView
 */
TreeView::~TreeView()
{
}


/**
 * popup_on_keypress
 */
bool
TreeView::popup_on_keypress (GdkEventKey* ev)
{
	std::cout << "menu key" << std::endl;
	if ((ev->keyval == GDK_KEY_Menu) && menu_active) {
		m_Menu_Popup.popdown();
		menu_active = false;
		return true;
	}

	return false;
}

/**
 * on_button_press_event
 */
bool
TreeView::on_button_press_event (GdkEventButton* event)
{
	bool return_value = false;

	//Call base class, to allow normal handling,
	//such as allowing the row to be selected by the right-click:
	return_value = Gtk::TreeView::on_button_press_event (event);
#if 0

	//Then do our custom stuff:
	if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3)) {
		m_Menu_Popup.popup (event->button, event->time);
	}

	//Then do our custom stuff:
	if ((event->type == GDK_DOUBLE_BUTTON_PRESS) && (event->button == 1)) {
		Glib::RefPtr<Gtk::TreeSelection> s1 = get_selection();
		Gtk::TreeModel::iterator s2 = s1->get_selected();
		const Gtk::TreeModel::Row& s3 = *s2;
		GfxContainerPtr c = s3[m_Columns.col_container];
		std::cout << "Selection: " << c << std::endl;

#if 0
		Gtk::TreeModel::Row row;
		row = *(m_refTreeModel->append (s3->children()));
		row[m_Columns.col_container] = c->device;
		row[m_Columns.col_size]      = c->bytes_size;
		row[m_Columns.col_used]      = c->bytes_used;
		row[m_Columns.col_free]      = c->bytes_size - c->bytes_used;
		row[m_Columns.col_container] = c;
		expand_all();
#endif
	}
#endif

	return return_value;
}

/**
 * on_menu_file_popup_generic
 */
void
TreeView::on_menu_file_popup_generic()
{
	std::cout << "A popup menu item was selected.\n";

	Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
	if (refSelection) {
		Gtk::TreeModel::iterator iter = refSelection->get_selected();
		if (iter) {
			std::cout << "selected something\n";
		}
	}
}


/**
 * get_colour_as_pixbuf
 */
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


/**
 * tree_add_row
 */
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
			//std::cout << x->treepath << '\t' << x << std::endl;

			//row[m_Columns.col_icon]      = render_icon_pixbuf (Gtk::Stock::DND, Gtk::ICON_SIZE_MENU);

			row[m_Columns.col_gfx_container] = x;

			std::string dev = x->device;
			size_t pos = dev.find_last_of ('/');
			if (pos != std::string::npos) {
				dev = dev.substr (pos+1);
			}
			row[m_Columns.col_container] = dev;

			row[m_Columns.col_colour]    = get_colour_as_pixbuf (16, x->colour);
			row[m_Columns.col_type]      = x->type;
			row[m_Columns.col_name]      = x->name;

			row[m_Columns.col_size]        = x->bytes_size / 1024 / 1024;
			row[m_Columns.col_size_suffix] = "MiB";

			row[m_Columns.col_used]        = x->bytes_used / 1024 / 1024;
			row[m_Columns.col_used_suffix] = "MiB";

			row[m_Columns.col_free]        = (x->bytes_size - x->bytes_used) / 1024 / 1024;
			row[m_Columns.col_free_suffix] = "MiB";
		} else {
			if (parent)
				row = *parent;
		}

		if (x->children.size() > 0) {
			tree_add_row (x, &row);
		}
	}
}


/**
 * init_treeview
 */
void
TreeView::init_treeview (GfxContainerPtr& c)
{
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


	//Add the TreeView's view columns:
	Gtk::TreeView::Column* col = nullptr;

	col = Gtk::manage (new Gtk::TreeView::Column ("Container"));
	col->pack_start (m_Columns.col_container,  true);
	col->set_alignment (0.0);
	append_column (*col);

	col = Gtk::manage (new Gtk::TreeView::Column ("Type"));
	col->pack_start (m_Columns.col_colour, false);
	col->pack_start (m_Columns.col_type,   false);
	col->set_alignment (0.0);
	append_column (*col);

	col = Gtk::manage (new Gtk::TreeView::Column ("Label"));
	col->pack_start (m_Columns.col_name,   false);
	col->set_alignment (0.0);
	append_column (*col);

	col = Gtk::manage (new Gtk::TreeView::Column ("Size"));
	col->pack_start (m_Columns.col_size,        false);
	col->pack_start (m_Columns.col_size_suffix, false);
	col->get_first_cell()->set_alignment (1.0, 0.5);
	col->set_alignment (1.0);
	append_column (*col);

	col = Gtk::manage (new Gtk::TreeView::Column ("Used"));
	col->pack_start (m_Columns.col_used,        false);
	col->pack_start (m_Columns.col_used_suffix, false);
	col->get_first_cell()->set_alignment (1.0, 0.5);
	col->set_alignment (1.0);
	append_column (*col);

	col = Gtk::manage (new Gtk::TreeView::Column ("Free"));
	col->pack_start (m_Columns.col_free,        false);
	col->pack_start (m_Columns.col_free_suffix, false);
	col->get_first_cell()->set_alignment (1.0, 0.5);
	col->set_alignment (1.0);
	append_column (*col);

	col = Gtk::manage (new Gtk::TreeView::Column (""));
	col->pack_start (m_Columns.col_empty, true);
	append_column (*col);

	/* col->pack_start (m_Columns.col_icon1,      false); */
	/* col->pack_start (m_Columns.col_icon2,      false); */
#if 0
	append_column ("Mount", m_Columns.col_mount);
	append_column ("Label", m_Columns.col_label);
	append_column ("Size",  m_Columns.col_size);
	append_column ("Used",  m_Columns.col_used);
	append_column ("Free",  m_Columns.col_free);
	append_column ("Flags", m_Columns.col_flags);
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


/**
 * on_query_tooltip
 */
bool
TreeView::on_query_tooltip (int x, int y, bool keyboard_tooltip, const Glib::RefPtr<Gtk::Tooltip>& tooltip)
{
#if 1
	//std::cout << "qtt: " << menu_active << std::endl;
	if (keyboard_tooltip)
		return false;
	if (menu_active)
		return false;

	int tx = 0;
	int ty = 0;

	convert_widget_to_tree_coords (x, y, tx, ty);

	//std::cout << "tooltip at (x,y) " << x << "," << y << "-- (tx,ty) " << tx << "," << ty << "\n";

	Gtk::TreeModel::Path path;

	if (get_path_at_pos (tx, ty, path)) {
#if 1
		tooltip->set_text (path.to_string());
#else
		Gtk::TreeModel::iterator iter = m_refTreeModel->get_iter (path);
		Gtk::TreeModel::Row row = *iter;
		tooltip->set_text (row[m_Columns.col_name] + ":" + row[m_Columns.col_type]);
#endif
	} else {
		// implies mouse over non-data part of textview, e.g. headers
		tooltip->set_text ("wibble"); // "Click to sort"?
	}
#endif
	return true;
}


/**
 * set_focus
 */
void
TreeView::set_focus (GfxContainerPtr& c)
{
	//std::cout << "focus: " << c->treepath << std::endl;
	if (!c)
		return;

	if (c->treepath.empty()) {
		std::cout << "TreeView: not visible" << std::endl;
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

/**
 * on_selection_changed
 */
void TreeView::on_selection_changed()
{
	//LOG_TRACE;

	Gtk::TreeModel::iterator it = treeselection->get_selected();
	if (!it)
		return;

	const Gtk::TreeModel::Row& row = *it;
	GfxContainerPtr c = row[m_Columns.col_gfx_container];
	std::cout << "sel: " << c << std::endl;

	DParted *dp = reinterpret_cast<DParted*> (get_toplevel());
	dp->set_focus (c);
}

