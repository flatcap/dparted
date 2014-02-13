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

#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <gtkmm/applicationwindow.h>
#include <gtkmm/box.h>
#include <gtkmm/eventbox.h>
#include <gtkmm/menubar.h>
#include <gtkmm/radioaction.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/spinbutton.h>
#include <giomm/simpleactiongroup.h>

#include "tree_view.h"
#include "drawing_area.h"

class Container;

/**
 * class Window
 */
class Window : public Gtk::ApplicationWindow
{
public:
	Window();
	virtual ~Window();

	void set_data (GfxContainerPtr c);
	bool set_focus (GfxContainerPtr cont);
	GfxContainerPtr get_focus (void);

	void load_config (const std::string& filename);
	void load_theme  (const std::string& filename);
	void load_disk   (const std::string& filename);

	void set_geometry (int x, int y, int w, int h);

protected:
	Gtk::Box		outer_box;	//XXX dynamically create the ones we don't care about?
	Gtk::MenuBar*		menubar = nullptr;	//do not delete
	Gtk::Toolbar*		toolbar = nullptr;	//do not delete
	Gtk::EventBox		eventbox;
	Gtk::ScrolledWindow	scrolledwindow;
	Gtk::Box		inner_box;
	DrawingArea		drawingarea;
	TreeView		treeview;
	Gtk::Statusbar		statusbar;

	virtual bool on_delete_event (GdkEventAny* event);

	// MENU
	void init_menubar (Gtk::Box& box);
	Glib::RefPtr<Gtk::Builder> m_refBuilder;
	Glib::RefPtr<Gio::SimpleAction> m_refChoice;
	Glib::RefPtr<Gio::SimpleAction> m_refChoiceOther;
	Glib::RefPtr<Gio::SimpleAction> m_refToggle;

	Glib::RefPtr<Gio::SimpleAction> m_refViewGfx;
	Glib::RefPtr<Gio::SimpleAction> m_refViewTree;
	Glib::RefPtr<Gio::SimpleAction> m_refViewToolbar;
	Glib::RefPtr<Gio::SimpleAction> m_refViewStatus;

	void on_menu_choices(const Glib::ustring& parameter);
	void on_menu_choices_other(int parameter);
	void on_menu_file_new_generic (void);
	void on_menu_file_quit (void);
	void on_menu_others (void);
	void on_menu_toggle (void);
	void on_menu_view (int option);

	Gtk::Menu m_fake_menu;

	bool on_mouse_click (GdkEventButton* event);

	void on_keypress (int modifier, int key);
	void init_shortcuts (void);
	void init_actions (void);

	void my_realize (void);
	void my_show (void);
	bool my_idle (void);

	ContainerPtr m_c;
	GfxContainerPtr m_g;

	GfxContainerPtr focus;
	std::vector<GfxContainerPtr> selection;

	void insert_general_actions (std::string section, const std::vector<const char*>& commands);
	void on_action_general (std::string section, std::string name);

private:
};


#endif // _WINDOW_H_

