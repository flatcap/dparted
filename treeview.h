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


#ifndef _TREEVIEW_H_
#define _TREEVIEW_H_

#include <gtkmm/treeview.h>
#include <gtkmm/treestore.h>

#include "container.h"

class TreeView : public Gtk::TreeView
{
public:
	TreeView();
	virtual ~TreeView();

	void init_treeview (DPContainer *c);

protected:
	// Override Signal handler:
	// Alternatively, use signal_button_press_event().connect_notify()
	virtual bool on_button_press_event (GdkEventButton *ev);

	//Signal handler for popup menu items:
	void on_menu_file_popup_generic();

	Gtk::Menu m_Menu_Popup;

	//Tree model columns:
	class ModelColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:
		ModelColumns()
		{
			add (col_partition);
			add (col_filesystem);
			add (col_mount);
			add (col_label);
			add (col_size);
			add (col_used);
			add (col_unused);
			add (col_flags);

			add (col_container);
			add (col_icon1);
			add (col_icon2);
			add (col_colour);
		}

		Gtk::TreeModelColumn<Glib::ustring>              col_partition;
		Gtk::TreeModelColumn<Glib::ustring>              col_filesystem;
		Gtk::TreeModelColumn<Glib::ustring>              col_mount;
		Gtk::TreeModelColumn<Glib::ustring>              col_label;
		Gtk::TreeModelColumn<long long>                  col_size;
		Gtk::TreeModelColumn<long long>                  col_used;
		Gtk::TreeModelColumn<long long>                  col_unused;
		Gtk::TreeModelColumn<Glib::ustring>              col_flags;

		Gtk::TreeModelColumn<DPContainer*>               col_container;
		Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > col_icon1;
		Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > col_icon2;
		Gtk::TreeModelColumn<int>                        col_colour;
	};

	ModelColumns m_Columns;
	Glib::RefPtr<Gtk::TreeStore> m_refTreeModel;

	void on_row_activated (const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column);
	bool on_query_tooltip (int x, int y, bool keyboard_tooltip, const Glib::RefPtr<Gtk::Tooltip> &tooltip);
	bool on_popup_menu (void);

	void tree_add_row (DPContainer *c, Gtk::TreeModel::Row *parent);

private:

};

#endif // _TREEVIEW_H_

