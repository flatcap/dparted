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
	m_Menu_Popup.signal_show().connect([this] { menu_active = true;  });
	m_Menu_Popup.signal_hide().connect([this] { menu_active = false; });

	set_has_tooltip (true);
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
		row[m_Columns.col_partition] = c->device;
		row[m_Columns.col_size]      = c->bytes_size;
		row[m_Columns.col_used]      = c->bytes_used;
		row[m_Columns.col_free]      = c->bytes_size - c->bytes_used;
		row[m_Columns.col_container] = c;
		expand_all();
#endif
	}

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
 * get_color_as_pixbuf
 */
Glib::RefPtr<Gdk::Pixbuf>
get_color_as_pixbuf (int width, int height)
{
	Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create (Gdk::COLORSPACE_RGB, false, 8, width, height);

	int r = random();
	pixbuf->fill (r);

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
#if 0
		if (x->is_a ("Group"))
			continue; //RAR for now ignore vg
#endif
		//std::cout << "name: " << x->name << "\n";
		if (parent) {
			row = *(m_refTreeModel->append (parent->children()));
		} else {
			row = *(m_refTreeModel->append());
		}
		//row[m_Columns.col_icon]      = render_icon_pixbuf (Gtk::Stock::DND, Gtk::ICON_SIZE_MENU);
		row[m_Columns.col_partition] = x->label;
		row[m_Columns.col_size]      = x->bytes_size;
		row[m_Columns.col_used]      = x->bytes_used;
		row[m_Columns.col_free]      = x->bytes_size - x->bytes_used;
		row[m_Columns.col_container] = x;
		//row[m_Columns.col_colour]    = get_color_as_pixbuf (16, 16);
		//std::cout << "Container: " << x << std::endl;

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
	//Add the TreeView's view columns:
	Gtk::TreeView::Column* col = nullptr;

	col = Gtk::manage (new Gtk::TreeView::Column ("Partition"));
	col->pack_start (m_Columns.col_partition,  true);
	col->pack_start (m_Columns.col_icon1,      false);
	col->pack_start (m_Columns.col_icon2,      false);
	append_column (*col);

	col = Gtk::manage (new Gtk::TreeView::Column ("Filesystem"));
	col->pack_start (m_Columns.col_colour,     false);
	col->pack_start (m_Columns.col_filesystem, true);
	append_column (*col);

	append_column ("Mount", m_Columns.col_mount);
	append_column ("Label", m_Columns.col_label);
	append_column ("Size",  m_Columns.col_size);
	append_column ("Used",  m_Columns.col_used);
	append_column ("Free",  m_Columns.col_free);
	append_column ("Flags", m_Columns.col_flags);

	//Create the Tree model:
	m_refTreeModel = Gtk::TreeStore::create (m_Columns);
	//set_model (m_refTreeModel);

	set_level_indentation (20);

	//Connect signal:
	signal_row_activated().connect (sigc::mem_fun (*this, &TreeView::on_row_activated));
	signal_query_tooltip().connect (sigc::mem_fun (*this, &TreeView::on_query_tooltip));

	tree_add_row (c, nullptr);

	set_model (m_refTreeModel);
	//expand_all();
}

/**
 * on_row_activated
 */
void
TreeView::on_row_activated (const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* col)
{
#if 0
	std::cout << "Path: " << path.to_string() << "\n";

	Gtk::TreeModel::iterator iter = m_refTreeModel->get_iter (path);
	if (iter) {
		GfxContainerPtr c;
		Gtk::TreeModel::Row row = *iter;
		std::cout << "Row activated: Name=" << row[m_Columns.col_name] << ", Type=" << row[m_Columns.col_type] << "\n";

		c = row[m_Columns.col_container];
		std::cout << typeid (row[m_Columns.col_container]).name() << "\n";
		std::cout << typeid (c).name() << "\n";
		std::cout << "Name=" << c->name << "\n";
	}
#endif
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