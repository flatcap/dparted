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

#include <gtkmm/stock.h>
#include <gtkmm.h>

#include <iostream>

#include "app.h"
#include "dparted.h"
#include "drawing_area.h"
#include "log.h"
#include "log_trace.h"

/**
 * Dparted
 */
DParted::DParted ()
{
	set_title ("DParted");
#if 0
	set_size_request (1360, 70*3);
#else
	set_size_request (800, 500);
#endif
#if 0
	set_default_size (1439, 800);
#endif

	init_menubar();
	init_toolbar();
	init_scrolledwindow();

	scrolledwindow.set_hexpand (true);
	scrolledwindow.set_vexpand (true);

	box.set_orientation (Gtk::ORIENTATION_VERTICAL);

#if 1
	//Get the menubar and toolbar widgets, and add them to a container widget:
	Gtk::Widget* pMenubar = m_refUIManager->get_widget ("/MenuBar");
	if (pMenubar) {
		pMenubar->set_hexpand (true);
		//box.add (*pMenubar);
	}

	Gtk::Widget* pToolbar = m_refUIManager->get_widget ("/ToolBar") ;
	if (pToolbar) {
		pToolbar->set_hexpand (true);
		//box.add (*pToolbar);
	}
#endif

	treeview.set_hexpand (true);

	add_events (Gdk::KEY_PRESS_MASK | Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::LEAVE_NOTIFY_MASK);
#if 0
	signal_button_press_event().connect (sigc::mem_fun (*this, &DParted::on_mouse_click));
#endif

	eventbox.set_events(Gdk::KEY_PRESS_MASK);
	eventbox.signal_key_press_event().connect (sigc::mem_fun(drawingarea, &DrawingArea::on_keypress), false);

	signal_realize().connect (sigc::mem_fun (*this, &DParted::my_realize));
	signal_realize().connect (sigc::mem_fun (*this, &DParted::my_show));
	Glib::signal_idle().connect (sigc::mem_fun (*this, &DParted::my_idle));

	set_default_icon_name ("dparted");

	//init_shortcuts();

	outer_box.set_orientation (Gtk::ORIENTATION_VERTICAL);

	add (outer_box);

	outer_box.set_homogeneous (false);
	outer_box.pack_start (*pMenubar, false, false);
	outer_box.pack_start (*pToolbar, false, false);
	outer_box.add (scrolledwindow);
	outer_box.pack_end (statusbar, false, false);

	scrolledwindow.add (box);
	eventbox.add(drawingarea);
	box.pack_start (eventbox, false, false);
	box.pack_end (treeview, true, true);

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
 * my_realize
 */
void
DParted::my_realize (void)
{
	//LOG_TRACE;
	drawingarea.grab_focus();
}

/**
 * my_show
 */
void
DParted::my_show (void)
{
	//LOG_TRACE;
}

/**
 * my_idle
 */
bool
DParted::my_idle (void)
{
#if 1
	std::vector<std::string> files;
	ContainerPtr c = main_app->scan (files);
	//std::cout << c->get_children().size() << " children\n";
	GfxContainerPtr dummy;
	m_g = GfxContainer::create (dummy, c);
	set_data (m_g);
#endif

	return false;
}


/**
 * on_mouse_click
 */
bool
DParted::on_mouse_click (GdkEventButton* event)
{
	std::cout << "DParted: mouse click: (" << event->x << "," << event->y << ")\n";
	return true;
}

/**
 * set_focus
 *
 * Return: true  - focus has changed
 *	   false - focus is the same
 */
bool
DParted::set_focus (GfxContainerPtr cont)
{
	if (!cont)
		return false;

	if (focus == cont)
		return false;

	if (focus) {
		focus->set_focus (false);
	}
	cont->set_focus (true);
	focus = cont;

	std::cout << "Focus: " << cont << std::endl;
	return true;
}

/**
 * get_focus
 */
GfxContainerPtr
DParted::get_focus (void)
{
	return focus;
}

/**
 * set_data
 */
void
DParted::set_data (GfxContainerPtr c)
{
	treeview.init_treeview (c);
	drawingarea.set_data (c);

	//std::cout << m_c->children.size() << " children\n";
	//set_size_request (1362, 77*count+6); //RAR

	int width = 0;
	int height = 0;
	get_size (width, height);
	//log_info ("width = %d, height = %d\n", width, height);
	move (1920+1366-width, 0);
	//move (1920-width, 0);
}


/**
 * init_menubar
 */
void
DParted::init_menubar (void)
{
	//Create actions for menus and toolbars:
	m_refActionGroup = Gtk::ActionGroup::create();

	//File|New sub menu:
	m_refActionGroup->add (Gtk::Action::create ("FileNewStandard", Gtk::Stock::NEW, "_New",     "Create a new file"), sigc::mem_fun (*this, &DParted::on_menu_file_new_generic));
	m_refActionGroup->add (Gtk::Action::create ("FileNewFoo",      Gtk::Stock::NEW, "New Foo",  "Create a new foo"),  sigc::mem_fun (*this, &DParted::on_menu_file_new_generic));
	m_refActionGroup->add (Gtk::Action::create ("FileNewGoo",      Gtk::Stock::NEW, "_New Goo", "Create a new goo"),  sigc::mem_fun (*this, &DParted::on_menu_file_new_generic));

	//File menu:
	m_refActionGroup->add (Gtk::Action::create ("FileMenu", "_File"));
	//Sub-menu.
	m_refActionGroup->add (Gtk::Action::create ("FileNew",         Gtk::Stock::NEW));
	m_refActionGroup->add (Gtk::Action::create ("FileQuit",        Gtk::Stock::QUIT),                                 sigc::mem_fun (*this, &DParted::on_menu_file_quit));

	//Edit menu:
	m_refActionGroup->add (Gtk::Action::create ("EditMenu", "_Edit"));
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
	m_refActionGroup->add (Gtk::Action::create ("HelpMenu", "_Help"));
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
	} catch (const Glib::Error& ex) {
		std::cerr << "building menus failed: " << ex.what();
	}
}

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


/**
 * on_menu_file_quit
 */
void
DParted::on_menu_file_quit()
{
	//LOG_TRACE;
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
	std::string message;
	if (m_refChoiceOne->get_active())
		message = "Choice 1 was selected.";
	else
		message = "Choice 1 was deselected";

	std::cout << message << "\n";
}

/**
 * on_menu_choices_two
 */
void
DParted::on_menu_choices_two()
{
	std::string message;
	if (m_refChoiceTwo->get_active())
		message = "Choice 2 was selected.";
	else
		message = "Choice 2 was deselected";

	std::cout << message << "\n";
}


/**
 * init_shortcuts
 */
void
DParted::init_shortcuts (void)
{
	std::vector<std::pair<int,int>> keys = {
		{ 0,                 GDK_KEY_Left   },
		{ 0,                 GDK_KEY_Up     },
		{ 0,                 GDK_KEY_Right  },
		{ 0,                 GDK_KEY_Down   },

		{ 0,                 GDK_KEY_Tab    },
		{ 0,                 GDK_KEY_space  },
		{ 0,                 GDK_KEY_Return },

		{ Gdk::CONTROL_MASK, GDK_KEY_C      },
		{ Gdk::CONTROL_MASK, GDK_KEY_F      },
		{ Gdk::CONTROL_MASK, GDK_KEY_O      },
		{ Gdk::CONTROL_MASK, GDK_KEY_Q      },
		{ Gdk::CONTROL_MASK, GDK_KEY_V      },
		{ Gdk::CONTROL_MASK, GDK_KEY_X      },
		{ Gdk::CONTROL_MASK, GDK_KEY_Z      }
	};

	Glib::RefPtr<Gtk::AccelGroup> accel = Gtk::AccelGroup::create();

	for (auto k : keys) {
		//std::cout << "Keypress: " << k.first << " : " << k.second << std::endl;
		Gtk::MenuItem* i = manage (new Gtk::MenuItem());
		i->signal_activate().connect (sigc::bind<int,int> (sigc::mem_fun (*this, &DParted::on_keypress), k.first, k.second));
		i->add_accelerator ("activate", accel, k.second, (Gdk::ModifierType) k.first, Gtk::ACCEL_VISIBLE);
		i->show();
		m_fake_menu.append (*i);
	}

	add_accel_group (accel);

}

/**
 * on_keypress
 */
void
DParted::on_keypress (int modifier, int key)
{
	std::cout << "Keypress: " << modifier << " : " << (char) key << std::endl;

	if ((modifier == Gdk::CONTROL_MASK) && (key == 'Q'))
		hide();
}


