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
#include "gfx_container_listener.h"
#include "theme.h"
#include "theme_listener.h"

class TreeView :
	public Gtk::TreeView,
	public IGfxContainerListener,
	public IThemeListener
{
public:
	TreeView();
	virtual ~TreeView();

	void init_treeview (GfxContainerPtr& c);
	void set_focus (GfxContainerPtr& c);

	virtual void gfx_container_added   (const GfxContainerPtr& cont, const GfxContainerPtr& parent);
	virtual void gfx_container_busy    (const GfxContainerPtr& cont, int busy);
	virtual void gfx_container_changed (const GfxContainerPtr& cont);
	virtual void gfx_container_deleted (const GfxContainerPtr& cont);
	virtual void gfx_container_resync  (const GfxContainerPtr& cont);

	virtual void theme_changed (const ThemePtr& theme);
	virtual void theme_dead    (const ThemePtr& theme);

protected:
	GfxContainerPtr top_level;

	// Override Signal handler:
	// Alternatively, use signal_button_press_event().connect_notify()
	virtual bool on_button_press_event (GdkEventButton* ev);

	typedef std::shared_ptr<Gtk::TreeModelColumnBase> ModColPtr;
	std::vector<ModColPtr> mod_cols;

	Glib::RefPtr<Gtk::TreeStore> tree_model;

	bool on_query_tooltip (int x, int y, bool keyboard_tooltip, const Glib::RefPtr<Gtk::Tooltip>& tooltip);

	void tree_add_row (GfxContainerPtr& c, Gtk::TreeModel::Row* parent = nullptr);

	Glib::RefPtr<Gtk::TreeSelection> treeselection;
	void on_selection_changed();

	ThemePtr theme;

	template <class T>
	int add_column (Gtk::TreeModel::ColumnRecord& col_rec, Gtk::TreeView::Column* col, float align, int size)
	{
		auto* tmc = new Gtk::TreeModelColumn<T>;
		auto* cell = Gtk::manage (new Gtk::CellRendererText);
		mod_cols.push_back (ModColPtr (tmc));
		col_rec.add (*tmc);
		col->pack_start (*cell, false);
		col->add_attribute (cell->property_text(), *tmc);

		cell->set_alignment (align, 0.5);
		if (size > 0) {
			cell->set_fixed_size (size, -1);
		}

		return (col_rec.size()-1);
	}

	Glib::RefPtr<Gdk::Pixbuf> get_colour_as_pixbuf (int size, const std::string& colstr);

	// Map prop_name -> col_index, col type, align, precision, size
	std::map<std::string, std::tuple<int, int, float, int, int>> col_list;

private:
	GfxContainerListenerPtr gfx_listener;
	ThemeListenerPtr theme_listener;

	// POPUP
	void setup_popup (void);
	void on_menu_select (int option);
	bool get_coords (int& x, int& y);
	void popup_menu (int x, int y);
	bool popup_on_keypress (GdkEventKey* ev);
	bool on_keypress (GdkEventKey* event);
	Gtk::Menu menu_popup;
	bool menu_active = false;
};


#endif // _TREE_VIEW_H_

