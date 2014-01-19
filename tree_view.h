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

#ifndef _TREE_VIEW_H_
#define _TREE_VIEW_H_

#include <gtkmm/treeview.h>
#include <gtkmm/treestore.h>

#include "gfx_container.h"

/**
 * class TreeView
 */
class TreeView : public Gtk::TreeView
{
public:
	TreeView();
	virtual ~TreeView();

	void init_treeview (GfxContainerPtr& c);
	void set_focus (GfxContainerPtr& c);

protected:
	// Override Signal handler:
	// Alternatively, use signal_button_press_event().connect_notify()
	virtual bool on_button_press_event (GdkEventButton* ev);

	//Tree model columns:
	class ModelColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:
		ModelColumns()
		{
			add (col_gfx_container);
			add (col_container);
			add (col_colour);
			add (col_type);
			add (col_name);
			add (col_size);
			add (col_size_suffix);
			add (col_used);
			add (col_used_suffix);
			add (col_free);
			add (col_free_suffix);
			add (col_empty);
		}

		Gtk::TreeModelColumn<GfxContainerPtr>           col_gfx_container;
		Gtk::TreeModelColumn<std::string>               col_container;
		Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>> col_colour;
		Gtk::TreeModelColumn<std::string>               col_type;
		Gtk::TreeModelColumn<std::string>               col_name;
		Gtk::TreeModelColumn<uint64_t>                  col_size;
		Gtk::TreeModelColumn<std::string>               col_size_suffix;
		Gtk::TreeModelColumn<uint64_t>                  col_used;
		Gtk::TreeModelColumn<std::string>               col_used_suffix;
		Gtk::TreeModelColumn<uint64_t>                  col_free;
		Gtk::TreeModelColumn<std::string>               col_free_suffix;
		Gtk::TreeModelColumn<std::string>               col_empty;
	};

	ModelColumns m_Columns;
	Glib::RefPtr<Gtk::TreeStore> m_refTreeModel;

	bool on_query_tooltip (int x, int y, bool keyboard_tooltip, const Glib::RefPtr<Gtk::Tooltip>& tooltip);

	void tree_add_row (GfxContainerPtr& c, Gtk::TreeModel::Row* parent);

	Glib::RefPtr<Gtk::TreeSelection> treeselection;
	void on_selection_changed();

private:
	// POPUP
	void setup_popup (void);
	void on_menu_select (int option);
	bool get_coords (int& x, int& y);
	void popup_menu (int x, int y);
	bool popup_on_keypress (GdkEventKey* ev);
	bool on_keypress (GdkEventKey* event);
	Gtk::Menu m_Menu_Popup;
	bool menu_active = false;
};


#endif // _TREE_VIEW_H_

