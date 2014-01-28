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
#include <giomm/simpleactiongroup.h>

#include <iostream>

#include "gui_app.h"
#include "window.h"
#include "drawing_area.h"
#include "log.h"
#include "log_trace.h"

/**
 * Window
 */
Window::Window()
{
	set_title ("DParted");

	// set_size_request (1360, 70*3);
	set_size_request (800, 700);
	// set_default_size (1439, 800);

	scrolledwindow.set_hexpand (true);
	scrolledwindow.set_vexpand (true);

	inner_box.set_orientation (Gtk::ORIENTATION_VERTICAL);

	treeview.set_hexpand (true);

	add_events (Gdk::KEY_PRESS_MASK | Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::LEAVE_NOTIFY_MASK);
#if 0
	signal_button_press_event().connect (sigc::mem_fun (*this, &Window::on_mouse_click));
#endif

	eventbox.set_events (Gdk::KEY_PRESS_MASK);
	eventbox.signal_key_press_event().connect (sigc::mem_fun (drawingarea, &DrawingArea::on_keypress), false);

	signal_realize().connect (sigc::mem_fun (*this, &Window::my_realize));
	signal_show().connect (sigc::mem_fun (*this, &Window::my_show));
	Glib::signal_idle().connect (sigc::mem_fun (*this, &Window::my_idle));

	set_default_icon_name ("dparted");

	init_shortcuts();

	outer_box.set_orientation (Gtk::ORIENTATION_VERTICAL);
	outer_box.set_homogeneous (false);

	//-------------------------------------

	add (outer_box);
		init_menubar (outer_box);
		// init_toolbar (outer_box);
		outer_box.add (eventbox);
			eventbox.add (scrolledwindow);
				scrolledwindow.add (inner_box);
					inner_box.pack_start (drawingarea, false, false);
					inner_box.pack_start (treeview,    true,  true);
		outer_box.pack_end (statusbar, false, false);

	//-------------------------------------

	bool tb = true;
	bool gx = true;
	bool tv = true;
	bool sb = true;
	ConfigFilePtr cfg = gui_app->get_config();
	if (cfg) {
		try { tb = cfg->get_bool ("display.tool_bar");   } catch (const char *msg) { std::cout << "notb\n"; }
		try { gx = cfg->get_bool ("display.graphics");   } catch (const char *msg) { std::cout << "nogx\n"; }
		try { tv = cfg->get_bool ("display.tree_view");  } catch (const char *msg) { std::cout << "notv\n"; }
		try { sb = cfg->get_bool ("display.status_bar"); } catch (const char *msg) { std::cout << "nosb\n"; }
	}

	show_all_children();

	//printf ("%d,%d,%d,%d\n", tb, gx, tv, sb);
	toolbar->set_visible (tb);
	drawingarea.set_visible (gx);
	treeview.set_visible (tv);
	statusbar.set_visible (sb);

#if 0
	Glib::RefPtr<Gtk::Settings> s = get_settings();
	s->property_gtk_tooltip_timeout() = 3000;
#endif
}

/**
 * ~Window
 */
Window::~Window()
{
}


/**
 * my_realize
 */
void
Window::my_realize (void)
{
	//LOG_TRACE;
	drawingarea.grab_focus();
}

/**
 * my_show
 */
void
Window::my_show (void)
{
	//LOG_TRACE;
}

/**
 * my_idle
 */
bool
Window::my_idle (void)
{
#if 1
	std::vector<std::string> files;
	ContainerPtr c = gui_app->scan (files);
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
Window::on_mouse_click (GdkEventButton* event)
{
	//std::cout << "Window: mouse click: (" << event->x << "," << event->y << ")\n";
	return true;
}

/**
 * set_focus
 *
 * Return: true  - focus has changed
 *	   false - focus is the same
 */
bool
Window::set_focus (GfxContainerPtr cont)
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

	treeview.set_focus (focus);
	drawingarea.set_focus (focus);

	//std::cout << "Focus: " << cont << std::endl;
	return true;
}

/**
 * get_focus
 */
GfxContainerPtr
Window::get_focus (void)
{
	return focus;
}

/**
 * set_data
 */
void
Window::set_data (GfxContainerPtr c)
{
	treeview.init_treeview(c);
	drawingarea.set_data(c);

	//std::cout << m_c->children.size() << " children\n";
	//set_size_request (1362, 77*count+6); //RAR

	int width = 0;
	int height = 0;
	get_size (width, height);
	//log_info ("width = %d, height = %d\n", width, height);
	//move (1920+1366-width, 768-height);
	move (1920-width, 0);
	//move (1366-width, 0);
}


/**
 * init_menubar
 */
void
Window::init_menubar (Gtk::Box& box)
{
	//Create actions for menus and toolbars:
	Glib::RefPtr<Gio::SimpleActionGroup> refActionGroup = Gio::SimpleActionGroup::create();

	refActionGroup->add_action ("newstandard", sigc::mem_fun (*this, &Window::on_menu_file_new_generic));
	refActionGroup->add_action ("newfoo",      sigc::mem_fun (*this, &Window::on_menu_file_new_generic));
	refActionGroup->add_action ("newgoo",      sigc::mem_fun (*this, &Window::on_menu_file_new_generic));

	refActionGroup->add_action ("quit",        sigc::mem_fun (*this, &Window::on_menu_file_quit));

	refActionGroup->add_action ("copy",        sigc::mem_fun (*this, &Window::on_menu_others));
	refActionGroup->add_action ("paste",       sigc::mem_fun (*this, &Window::on_menu_others));
	refActionGroup->add_action ("something",   sigc::mem_fun (*this, &Window::on_menu_others));

	//Choices menus, to demonstrate Radio items,
	//using our convenience methods for string and int radio values:
	m_refChoice      = refActionGroup->add_action_radio_string  ("choice",      sigc::mem_fun (*this, &Window::on_menu_choices),       "a");
	m_refChoiceOther = refActionGroup->add_action_radio_integer ("choiceother", sigc::mem_fun (*this, &Window::on_menu_choices_other), 1);
	m_refToggle      = refActionGroup->add_action_bool          ("sometoggle",  sigc::mem_fun (*this, &Window::on_menu_toggle),        false);

	m_refViewGfx     = refActionGroup->add_action_bool ("view.gfx",     sigc::bind<int> (sigc::mem_fun (*this, &Window::on_menu_view), 1), true);	//XXX these need to match the config values
	m_refViewTree    = refActionGroup->add_action_bool ("view.tree",    sigc::bind<int> (sigc::mem_fun (*this, &Window::on_menu_view), 2), true);
	m_refViewToolbar = refActionGroup->add_action_bool ("view.toolbar", sigc::bind<int> (sigc::mem_fun (*this, &Window::on_menu_view), 3), true);
	m_refViewStatus  = refActionGroup->add_action_bool ("view.status",  sigc::bind<int> (sigc::mem_fun (*this, &Window::on_menu_view), 4), true);

	//Help menu:
	refActionGroup->add_action ("about", sigc::mem_fun (*this, &Window::on_menu_others));

	insert_action_group ("example", refActionGroup);

	m_refBuilder = Gtk::Builder::create();

	//TODO: add_accel_group (m_refBuilder->get_accel_group());

	//Layout the actions in a menubar and toolbar:
	Glib::ustring ui_info =
		"<interface>"
		"	<menu id='menu-example'>"
		"		<submenu>"
		"			<attribute name='label' translatable='yes'>_File</attribute>"
		"			<section>"
		"				<item>"
		"					<attribute name='label' translatable='yes'>New _Standard</attribute>"
		"					<attribute name='action'>example.newstandard</attribute>"
		"					<attribute name='accel'>&lt;Primary&gt;n</attribute>"
		"				</item>"
		"				<item>"
		"					<attribute name='label' translatable='yes'>New _Foo</attribute>"
		"					<attribute name='action'>example.newfoo</attribute>"
		"				</item>"
		"				<item>"
		"					<attribute name='label' translatable='yes'>New _Goo</attribute>"
		"					<attribute name='action'>example.newgoo</attribute>"
		"				</item>"
		"			</section>"
		"			<section>"
		"				<item>"
		"					<attribute name='label' translatable='yes'>_Quit</attribute>"
		"					<attribute name='action'>example.quit</attribute>"
		"					<attribute name='accel'>&lt;Primary&gt;q</attribute>"
		"				</item>"
		"			</section>"
		"		</submenu>"
		"		<submenu>"
		"			<attribute name='label' translatable='yes'>_Edit</attribute>"
		"			<section>"
		"				<item>"
		"					<attribute name='label' translatable='yes'>_Copy</attribute>"
		"					<attribute name='action'>example.copy</attribute>"
		"					<attribute name='accel'>&lt;Primary&gt;c</attribute>"
		"				</item>"
		"				<item>"
		"					<attribute name='label' translatable='yes'>_Paste</attribute>"
		"					<attribute name='action'>example.paste</attribute>"
		"					<attribute name='accel'>&lt;Primary&gt;v</attribute>"
		"				</item>"
		"				<item>"
		"					<attribute name='label' translatable='yes'>_Something</attribute>"
		"					<attribute name='action'>example.something</attribute>"
		"				</item>"
		"			</section>"
		"		</submenu>"
		"		<submenu>"
		"			<attribute name='label' translatable='yes'>_View</attribute>"
		"			<section>"
		"				<item>"
		"					<attribute name='label' translatable='yes'>_Graphics</attribute>"
		"					<attribute name='action'>example.view.gfx</attribute>"
		"				</item>"
		"				<item>"
		"					<attribute name='label' translatable='yes'>_Tree View</attribute>"
		"					<attribute name='action'>example.view.tree</attribute>"
		"				</item>"
		"			</section>"
		"			<section>"
		"				<item>"
		"					<attribute name='label' translatable='yes'>Tool_bar</attribute>"
		"					<attribute name='action'>example.view.toolbar</attribute>"
		"				</item>"
		"				<item>"
		"					<attribute name='label' translatable='yes'>_Status bar</attribute>"
		"					<attribute name='action'>example.view.status</attribute>"
		"				</item>"
		"			</section>"
		"		</submenu>"
		"		<submenu>"
		"			<attribute name='label' translatable='yes'>_Choices</attribute>"
		"			<section>"
		"				<item>"
		"					<attribute name='label' translatable='yes'>Choice _A</attribute>"
		"					<attribute name='action'>example.choice</attribute>"
		"					<attribute name='target'>a</attribute>"
		"				</item>"
		"				<item>"
		"					<attribute name='label' translatable='yes'>Choice _B</attribute>"
		"					<attribute name='action'>example.choice</attribute>"
		"					<attribute name='target'>b</attribute>"
		"				</item>"
		"			</section>"
		"		</submenu>"
		"		<submenu>"
		"			<attribute name='label' translatable='yes'>_Other Choices</attribute>"
		"			<section>"
		"				<item>"
		"					<attribute name='label' translatable='yes'>Choice 1</attribute>"
		"					<attribute name='action'>example.choiceother</attribute>"
		"					<attribute name='target' type='i'>1</attribute>"
		"				</item>"
		"				<item>"
		"					<attribute name='label' translatable='yes'>Choice 2</attribute>"
		"					<attribute name='action'>example.choiceother</attribute>"
		"					<attribute name='target' type='i'>2</attribute>"
		"				</item>"
		"			</section>"
		"			<section>"
		"				<item>"
		"					<attribute name='label' translatable='yes'>Some Toggle</attribute>"
		"					<attribute name='action'>example.sometoggle</attribute>"
		"				</item>"
		"			</section>"
		"		</submenu>"
		"		<submenu>"
		"			<attribute name='label' translatable='yes'>_Help</attribute>"
		"			<section>"
		"				<item>"
		"					<attribute name='label' translatable='yes'>_About</attribute>"
		"					<attribute name='action'>example.about</attribute>"
		"				</item>"
		"			</section>"
		"		</submenu>"
		"	</menu>"
		"</interface>";

	try {
		m_refBuilder->add_from_string (ui_info);
	} catch (const Glib::Error& ex) {
		std::cerr << "building menus failed: " << ex.what();
	}

	//Get the menubar and add it to a container widget:
	Glib::RefPtr<Glib::Object> object = m_refBuilder->get_object ("menu-example");
	Glib::RefPtr<Gio::Menu> gmenu = Glib::RefPtr<Gio::Menu>::cast_dynamic (object);
	if (!gmenu)
		g_warning ("GMenu not found");

	//Menubar:
	menubar = Gtk::manage (new Gtk::MenuBar (gmenu));
	box.pack_start (*menubar, Gtk::PACK_SHRINK);

	//Create the toolbar and add it to a container widget:
	toolbar = Gtk::manage (new Gtk::Toolbar());
	Gtk::ToolButton* button = Gtk::manage (new Gtk::ToolButton());
	button->set_icon_name ("document-new");
	//We can't do this until we can break the ToolButton ABI: button->set_detailed_action_name ("example.new");
	gtk_actionable_set_detailed_action_name (GTK_ACTIONABLE (button->gobj()), "example.newstandard");
	toolbar->add (*button);

	button = Gtk::manage (new Gtk::ToolButton());
	button->set_icon_name ("application-exit");
	//We can't do this until we can break the ToolButton ABI: button->set_detailed_action_name ("example.quit");
	gtk_actionable_set_detailed_action_name (GTK_ACTIONABLE (button->gobj()), "example.quit");
	toolbar->add (*button);

	box.pack_start (*toolbar, Gtk::PACK_SHRINK);
}


/**
 * on_menu_choices
 */
void
Window::on_menu_choices (const Glib::ustring& parameter)
{
	//The radio action's state does not change automatically:
	m_refChoice->change_state (parameter);

	Glib::ustring message;
	if (parameter == "a")
		message = "Choice a was selected.";
	else
		message = "Choice b was selected";

	std::cout << message << std::endl;
}

/**
 * on_menu_choices_other
 */
void
Window::on_menu_choices_other (int parameter)
{
	//The radio action's state does not change automatically:
	m_refChoiceOther->change_state (parameter);

	Glib::ustring message;
	if (parameter == 1)
		message = "Choice 1 was selected.";
	else
		message = "Choice 2 was selected";

	std::cout << message << std::endl;
}

/**
 * on_menu_file_new_generic
 */
void
Window::on_menu_file_new_generic (void)
{
	std::cout << "A File|New menu item was selected." << std::endl;
}

/**
 * on_menu_file_quit
 */
void
Window::on_menu_file_quit (void)
{
	bool ask_user = true;
	bool quit_app = false;

	ConfigFilePtr cfg = gui_app->get_config();
	if (cfg) {
		ask_user = cfg->get_bool ("confirmation.quit_application");
	}

	if (ask_user) {
		ContainerPtr c;
		QuestionPtr q = Question::create (c, [] (QuestionPtr q) { std::cout << "reply\n"; } );
		q->title = "Quit Application?";
		q->question = "Are you sure?";
		q->answers = { "No", "Yes" };
		gui_app->ask (q);

		quit_app = (q->result == 1);
	} else {
		quit_app = true;
	}

	if (quit_app) {
		hide();
	}
}

/**
 * on_menu_others
 */
void
Window::on_menu_others (void)
{
	std::cout << "A menu item was selected." << std::endl;
}

/**
 * on_menu_toggle
 */
void
Window::on_menu_toggle (void)
{
	bool active = false;
	m_refToggle->get_state (active);

	//The toggle action's state does not change automatically:
	m_refToggle->change_state (!active);
	active = !active;

	Glib::ustring message;
	if (active)
		message = "Toggle is active.";
	else
		message = "Toggle is not active";

	std::cout << message << std::endl;
}

/**
 * on_menu_view
 */
void
Window::on_menu_view (int option)
{
	std::cout << "on_menu_view: " << option << std::endl;

	bool val = false;
	switch (option) {
		case 1:
			m_refViewGfx ->get_state (val);
			val = !val;
			m_refViewGfx->change_state (val);
			if (val)
				drawingarea.show_all();
			else
				drawingarea.hide();
			break;
		case 2:
			m_refViewTree->get_state (val);
			val = !val;
			m_refViewTree->change_state (val);
			if (val)
				treeview.show_all();
			else
				treeview.hide();
			break;
		case 3:
			m_refViewToolbar->get_state (val);
			val = !val;
			m_refViewToolbar->change_state (val);
			if (val)
				toolbar->show_all();
			else
				toolbar->hide();
			break;
		case 4:
			m_refViewStatus->get_state (val);
			val = !val;
			m_refViewStatus->change_state (val);
			if (val)
				statusbar.show_all();
			else
				statusbar.hide();
			break;
	}
}

/**
 * init_shortcuts
 */
void
Window::init_shortcuts (void)
{
	std::vector<std::pair<int,int>> keys = {
		{ 0,                 GDK_KEY_Left   },
		{ 0,                 GDK_KEY_Up     },
		{ 0,                 GDK_KEY_Right  },
		{ 0,                 GDK_KEY_Down   },

		//{ 0,                 GDK_KEY_Tab    },
		//{ 0,                 GDK_KEY_space  },
		//{ 0,                 GDK_KEY_Return },

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
		i->signal_activate().connect (sigc::bind<int,int> (sigc::mem_fun (*this, &Window::on_keypress), k.first, k.second));
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
Window::on_keypress (int modifier, int key)
{
	std::cout << "Keypress: " << modifier << " : " << (char) key << std::endl;

	if ((modifier == Gdk::CONTROL_MASK) && (key == 'Q'))
		hide();
}

