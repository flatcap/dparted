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
#include <giomm/simpleactiongroup.h>
#include <glibmm.h>
#include <giomm/menu.h>

#include <algorithm>

#include "window.h"
#include "action.h"
#include "drawing_area.h"
#include "gui_app.h"
#include "log.h"
#include "log_trace.h"

Window::Window (void)
{
	set_title ("DParted");

	//XXX Arbitrary minimum requirement -- theme?
	//set_size_request (800, 600);

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

	bool tb = false;
	bool gx = true;
	bool tv = true;
	bool sb = false;
	ConfigFilePtr cfg = gui_app->get_config();
	if (cfg) {
		try { tb = cfg->get_bool ("display.tool_bar");   } catch (const char *msg) { log_debug ("notb\n"); }
		try { gx = cfg->get_bool ("display.graphics");   } catch (const char *msg) { log_debug ("nogx\n"); }
		try { tv = cfg->get_bool ("display.tree_view");  } catch (const char *msg) { log_debug ("notv\n"); }
		try { sb = cfg->get_bool ("display.status_bar"); } catch (const char *msg) { log_debug ("nosb\n"); }
	}

	show_all_children();

	//log_debug ("%d,%d,%d,%d\n", tb, gx, tv, sb);
	toolbar->set_visible (tb);
	drawingarea.set_visible (gx);
	treeview.set_visible (tv);
	statusbar.set_visible (sb);

#if 0
	Glib::RefPtr<Gtk::Settings> s = get_settings();
	s->property_gtk_tooltip_timeout() = 3000;
#endif
}

Window::~Window()
{
}


void
Window::my_realize (void)
{
	//LOG_TRACE;
	drawingarea.grab_focus();
}

void
Window::my_show (void)
{
	//LOG_TRACE;
	//XXX Arbitrary minimum requirement - theme?
	//set_size_request (800, 600);
	resize (800, 250);
}

bool
Window::my_idle (void)
{
#if 1
	std::vector<std::string> files;
	ContainerPtr c = gui_app->scan (files);
	//log_debug ("%d\n", c->get_children().size());
	GfxContainerPtr dummy;
	m_g = GfxContainer::create (dummy, c);
	set_data (m_g);
#endif

	return false;
}


bool
Window::on_delete_event (GdkEventAny* UNUSED(event))
{
	set_show_menubar(false);
	return false;
}


bool
Window::on_mouse_click (GdkEventButton* UNUSED(event))
{
	//log_debug ("Window: mouse click: ("); << event->x << "," << event->y << ")\n";
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

	//log_debug ("Focus: "); << cont << "\n";
	return true;
}

GfxContainerPtr
Window::get_focus (void)
{
	return focus;
}

void
Window::set_data (GfxContainerPtr c)
{
	treeview.init_treeview(c);
	drawingarea.set_data(c);
}


void
Window::load_config (const std::string& UNUSED(filename))
{
	LOG_TRACE;
}

void
Window::load_theme (const std::string& UNUSED(filename))
{
	LOG_TRACE;
}

void
Window::load_disk (const std::string& UNUSED(filename))
{
	LOG_TRACE;
}

void
Window::set_geometry (int x, int y, int w, int h)
{
	//LOG_TRACE;

	x = std::max (x, 0);
	y = std::max (y, 0);

	w = std::max (w, 100);
	h = std::max (h, 100);

	move (x, y);
	resize (w, h);

	//log_debug ("pg = %d\n", parse_geometry ("800x400+200+50"));
	//log_debug ("pg = %d\n", parse_geometry ("800x400-0-0"));

	//XXX do something smart with negative x,y?
#if 0
	int width = 0;
	int height = 0;
	get_size (width, height);
	move (1920+1366-width, 768-height);
#endif
}


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

	log_debug ("%s\n", message.c_str());
}

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

	log_debug ("%s\n", message.c_str());
}

void
Window::on_menu_file_new_generic (void)
{
	log_debug ("A File|New menu item was selected.\n");
}

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
		QuestionPtr q = Question::create (c, [] (QuestionPtr UNUSED(q)) { /*log_debug ("reply\n");;*/ } );
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

void
Window::on_menu_others (void)
{
	log_debug ("A menu item was selected.\n");
}

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

	log_debug ("%s\n", message.c_str());
}

void
Window::on_menu_view (int option)
{
	log_debug ("on_menu_view: "); << option << "\n";

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
		//log_debug ("Keypress: "); << k.first << " : " << k.second << "\n";
		Gtk::MenuItem* i = manage (new Gtk::MenuItem());
		i->signal_activate().connect (sigc::bind<int,int> (sigc::mem_fun (*this, &Window::on_keypress), k.first, k.second));
		i->add_accelerator ("activate", accel, k.second, (Gdk::ModifierType) k.first, Gtk::ACCEL_VISIBLE);
		i->show();
		m_fake_menu.append (*i);
	}

	add_accel_group (accel);

}

void
Window::insert_general_actions (std::string section, const std::vector<const char*>& commands)
{
	for (auto c : commands) {
		std::string name = section + "." + c;
		Glib::RefPtr<Gio::Action> a = add_action(name, sigc::bind<std::string,std::string> (sigc::mem_fun (*this, &Window::on_action_general), section, c));
		Glib::RefPtr<Gio::SimpleAction> s = Glib::RefPtr<Gio::SimpleAction>::cast_dynamic (a);
		action_map[name] = s;
	}
}

void
Window::init_actions (void)
{
	auto group = Gio::SimpleActionGroup::create();
	group->add_action("open",  sigc::mem_fun(*gui_app, &GuiApp::on_action_file_open));
	group->add_action("close", sigc::mem_fun(*gui_app, &GuiApp::on_action_file_close));
	group->add_action("quit",  sigc::mem_fun(*gui_app, &GuiApp::on_action_file_quit));
	insert_action_group ("file", group);

	group = Gio::SimpleActionGroup::create();
	group->add_action("x",          sigc::mem_fun(*gui_app, &GuiApp::on_action_plugin));
	group->add_action("y",          sigc::mem_fun(*gui_app, &GuiApp::on_action_plugin));
	group->add_action("configure",  sigc::mem_fun(*gui_app, &GuiApp::on_action_plugin));
	insert_action_group ("plugin", group);

	group = Gio::SimpleActionGroup::create();
	group->add_action("contents",     sigc::mem_fun(*gui_app, &GuiApp::on_action_help));
	group->add_action("whats_this",   sigc::mem_fun(*gui_app, &GuiApp::on_action_help));
	group->add_action("report_issue", sigc::mem_fun(*gui_app, &GuiApp::on_action_help));
	group->add_action("faq",          sigc::mem_fun(*gui_app, &GuiApp::on_action_help));
	group->add_action("community",    sigc::mem_fun(*gui_app, &GuiApp::on_action_help));
	group->add_action("homepage",     sigc::mem_fun(*gui_app, &GuiApp::on_action_help));
	group->add_action("about",        sigc::mem_fun(*gui_app, &GuiApp::on_action_help));
	insert_action_group ("help", group);

	insert_general_actions ("edit",       { "cut", "copy", "paste", "paste_special", "undo", "redo", "clear_all_ops", "apply_all_ops", "find", "find_next", "find_previous", "preferences" });
	insert_general_actions ("view",       { "toolbar", "graphics", "tree_view", "status_bar", "log", "pending_actions", "fullscreen", "refresh", "reload", "theme" });
	insert_general_actions ("create",     { "filesystem", "partition", "table", "encryption", "lvm_volume", "subvolume", "snapshot" });
	insert_general_actions ("delete",     { "filesystem", "partition", "table", "encryption", "lvm_volume", "subvolume", "snapshot" });
	insert_general_actions ("format",     { "wipe", "filesystem", "btrfs", "partition_type", "table" });
	insert_general_actions ("manage",     { "properties", "label", "uuid", "flags", "parameters" });
	insert_general_actions ("filesystem", { "check", "defragment", "rebalance", "resize_move", "mount", "umount", "swap_on", "swap_off", "usage" });
	insert_general_actions ("group",      { "resize", "split", "merge", "add_stripe", "remove_stripe", "add_mirror", "remove_mirror", "break_mirror", "add_raid", "remove_raid" });

	insert_general_actions ("dummy",      { "alignment", "block", "btrfs", "container", "disk", "ext2", "ext3", "ext4", "extended", "extfs", "file", "filesystem", "gpt", "gpt_partition", "loop", "luks_partition", "luks_table", "lvm_group", "lvm_linear", "lvm_metadata", "lvm_mirror", "lvm_partition", "lvm_raid", "lvm_stripe", "lvm_table", "lvm_volume", "md_linear", "md_mirror", "md_partition", "md_raid", "md_stripe", "md_table", "md_volume", "misc", "msdos", "msdos_partition", "ntfs", "partition", "random", "reiserfs", "reserved", "snapshot", "space", "subvolume", "swap", "table", "unallocated", "vfat", "volume", "whole", "xfs", "zero" });

#if 0
	Glib::RefPtr<Gio::Action> a;
	a = lookup_action ("edit.cut");
	a->reference();		//XXX seems to be a bug in Gtk+/Gtkmm
	Glib::RefPtr<Gio::SimpleAction> s = Glib::RefPtr<Gio::SimpleAction>::cast_dynamic (a);
	s->set_enabled (false);
#endif
#if 0
	for (auto a : action_map) {
		if (a.first[7] < 'n')
			a.second->set_enabled (false);
		//std::cout << a.first << "\n";
	}
#endif
}

void
Window::init_menubar (Gtk::Box& box)
{
	m_refBuilder = Gtk::Builder::create();

	//Layout the actions in a menubar and toolbar:
	Glib::ustring ui_info =
		"<interface><menu id='dparted-menu'>"
		"<submenu>"
		"	<attribute name='label' translatable='yes'>_File</attribute>"
		"	<section>"
		"		<item> <attribute name='label' translatable='yes'>Open...</attribute>              <attribute name='action'>file.open</attribute>              </item>"
		"	</section><section>"
		"		<item> <attribute name='label' translatable='yes'>Close</attribute>                <attribute name='action'>file.close</attribute>             </item>"
		"		<item> <attribute name='label' translatable='yes'>Quit</attribute>                 <attribute name='action'>file.quit</attribute>              </item>"
		"	</section>"
		"</submenu><submenu>"
		"	<attribute name='label' translatable='yes'>_Edit</attribute>"
		"	<section>"
		"		<item> <attribute name='label' translatable='yes'>Cut</attribute>                  <attribute name='action'>win.edit.cut</attribute>               </item>"
		"		<item> <attribute name='label' translatable='yes'>Copy</attribute>                 <attribute name='action'>win.edit.copy</attribute>              </item>"
		"		<item> <attribute name='label' translatable='yes'>Paste</attribute>                <attribute name='action'>win.edit.paste</attribute>             </item>"
		"		<item> <attribute name='label' translatable='yes'>Paste Special...</attribute>     <attribute name='action'>win.edit.paste_special</attribute>     </item>"
		"	</section><section>"
		"		<item> <attribute name='label' translatable='yes'>Undo</attribute>                 <attribute name='action'>win.edit.undo</attribute>              </item>"
		"		<item> <attribute name='label' translatable='yes'>Redo</attribute>                 <attribute name='action'>win.edit.redo</attribute>              </item>"
		"		<item> <attribute name='label' translatable='yes'>Clear all ops</attribute>        <attribute name='action'>win.edit.clear_all_ops</attribute>     </item>"
		"		<item> <attribute name='label' translatable='yes'>Apply all ops...</attribute>     <attribute name='action'>win.edit.apply_all_ops</attribute>     </item>"
		"	</section><section>"
		"		<item> <attribute name='label' translatable='yes'>Find...</attribute>              <attribute name='action'>win.edit.find</attribute>              </item>"
		"		<item> <attribute name='label' translatable='yes'>Find next</attribute>            <attribute name='action'>win.edit.find_next</attribute>         </item>"
		"		<item> <attribute name='label' translatable='yes'>Find previous</attribute>        <attribute name='action'>win.edit.find_previous</attribute>     </item>"
		"	</section><section>"
		"		<item> <attribute name='label' translatable='yes'>Preferences...</attribute>       <attribute name='action'>win.edit.preferences</attribute>       </item>"
		"	</section>"
		"</submenu><submenu>"
		"	<attribute name='label' translatable='yes'>_View</attribute>"
		"	<section>"
		"		<item> <attribute name='label' translatable='yes'>Toolbar</attribute>              <attribute name='action'>win.view.toolbar</attribute>           </item>"
		"		<item> <attribute name='label' translatable='yes'>Graphics</attribute>             <attribute name='action'>win.view.graphics</attribute>          </item>"
		"		<item> <attribute name='label' translatable='yes'>Tree view</attribute>            <attribute name='action'>win.view.tree_view</attribute>         </item>"
		"		<item> <attribute name='label' translatable='yes'>Status bar</attribute>           <attribute name='action'>win.view.status_bar</attribute>        </item>"
		"	</section><section>"
		"		<item> <attribute name='label' translatable='yes'>Log</attribute>                  <attribute name='action'>win.view.log</attribute>               </item>"
		"		<item> <attribute name='label' translatable='yes'>Pending Actions</attribute>      <attribute name='action'>win.view.pending_actions</attribute>   </item>"
		"	</section><section>"
		"		<item> <attribute name='label' translatable='yes'>Fullscreen</attribute>           <attribute name='action'>win.view.fullscreen</attribute>        </item>"
		"	</section><section>"
		"		<item> <attribute name='label' translatable='yes'>Refresh</attribute>              <attribute name='action'>win.view.refresh</attribute>           </item>"
		"		<item> <attribute name='label' translatable='yes'>Reload</attribute>               <attribute name='action'>win.view.reload</attribute>            </item>"
		"	</section><section>"
		"		<item> <attribute name='label' translatable='yes'>Theme</attribute>                <attribute name='action'>win.view.theme</attribute>             </item>"
		"	</section>"
		"</submenu><submenu>"
		"	<attribute name='label' translatable='yes'>_Create</attribute>"
		"	<section>"
		"		<item> <attribute name='label' translatable='yes'>Filesystem...</attribute>        <attribute name='action'>win.create.filesystem</attribute>      </item>"
		"		<item> <attribute name='label' translatable='yes'>Partition...</attribute>         <attribute name='action'>win.create.partition</attribute>       </item>"
		"		<item> <attribute name='label' translatable='yes'>Table...</attribute>             <attribute name='action'>win.create.table</attribute>           </item>"
		"	</section><section>"
		"		<item> <attribute name='label' translatable='yes'>Encryption...</attribute>        <attribute name='action'>win.create.encryption</attribute>      </item>"
		"		<item> <attribute name='label' translatable='yes'>Lvm Volume...</attribute>        <attribute name='action'>win.create.lvm_volume</attribute>      </item>"
		"	</section><section>"
		"		<item> <attribute name='label' translatable='yes'>Subvolume...</attribute>         <attribute name='action'>win.create.subvolume</attribute>       </item>"
		"		<item> <attribute name='label' translatable='yes'>Snapshot...</attribute>          <attribute name='action'>win.create.snapshot</attribute>        </item>"
		"	</section>"
		"</submenu><submenu>"
		"	<attribute name='label' translatable='yes'>_Delete</attribute>"
		"	<section>"
		"		<item> <attribute name='label' translatable='yes'>Filesystem</attribute>           <attribute name='action'>win.delete.filesystem</attribute>      </item>"
		"		<item> <attribute name='label' translatable='yes'>Partition</attribute>            <attribute name='action'>win.delete.partition</attribute>       </item>"
		"		<item> <attribute name='label' translatable='yes'>Table</attribute>                <attribute name='action'>win.delete.table</attribute>           </item>"
		"	</section><section>"
		"		<item> <attribute name='label' translatable='yes'>Encryption</attribute>           <attribute name='action'>win.delete.encryption</attribute>      </item>"
		"		<item> <attribute name='label' translatable='yes'>Lvm Volume</attribute>           <attribute name='action'>win.delete.lvm_volume</attribute>      </item>"
		"	</section><section>"
		"		<item> <attribute name='label' translatable='yes'>Subvolume</attribute>            <attribute name='action'>win.delete.subvolume</attribute>       </item>"
		"		<item> <attribute name='label' translatable='yes'>Snapshot</attribute>             <attribute name='action'>win.delete.snapshot</attribute>        </item>"
		"	</section>"
		"</submenu><submenu>"
		"	<attribute name='label' translatable='yes'>F_ormat</attribute>"
		"	<section>"
		"		<item> <attribute name='label' translatable='yes'>Wipe...</attribute>              <attribute name='action'>win.format.wipe</attribute>            </item>"
		"		<item> <attribute name='label' translatable='yes'>Filesystem...</attribute>        <attribute name='action'>win.format.filesystem</attribute>      </item>"
		"	</section><section>"
		"		<item> <attribute name='label' translatable='yes'>Btrfs...</attribute>             <attribute name='action'>win.format.btrfs</attribute>           </item>"
		"		<item> <attribute name='label' translatable='yes'>Partition Type...</attribute>    <attribute name='action'>win.format.partition_type</attribute>  </item>"
		"		<item> <attribute name='label' translatable='yes'>Table...</attribute>             <attribute name='action'>win.format.table</attribute>           </item>"
		"	</section>"
		"</submenu><submenu>"
		"	<attribute name='label' translatable='yes'>_Manage</attribute>"
		"	<section>"
		"		<item> <attribute name='label' translatable='yes'>Properties</attribute>           <attribute name='action'>win.manage.properties</attribute>      </item>"
		"	</section><section>"
		"		<item> <attribute name='label' translatable='yes'>Label...</attribute>             <attribute name='action'>win.manage.label</attribute>           </item>"
		"		<item> <attribute name='label' translatable='yes'>Uuid...</attribute>              <attribute name='action'>win.manage.uuid</attribute>            </item>"
		"		<item> <attribute name='label' translatable='yes'>Flags...</attribute>             <attribute name='action'>win.manage.flags</attribute>           </item>"
		"		<item> <attribute name='label' translatable='yes'>Parameters...</attribute>        <attribute name='action'>win.manage.parameters</attribute>      </item>"
		"	</section>"
		"</submenu><submenu>"
		"	<attribute name='label' translatable='yes'>File_system</attribute>"
		"	<section>"
		"		<item> <attribute name='label' translatable='yes'>Check</attribute>                <attribute name='action'>win.filesystem.check</attribute>       </item>"
		"		<item> <attribute name='label' translatable='yes'>Defragment</attribute>           <attribute name='action'>win.filesystem.defragment</attribute>  </item>"
		"		<item> <attribute name='label' translatable='yes'>Rebalance</attribute>            <attribute name='action'>win.filesystem.rebalance</attribute>   </item>"
		"	</section><section>"
		"		<item> <attribute name='label' translatable='yes'>Resize/Move...</attribute>       <attribute name='action'>win.filesystem.resize_move</attribute> </item>"
		"	</section><section>"
		"		<item> <attribute name='label' translatable='yes'>Mount...</attribute>             <attribute name='action'>win.filesystem.mount</attribute>       </item>"
		"		<item> <attribute name='label' translatable='yes'>Umount</attribute>               <attribute name='action'>win.filesystem.umount</attribute>      </item>"
		"		<item> <attribute name='label' translatable='yes'>Swap on</attribute>              <attribute name='action'>win.filesystem.swap_on</attribute>     </item>"
		"		<item> <attribute name='label' translatable='yes'>Swap off</attribute>             <attribute name='action'>win.filesystem.swap_off</attribute>    </item>"
		"	</section><section>"
		"		<item> <attribute name='label' translatable='yes'>Usage</attribute>                <attribute name='action'>win.filesystem.usage</attribute>       </item>"
		"	</section>"
		"</submenu><submenu>"
		"	<attribute name='label' translatable='yes'>_Group</attribute>"
		"	<section>"
		"		<item> <attribute name='label' translatable='yes'>Resize...</attribute>            <attribute name='action'>win.group.resize</attribute>           </item>"
		"		<item> <attribute name='label' translatable='yes'>Split...</attribute>             <attribute name='action'>win.group.split</attribute>            </item>"
		"		<item> <attribute name='label' translatable='yes'>Merge...</attribute>             <attribute name='action'>win.group.merge</attribute>            </item>"
		"	</section><section>"
		"		<item> <attribute name='label' translatable='yes'>Add Stripe...</attribute>        <attribute name='action'>win.group.add_stripe</attribute>       </item>"
		"		<item> <attribute name='label' translatable='yes'>Remove Stripe...</attribute>     <attribute name='action'>win.group.remove_stripe</attribute>    </item>"
		"	</section><section>"
		"		<item> <attribute name='label' translatable='yes'>Add Mirror...</attribute>        <attribute name='action'>win.group.add_mirror</attribute>       </item>"
		"		<item> <attribute name='label' translatable='yes'>Remove Mirror...</attribute>     <attribute name='action'>win.group.remove_mirror</attribute>    </item>"
		"		<item> <attribute name='label' translatable='yes'>Break Mirror...</attribute>      <attribute name='action'>win.group.break_mirror</attribute>     </item>"
		"	</section><section>"
		"		<item> <attribute name='label' translatable='yes'>Add RAID...</attribute>          <attribute name='action'>win.group.add_raid</attribute>         </item>"
		"		<item> <attribute name='label' translatable='yes'>Remove RAID...</attribute>       <attribute name='action'>win.group.remove_raid</attribute>      </item>"
		"	</section>"
		"</submenu><submenu>"
		"	<attribute name='label' translatable='yes'>D_ummy</attribute>"
		"	<section>"
		"		<item> <attribute name='label' translatable='yes'>block</attribute>        <attribute name='action'>win.dummy.block</attribute>        </item>"
		"		<item> <attribute name='label' translatable='yes'>btrfs</attribute>        <attribute name='action'>win.dummy.btrfs</attribute>        </item>"
		"		<item> <attribute name='label' translatable='yes'>container</attribute>    <attribute name='action'>win.dummy.container</attribute>    </item>"
		"		<item> <attribute name='label' translatable='yes'>disk</attribute>         <attribute name='action'>win.dummy.disk</attribute>         </item>"
		"		<item> <attribute name='label' translatable='yes'>extended</attribute>     <attribute name='action'>win.dummy.extended</attribute>     </item>"
		"		<item> <attribute name='label' translatable='yes'>extfs</attribute>        <attribute name='action'>win.dummy.extfs</attribute>        </item>"
		"		<item> <attribute name='label' translatable='yes'>file</attribute>         <attribute name='action'>win.dummy.file</attribute>         </item>"
		"		<item> <attribute name='label' translatable='yes'>filesystem</attribute>   <attribute name='action'>win.dummy.filesystem</attribute>   </item>"
		"		<item> <attribute name='label' translatable='yes'>gpt</attribute>          <attribute name='action'>win.dummy.gpt</attribute>          </item>"
		"		<item> <attribute name='label' translatable='yes'>loop</attribute>         <attribute name='action'>win.dummy.loop</attribute>         </item>"
		"		<item> <attribute name='label' translatable='yes'>luks</attribute>         <attribute name='action'>win.dummy.luks</attribute>         </item>"
		"		<item> <attribute name='label' translatable='yes'>lvmgroup</attribute>     <attribute name='action'>win.dummy.lvmgroup</attribute>     </item>"
		"		<item> <attribute name='label' translatable='yes'>lvmlinear</attribute>    <attribute name='action'>win.dummy.lvmlinear</attribute>    </item>"
		"		<item> <attribute name='label' translatable='yes'>lvmmetadata</attribute>  <attribute name='action'>win.dummy.lvmmetadata</attribute>  </item>"
		"		<item> <attribute name='label' translatable='yes'>lvmmirror</attribute>    <attribute name='action'>win.dummy.lvmmirror</attribute>    </item>"
		"		<item> <attribute name='label' translatable='yes'>lvmpartition</attribute> <attribute name='action'>win.dummy.lvmpartition</attribute> </item>"
		"		<item> <attribute name='label' translatable='yes'>lvmraid</attribute>      <attribute name='action'>win.dummy.lvmraid</attribute>      </item>"
		"		<item> <attribute name='label' translatable='yes'>lvmstripe</attribute>    <attribute name='action'>win.dummy.lvmstripe</attribute>    </item>"
		"		<item> <attribute name='label' translatable='yes'>lvmtable</attribute>     <attribute name='action'>win.dummy.lvmtable</attribute>     </item>"
		"		<item> <attribute name='label' translatable='yes'>lvmvolume</attribute>    <attribute name='action'>win.dummy.lvmvolume</attribute>    </item>"
		"		<item> <attribute name='label' translatable='yes'>mdgroup</attribute>      <attribute name='action'>win.dummy.mdgroup</attribute>      </item>"
		"		<item> <attribute name='label' translatable='yes'>mdtable</attribute>      <attribute name='action'>win.dummy.mdtable</attribute>      </item>"
		"		<item> <attribute name='label' translatable='yes'>misc</attribute>         <attribute name='action'>win.dummy.misc</attribute>         </item>"
		"		<item> <attribute name='label' translatable='yes'>msdos</attribute>        <attribute name='action'>win.dummy.msdos</attribute>        </item>"
		"		<item> <attribute name='label' translatable='yes'>ntfs</attribute>         <attribute name='action'>win.dummy.ntfs</attribute>         </item>"
		"		<item> <attribute name='label' translatable='yes'>partition</attribute>    <attribute name='action'>win.dummy.partition</attribute>    </item>"
		"		<item> <attribute name='label' translatable='yes'>table</attribute>        <attribute name='action'>win.dummy.table</attribute>        </item>"
		"		<item> <attribute name='label' translatable='yes'>volume</attribute>       <attribute name='action'>win.dummy.volume</attribute>       </item>"
		"		<item> <attribute name='label' translatable='yes'>whole</attribute>        <attribute name='action'>win.dummy.whole</attribute>        </item>"
		"	</section>"
		"</submenu><submenu>"
		"	<attribute name='label' translatable='yes'>_Plugin</attribute>"
		"	<section>"
		"		<item> <attribute name='label' translatable='yes'>Plugin X</attribute>             <attribute name='action'>plugin.x</attribute>               </item>"
		"		<item> <attribute name='label' translatable='yes'>Plugin Y</attribute>             <attribute name='action'>plugin.y</attribute>               </item>"
		"	</section><section>"
		"		<item> <attribute name='label' translatable='yes'>Configure plugins...</attribute> <attribute name='action'>plugin.configure</attribute>       </item>"
		"	</section>"
		"</submenu><submenu>"
		"	<attribute name='label' translatable='yes'>_Help</attribute>"
		"	<section>"
		"		<item> <attribute name='label' translatable='yes'>Contents</attribute>             <attribute name='action'>help.contents</attribute>          </item>"
		"		<item> <attribute name='label' translatable='yes'>What's this?</attribute>         <attribute name='action'>help.whats_this</attribute>        </item>"
		"	</section><section>"
		"		<item> <attribute name='label' translatable='yes'>Report an issue</attribute>      <attribute name='action'>help.report_issue</attribute>      </item>"
		"		<item> <attribute name='label' translatable='yes'>FAQ</attribute>                  <attribute name='action'>help.faq</attribute>               </item>"
		"		<item> <attribute name='label' translatable='yes'>Community</attribute>            <attribute name='action'>help.community</attribute>         </item>"
		"		<item> <attribute name='label' translatable='yes'>Homepage</attribute>             <attribute name='action'>help.homepage</attribute>          </item>"
		"	</section><section>"
		"		<item> <attribute name='label' translatable='yes'>About</attribute>                <attribute name='action'>help.about</attribute>             </item>"
		"	</section>"
		"</submenu>"
		"</menu></interface>";

	try {
		m_refBuilder->add_from_string (ui_info);
	} catch (const Glib::Error& ex) {
		log_debug ("building menus failed: "); << ex.what();
	}

	Glib::RefPtr<Glib::Object> object = m_refBuilder->get_object ("dparted-menu");
	Glib::RefPtr<Gio::MenuModel> gmenu = Glib::RefPtr<Gio::Menu>::cast_dynamic (object);
	if (!gmenu)
		g_warning ("GMenu not found");

	gui_app->set_menubar (gmenu);

	toolbar = Gtk::manage (new Gtk::Toolbar());
	Gtk::ToolButton* button = Gtk::manage (new Gtk::ToolButton());
	button->set_icon_name ("document-new");
	gtk_actionable_set_detailed_action_name (GTK_ACTIONABLE (button->gobj()), "file.open");
	toolbar->add (*button);

	button = Gtk::manage (new Gtk::ToolButton());
	button->set_icon_name ("application-exit");
	gtk_actionable_set_detailed_action_name (GTK_ACTIONABLE (button->gobj()), "file.quit");
	toolbar->add (*button);

	box.pack_start (*toolbar, Gtk::PACK_SHRINK);
}


void
Window::set_actions (std::vector<Action>& list)
{
	for (auto a : action_map) {			// First, disable all the actions
		a.second->set_enabled (false);
	}

	for (auto a : list) {				// Then selectively enable the ones we want
		auto it = action_map.find (a.name);
		if (it != std::end (action_map)) {
			//log_debug ("Enable: "); << a.name << "\n";
			it->second->set_enabled (true);
		} else {
			log_debug ("Can't find "); << a.name << "\n";
		}
	}
}

void
Window::on_keypress (int modifier, int key)
{
	//log_debug ("Keypress: "); << modifier << " : " << (char) key << "\n";

	if ((modifier == Gdk::CONTROL_MASK) && (key == 'Q')) {
		set_show_menubar(false);
		hide();
	}
}


void
Window::on_action_general (std::string section, std::string name)
{
	log_debug ("%s: %s.%s\n", __FUNCTION__, section.c_str(), name.c_str());

	if (!focus) {
		log_debug ("no focus\n");
		return;
	}

	ContainerPtr c = focus->get_container();
	if (!c) {
		log_debug ("no container\n");
		return;
	}

	Action a = { section + '.' + name, true };
	c->perform_action (a);
}


