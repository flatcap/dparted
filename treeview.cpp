/* Copyright (c) 2012 Richard Russon (FlatCap)
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


#include <iostream>
#include <stdlib.h>

#include "treeview.h"

/**
 * TreeView
 */
TreeView::TreeView()
{
	//Add the TreeView's view columns:
	Gtk::TreeView::Column *col = NULL;

	col = Gtk::manage (new Gtk::TreeView::Column ("Partition"));
	col->pack_start (m_Columns.col_partition,  true);
	col->pack_start (m_Columns.col_icon1,      false);
	col->pack_start (m_Columns.col_icon2,      false);
	append_column (*col);

	col = Gtk::manage (new Gtk::TreeView::Column ("Filesystem"));
	col->pack_start (m_Columns.col_colour,     false);
	col->pack_start (m_Columns.col_filesystem, true);
	append_column (*col);

	append_column ("Mount",      m_Columns.col_mount);
	append_column ("Label",      m_Columns.col_label);
	append_column ("Size",       m_Columns.col_size);
	append_column ("Used",       m_Columns.col_used);
	append_column ("Unused",     m_Columns.col_unused);
	append_column ("Flags",      m_Columns.col_flags);

	//Create the Tree model:
	m_refTreeModel = Gtk::TreeStore::create (m_Columns);
	set_model (m_refTreeModel);

	set_level_indentation (20);

	//Connect signal:
	signal_row_activated().connect (sigc::mem_fun (*this, &TreeView::on_row_activated));
	signal_query_tooltip().connect (sigc::mem_fun (*this, &TreeView::on_query_tooltip));

#if 1
	//Fill popup menu:
	Gtk::MenuItem *item = Gtk::manage (new Gtk::MenuItem ("_Edit", true));
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

	set_has_tooltip (true);

	signal_popup_menu().connect (sigc::mem_fun (*this, &TreeView::on_popup_menu));
#endif
}

/**
 * ~TreeView
 */
TreeView::~TreeView()
{
}

/**
 * on_popup_menu
 */
bool TreeView::on_popup_menu (void)
{
	m_Menu_Popup.popup (0, 0);

	return true;
}

/**
 * on_button_press_event
 */
bool TreeView::on_button_press_event (GdkEventButton *event)
{
	bool return_value = false;

	//Call base class, to allow normal handling,
	//such as allowing the row to be selected by the right-click:
	return_value = Gtk::TreeView::on_button_press_event (event);

	//Then do our custom stuff:
	if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3))
	{
		m_Menu_Popup.popup (event->button, event->time);
	}

	return return_value;
}

/**
 * on_menu_file_popup_generic
 */
void TreeView::on_menu_file_popup_generic()
{
	std::cout << "A popup menu item was selected.\n";

	Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
	if (refSelection)
	{
		Gtk::TreeModel::iterator iter = refSelection->get_selected();
		if (iter)
		{
			std::cout << "selected something\n";
		}
	}
}


/**
 * get_color_as_pixbuf
 */
Glib::RefPtr<Gdk::Pixbuf> get_color_as_pixbuf (int width, int height)
{
	Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create (Gdk::COLORSPACE_RGB, false, 8, width, height);

	int r = random();
	pixbuf->fill (r);

	return pixbuf;
}


/**
 * tree_add_row
 */
void TreeView::tree_add_row (DPContainer *c, Gtk::TreeModel::Row *parent)
{
	Gtk::TreeModel::Row row;
	std::vector<DPContainer*>::iterator i;

	for (i = c->children.begin(); i != c->children.end(); i++) {
		DPContainer *x = (*i);
		if (x->is_a ("volumegroup"))
			continue; //RAR for now ignore vg
		//std::cout << "name: " << x->name << "\n";
		if (parent) {
			row = *(m_refTreeModel->append (parent->children()));
		} else {
			row = *(m_refTreeModel->append());
		}
		//row[m_Columns.col_icon] = render_icon_pixbuf (Gtk::Stock::DND, Gtk::ICON_SIZE_MENU);
		row[m_Columns.col_partition] = x->device;
		row[m_Columns.col_size] = x->bytes_size;
		row[m_Columns.col_used] = x->bytes_used;
		row[m_Columns.col_unused] = x->bytes_size - x->bytes_used;
		//row[m_Columns.col_colour] = get_color_as_pixbuf (16, 16);

		if (x->children.size() > 0) {
			tree_add_row (x, &row);
		}
	}
	expand_all();
}


/**
 * init_treeview
 */
void TreeView::init_treeview (DPContainer *c)
{
	tree_add_row (c, NULL);
}

/**
 * on_row_activated
 */
void TreeView::on_row_activated (const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn *col)
{
#if 0
	std::cout << "Path: " << path.to_string() << "\n";

	Gtk::TreeModel::iterator iter = m_refTreeModel->get_iter (path);
	if (iter)
	{
		DPContainer *c = NULL;
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
bool TreeView::on_query_tooltip (int x, int y, bool keyboard_tooltip, const Glib::RefPtr<Gtk::Tooltip>& tooltip)
{
#if 1
	if (keyboard_tooltip)
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
		tooltip->set_text ("wibble");
	}
#endif
	return true;
}
