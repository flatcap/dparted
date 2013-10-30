/* Copyright (c) 2013 Richard Russon (FlatCap)
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

#include <gtkmm/stock.h>
#include <gtkmm.h>

#include <iostream>

#include "dparted.h"
#include "log.h"
#include "log_trace.h"

/**
 * Dparted
 */
DParted::DParted () :
	m_c (nullptr)
{
	set_title ("DParted");
	set_size_request (1324, 120);
	//set_size_request (1900, 1000);
#if 0
	set_default_size (1439, 800); //RAR 1439, 800
#endif

	add (scrolledwindow);

	scrolledwindow.add (grid);
	scrolledwindow.set_hexpand (true);
	scrolledwindow.set_vexpand (true);

	init_menubar();
	//init_toolbar();
	//init_scrolledwindow();

	grid.set_orientation (Gtk::ORIENTATION_VERTICAL);
	//grid.set_row_homogeneous (false);
	//grid.add (menubar);

	da_grid.set_orientation (Gtk::ORIENTATION_VERTICAL);

#if 0
	//Get the menubar and toolbar widgets, and add them to a container widget:
	Gtk::Widget *pMenubar = m_refUIManager->get_widget ("/MenuBar");
	if (pMenubar) {
		pMenubar->set_hexpand (true);
		grid.add (*pMenubar);
	}

	Gtk::Widget *pToolbar = m_refUIManager->get_widget ("/ToolBar") ;
	if (pToolbar) {
		pToolbar->set_hexpand (true);
		grid.add (*pToolbar);
	}
#endif

	//grid.add (toolbar);
	//grid.add (scrolledwindow);

	treeview.set_hexpand (true);

	grid.add (da_grid);
	//RAR grid.add (treeview);

	set_default_icon_name ("dparted");

	show_all_children();

#if 0
	Glib::RefPtr<Gtk::Settings> s = get_settings();
	s->property_gtk_tooltip_timeout() = 3000;
#endif
}

/**
 * ~DParted
 */
DParted::~DParted()
{
}


/**
 * set_data
 */
void
DParted::set_data (DPContainer *c)
{
	m_c = c;
	treeview.init_treeview (m_c);

	int count = 0;
	for (auto i : c->children) {
		if (i->is_a ("lvm_group"))
			continue; //RAR for now ignore vg
		//std::cout << i->type << "\n";
		DPDrawingArea *da = manage (new DPDrawingArea());
		da_grid.add (*da);
		da_grid.show_all();
		//std::cout << i->device << "\n";
		da->set_data (i);
		count++;
	}

	//std::cout << m_c->children.size() << " children\n";
	//set_size_request (1362, 77*count+6); //RAR

#if 1
	int width = 0;
	int height = 0;
	get_size (width, height);
	//log_info ("width = %d, height = %d\n", width, height);
	//move (1920, 768 - height);
	move (0, 0);
	//move (480, 1080-height);
#endif

}

/**
 * init_menubar
 */
void
DParted::init_menubar (void)
{
#if 0 //YYY
	//Create actions for menus and toolbars:
	m_refActionGroup = Gtk::ActionGroup::create();

	//File|New sub menu:
	m_refActionGroup->add (Gtk::Action::create ("FileNewStandard", Gtk::Stock::NEW, "_New",     "Create a new file"), sigc::mem_fun (*this, &DParted::on_menu_file_new_generic));
	m_refActionGroup->add (Gtk::Action::create ("FileNewFoo",      Gtk::Stock::NEW, "New Foo",  "Create a new foo"),  sigc::mem_fun (*this, &DParted::on_menu_file_new_generic));
	m_refActionGroup->add (Gtk::Action::create ("FileNewGoo",      Gtk::Stock::NEW, "_New Goo", "Create a new goo"),  sigc::mem_fun (*this, &DParted::on_menu_file_new_generic));

	//File menu:
	m_refActionGroup->add (Gtk::Action::create ("FileMenu", "File"));
	//Sub-menu.
	m_refActionGroup->add (Gtk::Action::create ("FileNew",         Gtk::Stock::NEW));
	m_refActionGroup->add (Gtk::Action::create ("FileQuit",        Gtk::Stock::QUIT),                                 sigc::mem_fun (*this, &DParted::on_menu_file_quit));

	//Edit menu:
	m_refActionGroup->add (Gtk::Action::create ("EditMenu", "Edit"));
	m_refActionGroup->add (Gtk::Action::create ("EditCopy",        Gtk::Stock::COPY),                                 sigc::mem_fun (*this, &DParted::on_menu_others));
	m_refActionGroup->add (Gtk::Action::create ("EditPaste",       Gtk::Stock::PASTE),                                sigc::mem_fun (*this, &DParted::on_menu_others));
	m_refActionGroup->add (Gtk::Action::create ("EditSomething", "Something"), Gtk::AccelKey ("<control><alt>S"),     sigc::mem_fun (*this, &DParted::on_menu_others));


	//Choices menu, to demonstrate Radio items
	m_refActionGroup->add (Gtk::Action::create ("ChoicesMenu", "Choices"));
	Gtk::RadioAction::Group group_userlevel;
	m_refChoiceOne = Gtk::RadioAction::create (group_userlevel, "ChoiceOne", "One");
	m_refActionGroup->add (m_refChoiceOne, sigc::mem_fun (*this, &DParted::on_menu_choices_one));
	m_refChoiceTwo = Gtk::RadioAction::create (group_userlevel, "ChoiceTwo", "Two");
	m_refActionGroup->add (m_refChoiceTwo, sigc::mem_fun (*this, &DParted::on_menu_choices_two));

	//Help menu:
	m_refActionGroup->add (Gtk::Action::create ("HelpMenu", "Help"));
	m_refActionGroup->add (Gtk::Action::create ("HelpAbout", Gtk::Stock::HELP), sigc::mem_fun (*this, &DParted::on_menu_others));

	m_refUIManager = Gtk::UIManager::create();
	m_refUIManager->insert_action_group (m_refActionGroup);

	add_accel_group (m_refUIManager->get_accel_group());

	//Layout the actions in a menubar and toolbar:
	Glib::ustring ui_info =
		"<ui>"
		"	<menubar name='MenuBar'>"
		"		<menu action='FileMenu'>"
		"			<menu action='FileNew'>"
		"				<menuitem action='FileNewStandard'/>"
		"				<menuitem action='FileNewFoo'/>"
		"				<menuitem action='FileNewGoo'/>"
		"			</menu>"
		"			<separator/>"
		"			<menuitem action='FileQuit'/>"
		"		</menu>"
		"		<menu action='EditMenu'>"
		"			<menuitem action='EditCopy'/>"
		"			<menuitem action='EditPaste'/>"
		"			<menuitem action='EditSomething'/>"
		"		</menu>"
		"		<menu action='ChoicesMenu'>"
		"			<menuitem action='ChoiceOne'/>"
		"			<menuitem action='ChoiceTwo'/>"
		"		</menu>"
		"		<menu action='HelpMenu'>"
		"			<menuitem action='HelpAbout'/>"
		"		</menu>"
		"	</menubar>"
		"	<toolbar name='ToolBar'>"
		"		<toolitem action='FileNewStandard'/>"
		"		<toolitem action='FileQuit'/>"
		"	</toolbar>"
		"</ui>";

	try {
		m_refUIManager->add_ui_from_string (ui_info);
	} catch (const Glib::Error &ex) {
		std::cerr << "building menus failed: " << ex.what();
	}
#endif
}

#if 0
/**
 * init_toolbar
 */
void
DParted::init_toolbar (void)
{
}

/**
 * init_scrolledwindow
 */
void
DParted::init_scrolledwindow (void)
{
}

#endif

/**
 * on_menu_file_quit
 */
void
DParted::on_menu_file_quit()
{
	hide(); //Closes the main window to stop the Gtk::Main::run().
}

/**
 * on_menu_file_new_generic
 */
void
DParted::on_menu_file_new_generic()
{
	std::cout << "A File|New menu item was selected.\n";
}

/**
 * on_menu_others
 */
void
DParted::on_menu_others()
{
	std::cout << "A menu item was selected.\n";
}

/**
 * on_menu_choices_one
 */
void
DParted::on_menu_choices_one()
{
#if 0 //YYY
	Glib::ustring message;
	if (m_refChoiceOne->get_active())
		message = "Choice 1 was selected.";
	else
		message = "Choice 1 was deselected";

	std::cout << message << "\n";
#endif
}

/**
 * on_menu_choices_two
 */
void
DParted::on_menu_choices_two()
{
#if 0 //YYY
	Glib::ustring message;
	if (m_refChoiceTwo->get_active())
		message = "Choice 2 was selected.";
	else
		message = "Choice 2 was deselected";

	std::cout << message << "\n";
#endif
}


