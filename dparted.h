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

#ifndef _DPARTED_H_
#define _DPARTED_H_

#include <gtkmm/grid.h>
#include <gtkmm/menubar.h>
#include <gtkmm/radioaction.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/uimanager.h>
#include "gtkmm/applicationwindow.h"

#include "treeview.h"

class Container;

/**
 * class DParted
 */
class DParted : public Gtk::ApplicationWindow
{
public:
	DParted();
	virtual ~DParted();

	void set_data (ContainerPtr c);

protected:
	// GUI
	Gtk::Grid		grid;
	Gtk::MenuBar		menubar;
	//Gtk::Toolbar		toolbar;
	Gtk::Grid		da_grid;
	//DrawingArea		drawingarea;
	TreeView		treeview;
	//Gtk::Statusbar		statusbar;

	Gtk::ScrolledWindow	scrolledwindow;

	//YYY Glib::RefPtr<Gtk::UIManager> m_refUIManager;
	//YYY Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;
	//YYY Glib::RefPtr<Gtk::RadioAction> m_refChoiceOne, m_refChoiceTwo;

	//Signal handlers:
	void on_menu_file_new_generic();
	void on_menu_file_quit();
	void on_menu_others();

	void on_menu_choices_one();
	void on_menu_choices_two();

	bool on_mouse_click (GdkEventButton* event);

	ContainerPtr m_c;

private:
	void init_menubar (void);
#if 0
	void init_toolbar (void);
	void init_scrolledwindow (void);
#endif
};


#endif // _DPARTED_H_

