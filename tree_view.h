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

#ifndef _TREE_VIEW_H_
#define _TREE_VIEW_H_

#include <gtkmm/treeview.h>
#include <gtkmm/treestore.h>

#include "gfx_container.h"

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

	typedef std::shared_ptr<Gtk::TreeModelColumnBase> ModColPtr;
	std::vector<ModColPtr> mod_cols;

	Glib::RefPtr<Gtk::TreeStore> m_refTreeModel;

	bool on_query_tooltip (int x, int y, bool keyboard_tooltip, const Glib::RefPtr<Gtk::Tooltip>& tooltip);

	void tree_add_row (GfxContainerPtr& c, Gtk::TreeModel::Row* parent = nullptr);

	Glib::RefPtr<Gtk::TreeSelection> treeselection;
	void on_selection_changed();

	ThemePtr theme;

	template <class T>
	int add_column (Gtk::TreeModel::ColumnRecord& col_rec, Gtk::TreeView::Column* col)
	{
		Gtk::TreeModelColumn<T>* tmc = new Gtk::TreeModelColumn<T>;
		mod_cols.push_back (ModColPtr (tmc));
		col_rec.add (*tmc);
		col->pack_start (*tmc, false);

		return (col_rec.size()-1);
	}

	Glib::RefPtr<Gdk::Pixbuf> get_colour_as_pixbuf (int size, const std::string& colstr);

	// Map prop_name -> col_index, col type, align, precision, size
	std::map<std::string,std::tuple<int,int,float,int,int>> col_list;

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

