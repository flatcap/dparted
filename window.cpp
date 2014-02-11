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
	set_size_request (800, 600);
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
	init_actions();

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
	move (1920+1366-width, 768-height);
	//move (1920-width, 0);
	//move (1366-width, 0);
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
 * init_actions
 */
void
Window::init_actions (void)
{
	Glib::RefPtr<Gio::SimpleActionGroup> ag_file = Gio::SimpleActionGroup::create();
	ag_file->add_action("open",         sigc::mem_fun(*gui_app, &GuiApp::on_action_file_open));
	ag_file->add_action("close",        sigc::mem_fun(*gui_app, &GuiApp::on_action_file_close));
	ag_file->add_action("quit",         sigc::mem_fun(*gui_app, &GuiApp::on_action_file_quit));
	insert_action_group ("file", ag_file);

	Glib::RefPtr<Gio::SimpleActionGroup> ag_edit = Gio::SimpleActionGroup::create();
	ag_edit->add_action("cut",           sigc::mem_fun(*this, &Window::on_action_edit_cut));
	ag_edit->add_action("copy",          sigc::mem_fun(*this, &Window::on_action_edit_copy));
	ag_edit->add_action("paste",         sigc::mem_fun(*this, &Window::on_action_edit_paste));
	ag_edit->add_action("paste_special", sigc::mem_fun(*this, &Window::on_action_edit_paste_special));
	ag_edit->add_action("undo",          sigc::mem_fun(*this, &Window::on_action_edit_undo));
	ag_edit->add_action("redo",          sigc::mem_fun(*this, &Window::on_action_edit_redo));
	ag_edit->add_action("clear_all_ops", sigc::mem_fun(*this, &Window::on_action_edit_clear_all_ops));
	ag_edit->add_action("apply_all_ops", sigc::mem_fun(*this, &Window::on_action_edit_apply_all_ops));
	ag_edit->add_action("find",          sigc::mem_fun(*this, &Window::on_action_edit_find));
	ag_edit->add_action("find_next",     sigc::mem_fun(*this, &Window::on_action_edit_find_next));
	ag_edit->add_action("find_previous", sigc::mem_fun(*this, &Window::on_action_edit_find_previous));
	ag_edit->add_action("preferences",   sigc::mem_fun(*this, &Window::on_action_edit_preferences));
	insert_action_group ("edit", ag_edit);

	Glib::RefPtr<Gio::SimpleActionGroup> ag_view = Gio::SimpleActionGroup::create();
	ag_view->add_action("toolbar",         sigc::mem_fun(*this, &Window::on_action_view_toolbar));
	ag_view->add_action("graphics",        sigc::mem_fun(*this, &Window::on_action_view_graphics));
	ag_view->add_action("tree_view",       sigc::mem_fun(*this, &Window::on_action_view_tree_view));
	ag_view->add_action("status_bar",      sigc::mem_fun(*this, &Window::on_action_view_status_bar));
	ag_view->add_action("log",             sigc::mem_fun(*this, &Window::on_action_view_log));
	ag_view->add_action("pending_actions", sigc::mem_fun(*this, &Window::on_action_view_pending_actions));
	ag_view->add_action("fullscreen",      sigc::mem_fun(*this, &Window::on_action_view_fullscreen));
	ag_view->add_action("refresh",         sigc::mem_fun(*this, &Window::on_action_view_refresh));
	ag_view->add_action("reload",          sigc::mem_fun(*this, &Window::on_action_view_reload));
	ag_view->add_action("theme",           sigc::mem_fun(*this, &Window::on_action_view_theme));
	insert_action_group ("view", ag_view);

	Glib::RefPtr<Gio::SimpleActionGroup> ag_create = Gio::SimpleActionGroup::create();
	ag_create->add_action("filesystem", sigc::mem_fun(*this, &Window::on_action_create_filesystem));
	ag_create->add_action("partition",  sigc::mem_fun(*this, &Window::on_action_create_partition));
	ag_create->add_action("table",      sigc::mem_fun(*this, &Window::on_action_create_table));
	ag_create->add_action("encryption", sigc::mem_fun(*this, &Window::on_action_create_encryption));
	ag_create->add_action("lvm_volume", sigc::mem_fun(*this, &Window::on_action_create_lvm_volume));
	ag_create->add_action("subvolume",  sigc::mem_fun(*this, &Window::on_action_create_subvolume));
	ag_create->add_action("snapshot",   sigc::mem_fun(*this, &Window::on_action_create_snapshot));
	insert_action_group ("create", ag_create);

	Glib::RefPtr<Gio::SimpleActionGroup> ag_delete = Gio::SimpleActionGroup::create();
	ag_delete->add_action("filesystem", sigc::mem_fun(*this, &Window::on_action_delete_filesystem));
	ag_delete->add_action("partition",  sigc::mem_fun(*this, &Window::on_action_delete_partition));
	ag_delete->add_action("table",      sigc::mem_fun(*this, &Window::on_action_delete_table));
	ag_delete->add_action("encryption", sigc::mem_fun(*this, &Window::on_action_delete_encryption));
	ag_delete->add_action("lvm_volume", sigc::mem_fun(*this, &Window::on_action_delete_lvm_volume));
	ag_delete->add_action("subvolume",  sigc::mem_fun(*this, &Window::on_action_delete_subvolume));
	ag_delete->add_action("snapshot",   sigc::mem_fun(*this, &Window::on_action_delete_snapshot));
	insert_action_group ("delete", ag_delete);

	Glib::RefPtr<Gio::SimpleActionGroup> ag_format = Gio::SimpleActionGroup::create();
	ag_format->add_action("wipe",           sigc::mem_fun(*this, &Window::on_action_format_wipe));
	ag_format->add_action("filesystem",     sigc::mem_fun(*this, &Window::on_action_format_filesystem));
	ag_format->add_action("btrfs",          sigc::mem_fun(*this, &Window::on_action_format_btrfs));
	ag_format->add_action("partition_type", sigc::mem_fun(*this, &Window::on_action_format_partition_type));
	ag_format->add_action("table",          sigc::mem_fun(*this, &Window::on_action_format_table));
	insert_action_group ("format", ag_format);

	Glib::RefPtr<Gio::SimpleActionGroup> ag_manage = Gio::SimpleActionGroup::create();
	ag_manage->add_action("properties", sigc::mem_fun(*this, &Window::on_action_manage_properties));
	ag_manage->add_action("label",      sigc::mem_fun(*this, &Window::on_action_manage_label));
	ag_manage->add_action("uuid",       sigc::mem_fun(*this, &Window::on_action_manage_uuid));
	ag_manage->add_action("flags",      sigc::mem_fun(*this, &Window::on_action_manage_flags));
	ag_manage->add_action("parameters", sigc::mem_fun(*this, &Window::on_action_manage_parameters));
	insert_action_group ("manage", ag_manage);

	Glib::RefPtr<Gio::SimpleActionGroup> ag_filesystem = Gio::SimpleActionGroup::create();
	ag_filesystem->add_action("check",       sigc::mem_fun(*this, &Window::on_action_filesystem_check));
	ag_filesystem->add_action("defragment",  sigc::mem_fun(*this, &Window::on_action_filesystem_defragment));
	ag_filesystem->add_action("rebalance",   sigc::mem_fun(*this, &Window::on_action_filesystem_rebalance));
	ag_filesystem->add_action("resize_move", sigc::mem_fun(*this, &Window::on_action_filesystem_resize_move));
	ag_filesystem->add_action("mount",       sigc::mem_fun(*this, &Window::on_action_filesystem_mount));
	ag_filesystem->add_action("umount",      sigc::mem_fun(*this, &Window::on_action_filesystem_umount));
	ag_filesystem->add_action("swap_on",     sigc::mem_fun(*this, &Window::on_action_filesystem_swap_on));
	ag_filesystem->add_action("swap_off",    sigc::mem_fun(*this, &Window::on_action_filesystem_swap_off));
	ag_filesystem->add_action("usage",       sigc::mem_fun(*this, &Window::on_action_filesystem_usage));
	insert_action_group ("filesystem", ag_filesystem);

	Glib::RefPtr<Gio::SimpleActionGroup> ag_group = Gio::SimpleActionGroup::create();
	ag_group->add_action("resize",        sigc::mem_fun(*this, &Window::on_action_group_resize));
	ag_group->add_action("split",         sigc::mem_fun(*this, &Window::on_action_group_split));
	ag_group->add_action("merge",         sigc::mem_fun(*this, &Window::on_action_group_merge));
	ag_group->add_action("add_stripe",    sigc::mem_fun(*this, &Window::on_action_group_add_stripe));
	ag_group->add_action("remove_stripe", sigc::mem_fun(*this, &Window::on_action_group_remove_stripe));
	ag_group->add_action("add_mirror",    sigc::mem_fun(*this, &Window::on_action_group_add_mirror));
	ag_group->add_action("remove_mirror", sigc::mem_fun(*this, &Window::on_action_group_remove_mirror));
	ag_group->add_action("break_mirror",  sigc::mem_fun(*this, &Window::on_action_group_break_mirror));
	ag_group->add_action("add_raid",      sigc::mem_fun(*this, &Window::on_action_group_add_raid));
	ag_group->add_action("remove_raid",   sigc::mem_fun(*this, &Window::on_action_group_remove_raid));
	insert_action_group ("group", ag_group);

	Glib::RefPtr<Gio::SimpleActionGroup> ag_plugin = Gio::SimpleActionGroup::create();
	ag_plugin->add_action("x",          sigc::mem_fun(*gui_app, &GuiApp::on_action_plugin));
	ag_plugin->add_action("y",          sigc::mem_fun(*gui_app, &GuiApp::on_action_plugin));
	ag_plugin->add_action("configure",  sigc::mem_fun(*gui_app, &GuiApp::on_action_plugin));
	insert_action_group ("plugin", ag_plugin);

	Glib::RefPtr<Gio::SimpleActionGroup> ag_help = Gio::SimpleActionGroup::create();
	ag_help->add_action("contents",     sigc::mem_fun(*gui_app, &GuiApp::on_action_help));
	ag_help->add_action("whats_this",   sigc::mem_fun(*gui_app, &GuiApp::on_action_help));
	ag_help->add_action("report_issue", sigc::mem_fun(*gui_app, &GuiApp::on_action_help));
	ag_help->add_action("faq",          sigc::mem_fun(*gui_app, &GuiApp::on_action_help));
	ag_help->add_action("community",    sigc::mem_fun(*gui_app, &GuiApp::on_action_help));
	ag_help->add_action("homepage",     sigc::mem_fun(*gui_app, &GuiApp::on_action_help));
	ag_help->add_action("about",        sigc::mem_fun(*gui_app, &GuiApp::on_action_help));
	insert_action_group ("help", ag_help);
}

/**
 * init_menubar
 */
void
Window::init_menubar (Gtk::Box& box)
{
	m_refBuilder = Gtk::Builder::create();

	//Layout the actions in a menubar and toolbar:
	Glib::ustring ui_info =
		"<interface><menu id='menu-example'>"
		"<submenu>"
		"	<attribute name='label' translatable='yes'>_File</attribute>"
		"	<section>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Open...</attribute>"
		"			<attribute name='action'>file.open</attribute>"
		"		</item>"
		"	</section>"
		"	<section>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Close</attribute>"
		"			<attribute name='action'>file.close</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Quit</attribute>"
		"			<attribute name='action'>file.quit</attribute>"
		"		</item>"
		"	</section>"
		"</submenu>"
		"<submenu>"
		"	<attribute name='label' translatable='yes'>_Edit</attribute>"
		"	<section>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Cut</attribute>"
		"			<attribute name='action'>edit.cut</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Copy</attribute>"
		"			<attribute name='action'>edit.copy</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Paste</attribute>"
		"			<attribute name='action'>edit.paste</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Paste Special...</attribute>"
		"			<attribute name='action'>edit.paste_special</attribute>"
		"		</item>"
		"	</section>"
		"	<section>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Undo</attribute>"
		"			<attribute name='action'>edit.undo</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Redo</attribute>"
		"			<attribute name='action'>edit.redo</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Clear all ops</attribute>"
		"			<attribute name='action'>edit.clear_all_ops</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Apply all ops...</attribute>"
		"			<attribute name='action'>edit.apply_all_ops</attribute>"
		"		</item>"
		"	</section>"
		"	<section>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Find...</attribute>"
		"			<attribute name='action'>edit.find</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Find next</attribute>"
		"			<attribute name='action'>edit.find_next</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Find previous</attribute>"
		"			<attribute name='action'>edit.find_previous</attribute>"
		"		</item>"
		"	</section>"
		"	<section>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Preferences...</attribute>"
		"			<attribute name='action'>edit.preferences</attribute>"
		"		</item>"
		"	</section>"
		"</submenu>"
		"<submenu>"
		"	<attribute name='label' translatable='yes'>_View</attribute>"
		"	<section>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Toolbar</attribute>"
		"			<attribute name='action'>view.toolbar</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Graphics</attribute>"
		"			<attribute name='action'>view.graphics</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Tree view</attribute>"
		"			<attribute name='action'>view.tree_view</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Status bar</attribute>"
		"			<attribute name='action'>view.status_bar</attribute>"
		"		</item>"
		"	</section>"
		"	<section>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Log</attribute>"
		"			<attribute name='action'>view.log</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Pending Actions</attribute>"
		"			<attribute name='action'>view.pending_actions</attribute>"
		"		</item>"
		"	</section>"
		"	<section>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Fullscreen</attribute>"
		"			<attribute name='action'>view.fullscreen</attribute>"
		"		</item>"
		"	</section>"
		"	<section>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Refresh</attribute>"
		"			<attribute name='action'>view.refresh</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Reload</attribute>"
		"			<attribute name='action'>view.reload</attribute>"
		"		</item>"
		"	</section>"
		"	<section>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Theme</attribute>"
		"			<attribute name='action'>view.theme</attribute>"
		"		</item>"
		"	</section>"
		"</submenu>"
		"<submenu>"
		"	<attribute name='label' translatable='yes'>_Create</attribute>"
		"	<section>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Filesystem...</attribute>"
		"			<attribute name='action'>create.filesystem</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Partition...</attribute>"
		"			<attribute name='action'>create.partition</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Table...</attribute>"
		"			<attribute name='action'>create.table</attribute>"
		"		</item>"
		"	</section>"
		"	<section>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Encryption...</attribute>"
		"			<attribute name='action'>create.encryption</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Lvm Volume...</attribute>"
		"			<attribute name='action'>create.lvm_volume</attribute>"
		"		</item>"
		"	</section>"
		"	<section>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Subvolume...</attribute>"
		"			<attribute name='action'>create.subvolume</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Snapshot...</attribute>"
		"			<attribute name='action'>create.snapshot</attribute>"
		"		</item>"
		"	</section>"
		"</submenu>"
		"<submenu>"
		"	<attribute name='label' translatable='yes'>_Delete</attribute>"
		"	<section>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Filesystem</attribute>"
		"			<attribute name='action'>delete.filesystem</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Partition</attribute>"
		"			<attribute name='action'>delete.partition</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Table</attribute>"
		"			<attribute name='action'>delete.table</attribute>"
		"		</item>"
		"	</section>"
		"	<section>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Encryption</attribute>"
		"			<attribute name='action'>delete.encryption</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Lvm Volume</attribute>"
		"			<attribute name='action'>delete.lvm_volume</attribute>"
		"		</item>"
		"	</section>"
		"	<section>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Subvolume</attribute>"
		"			<attribute name='action'>delete.subvolume</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Snapshot</attribute>"
		"			<attribute name='action'>delete.snapshot</attribute>"
		"		</item>"
		"	</section>"
		"</submenu>"
		"<submenu>"
		"	<attribute name='label' translatable='yes'>F_ormat</attribute>"
		"	<section>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Wipe...</attribute>"
		"			<attribute name='action'>format.wipe</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Filesystem...</attribute>"
		"			<attribute name='action'>format.filesystem</attribute>"
		"		</item>"
		"	</section>"
		"	<section>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Btrfs...</attribute>"
		"			<attribute name='action'>format.btrfs</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Partition Type...</attribute>"
		"			<attribute name='action'>format.partition_type</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Table...</attribute>"
		"			<attribute name='action'>format.table</attribute>"
		"		</item>"
		"	</section>"
		"</submenu>"
		"<submenu>"
		"	<attribute name='label' translatable='yes'>_Manage</attribute>"
		"	<section>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Properties</attribute>"
		"			<attribute name='action'>manage.properties</attribute>"
		"		</item>"
		"	</section>"
		"	<section>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Label...</attribute>"
		"			<attribute name='action'>manage.label</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Uuid...</attribute>"
		"			<attribute name='action'>manage.uuid</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Flags...</attribute>"
		"			<attribute name='action'>manage.flags</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Parameters...</attribute>"
		"			<attribute name='action'>manage.parameters</attribute>"
		"		</item>"
		"	</section>"
		"</submenu>"
		"<submenu>"
		"	<attribute name='label' translatable='yes'>File_system</attribute>"
		"	<section>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Check</attribute>"
		"			<attribute name='action'>filesystem.check</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Defragment</attribute>"
		"			<attribute name='action'>filesystem.defragment</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Rebalance</attribute>"
		"			<attribute name='action'>filesystem.rebalance</attribute>"
		"		</item>"
		"	</section>"
		"	<section>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Resize/Move...</attribute>"
		"			<attribute name='action'>filesystem.resize_move</attribute>"
		"		</item>"
		"	</section>"
		"	<section>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Mount...</attribute>"
		"			<attribute name='action'>filesystem.mount</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Umount</attribute>"
		"			<attribute name='action'>filesystem.umount</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Swap on</attribute>"
		"			<attribute name='action'>filesystem.swap_on</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Swap off</attribute>"
		"			<attribute name='action'>filesystem.swap_off</attribute>"
		"		</item>"
		"	</section>"
		"	<section>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Usage</attribute>"
		"			<attribute name='action'>filesystem.usage</attribute>"
		"		</item>"
		"	</section>"
		"</submenu>"
		"<submenu>"
		"	<attribute name='label' translatable='yes'>_Group</attribute>"
		"	<section>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Resize...</attribute>"
		"			<attribute name='action'>group.resize</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Split...</attribute>"
		"			<attribute name='action'>group.split</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Merge...</attribute>"
		"			<attribute name='action'>group.merge</attribute>"
		"		</item>"
		"	</section>"
		"	<section>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Add Stripe...</attribute>"
		"			<attribute name='action'>group.add_stripe</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Remove Stripe...</attribute>"
		"			<attribute name='action'>group.remove_stripe</attribute>"
		"		</item>"
		"	</section>"
		"	<section>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Add Mirror...</attribute>"
		"			<attribute name='action'>group.add_mirror</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Remove Mirror...</attribute>"
		"			<attribute name='action'>group.remove_mirror</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Break Mirror...</attribute>"
		"			<attribute name='action'>group.break_mirror</attribute>"
		"		</item>"
		"	</section>"
		"	<section>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Add RAID...</attribute>"
		"			<attribute name='action'>group.add_raid</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Remove RAID...</attribute>"
		"			<attribute name='action'>group.remove_raid</attribute>"
		"		</item>"
		"	</section>"
		"</submenu>"
		"<submenu>"
		"	<attribute name='label' translatable='yes'>_Plugin</attribute>"
		"	<section>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Plugin X</attribute>"
		"			<attribute name='action'>plugin.x</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Plugin Y</attribute>"
		"			<attribute name='action'>plugin.y</attribute>"
		"		</item>"
		"	</section>"
		"	<section>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Configure plugins...</attribute>"
		"			<attribute name='action'>plugin.configure</attribute>"
		"		</item>"
		"	</section>"
		"</submenu>"
		"<submenu>"
		"	<attribute name='label' translatable='yes'>_Help</attribute>"
		"	<section>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Contents</attribute>"
		"			<attribute name='action'>help.contents</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>What's this?</attribute>"
		"			<attribute name='action'>help.whats_this</attribute>"
		"		</item>"
		"	</section>"
		"	<section>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Report an issue</attribute>"
		"			<attribute name='action'>help.report_issue</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>FAQ</attribute>"
		"			<attribute name='action'>help.faq</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Community</attribute>"
		"			<attribute name='action'>help.community</attribute>"
		"		</item>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>Homepage</attribute>"
		"			<attribute name='action'>help.homepage</attribute>"
		"		</item>"
		"	</section>"
		"	<section>"
		"		<item>"
		"			<attribute name='label' translatable='yes'>About</attribute>"
		"			<attribute name='action'>help.about</attribute>"
		"		</item>"
		"	</section>"
		"</submenu>"
		"</menu></interface>";

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
 * on_keypress
 */
void
Window::on_keypress (int modifier, int key)
{
	//std::cout << "Keypress: " << modifier << " : " << (char) key << std::endl;

	if ((modifier == Gdk::CONTROL_MASK) && (key == 'Q'))
		hide();
}


/**
 * on_action_edit_cut
 */
void
Window::on_action_edit_cut (void)
{
	LOG_TRACE;
}

/**
 * on_action_edit_copy
 */
void
Window::on_action_edit_copy (void)
{
	LOG_TRACE;
}

/**
 * on_action_edit_paste
 */
void
Window::on_action_edit_paste (void)
{
	LOG_TRACE;
}

/**
 * on_action_edit_paste_special
 */
void
Window::on_action_edit_paste_special (void)
{
	LOG_TRACE;
}

/**
 * on_action_edit_undo
 */
void
Window::on_action_edit_undo (void)
{
	LOG_TRACE;
}

/**
 * on_action_edit_redo
 */
void
Window::on_action_edit_redo (void)
{
	LOG_TRACE;
}

/**
 * on_action_edit_clear_all_ops
 */
void
Window::on_action_edit_clear_all_ops (void)
{
	LOG_TRACE;
}

/**
 * on_action_edit_apply_all_ops
 */
void
Window::on_action_edit_apply_all_ops (void)
{
	LOG_TRACE;
}

/**
 * on_action_edit_find
 */
void
Window::on_action_edit_find (void)
{
	LOG_TRACE;
}

/**
 * on_action_edit_find_next
 */
void
Window::on_action_edit_find_next (void)
{
	LOG_TRACE;
}

/**
 * on_action_edit_find_previous
 */
void
Window::on_action_edit_find_previous (void)
{
	LOG_TRACE;
}

/**
 * on_action_edit_preferences
 */
void
Window::on_action_edit_preferences (void)
{
	LOG_TRACE;
}

/**
 * on_action_view_toolbar
 */
void
Window::on_action_view_toolbar (void)
{
	LOG_TRACE;
}

/**
 * on_action_view_graphics
 */
void
Window::on_action_view_graphics (void)
{
	LOG_TRACE;
}

/**
 * on_action_view_tree_view
 */
void
Window::on_action_view_tree_view (void)
{
	LOG_TRACE;
}

/**
 * on_action_view_status_bar
 */
void
Window::on_action_view_status_bar (void)
{
	LOG_TRACE;
}

/**
 * on_action_view_log
 */
void
Window::on_action_view_log (void)
{
	LOG_TRACE;
}

/**
 * on_action_view_pending_actions
 */
void
Window::on_action_view_pending_actions (void)
{
	LOG_TRACE;
}

/**
 * on_action_view_fullscreen
 */
void
Window::on_action_view_fullscreen (void)
{
	LOG_TRACE;
}

/**
 * on_action_view_refresh
 */
void
Window::on_action_view_refresh (void)
{
	LOG_TRACE;
}

/**
 * on_action_view_reload
 */
void
Window::on_action_view_reload (void)
{
	LOG_TRACE;
}

/**
 * on_action_view_theme
 */
void
Window::on_action_view_theme (void)
{
	LOG_TRACE;
}

/**
 * on_action_create_filesystem
 */
void
Window::on_action_create_filesystem (void)
{
	LOG_TRACE;
}

/**
 * on_action_create_partition
 */
void
Window::on_action_create_partition (void)
{
	LOG_TRACE;
}

/**
 * on_action_create_table
 */
void
Window::on_action_create_table (void)
{
	LOG_TRACE;
}

/**
 * on_action_create_encryption
 */
void
Window::on_action_create_encryption (void)
{
	LOG_TRACE;
}

/**
 * on_action_create_lvm_volume
 */
void
Window::on_action_create_lvm_volume (void)
{
	LOG_TRACE;
}

/**
 * on_action_create_subvolume
 */
void
Window::on_action_create_subvolume (void)
{
	LOG_TRACE;
}

/**
 * on_action_create_snapshot
 */
void
Window::on_action_create_snapshot (void)
{
	LOG_TRACE;
}

/**
 * on_action_delete_filesystem
 */
void
Window::on_action_delete_filesystem (void)
{
	LOG_TRACE;
}

/**
 * on_action_delete_partition
 */
void
Window::on_action_delete_partition (void)
{
	LOG_TRACE;
}

/**
 * on_action_delete_table
 */
void
Window::on_action_delete_table (void)
{
	LOG_TRACE;
}

/**
 * on_action_delete_encryption
 */
void
Window::on_action_delete_encryption (void)
{
	LOG_TRACE;
}

/**
 * on_action_delete_lvm_volume
 */
void
Window::on_action_delete_lvm_volume (void)
{
	LOG_TRACE;
}

/**
 * on_action_delete_subvolume
 */
void
Window::on_action_delete_subvolume (void)
{
	LOG_TRACE;
}

/**
 * on_action_delete_snapshot
 */
void
Window::on_action_delete_snapshot (void)
{
	LOG_TRACE;
}

/**
 * on_action_format_wipe
 */
void
Window::on_action_format_wipe (void)
{
	LOG_TRACE;
}

/**
 * on_action_format_filesystem
 */
void
Window::on_action_format_filesystem (void)
{
	LOG_TRACE;
}

/**
 * on_action_format_btrfs
 */
void
Window::on_action_format_btrfs (void)
{
	LOG_TRACE;
}

/**
 * on_action_format_partition_type
 */
void
Window::on_action_format_partition_type (void)
{
	LOG_TRACE;
}

/**
 * on_action_format_table
 */
void
Window::on_action_format_table (void)
{
	LOG_TRACE;
}

/**
 * on_action_manage_properties
 */
void
Window::on_action_manage_properties (void)
{
	LOG_TRACE;
}

/**
 * on_action_manage_label
 */
void
Window::on_action_manage_label (void)
{
	LOG_TRACE;
}

/**
 * on_action_manage_uuid
 */
void
Window::on_action_manage_uuid (void)
{
	LOG_TRACE;
}

/**
 * on_action_manage_flags
 */
void
Window::on_action_manage_flags (void)
{
	LOG_TRACE;
}

/**
 * on_action_manage_parameters
 */
void
Window::on_action_manage_parameters (void)
{
	LOG_TRACE;
}

/**
 * on_action_filesystem_check
 */
void
Window::on_action_filesystem_check (void)
{
	LOG_TRACE;
}

/**
 * on_action_filesystem_defragment
 */
void
Window::on_action_filesystem_defragment (void)
{
	LOG_TRACE;
}

/**
 * on_action_filesystem_rebalance
 */
void
Window::on_action_filesystem_rebalance (void)
{
	LOG_TRACE;
}

/**
 * on_action_filesystem_resize_move
 */
void
Window::on_action_filesystem_resize_move (void)
{
	LOG_TRACE;
}

/**
 * on_action_filesystem_mount
 */
void
Window::on_action_filesystem_mount (void)
{
	LOG_TRACE;
}

/**
 * on_action_filesystem_umount
 */
void
Window::on_action_filesystem_umount (void)
{
	LOG_TRACE;
}

/**
 * on_action_filesystem_swap_on
 */
void
Window::on_action_filesystem_swap_on (void)
{
	LOG_TRACE;
}

/**
 * on_action_filesystem_swap_off
 */
void
Window::on_action_filesystem_swap_off (void)
{
	LOG_TRACE;
}

/**
 * on_action_filesystem_usage
 */
void
Window::on_action_filesystem_usage (void)
{
	LOG_TRACE;
}

/**
 * on_action_group_resize
 */
void
Window::on_action_group_resize (void)
{
	LOG_TRACE;
}

/**
 * on_action_group_split
 */
void
Window::on_action_group_split (void)
{
	LOG_TRACE;
}

/**
 * on_action_group_merge
 */
void
Window::on_action_group_merge (void)
{
	LOG_TRACE;
}

/**
 * on_action_group_add_stripe
 */
void
Window::on_action_group_add_stripe (void)
{
	LOG_TRACE;
}

/**
 * on_action_group_remove_stripe
 */
void
Window::on_action_group_remove_stripe (void)
{
	LOG_TRACE;
}

/**
 * on_action_group_add_mirror
 */
void
Window::on_action_group_add_mirror (void)
{
	LOG_TRACE;
}

/**
 * on_action_group_remove_mirror
 */
void
Window::on_action_group_remove_mirror (void)
{
	LOG_TRACE;
}

/**
 * on_action_group_break_mirror
 */
void
Window::on_action_group_break_mirror (void)
{
	LOG_TRACE;
}

/**
 * on_action_group_add_raid
 */
void
Window::on_action_group_add_raid (void)
{
	LOG_TRACE;
}

/**
 * on_action_group_remove_raid
 */
void
Window::on_action_group_remove_raid (void)
{
	LOG_TRACE;
}

