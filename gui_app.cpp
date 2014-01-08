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

#include <gtkmm/object.h>
#include <gtkmm/main.h>
#include <gtkmm/icontheme.h>
#include <gtkmm/settings.h>
#include <gtkmm/aboutdialog.h>
#include <gtkmm/stock.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/builder.h>
#include <giomm/menu.h>
#include <giomm/menuitem.h>
#include <giomm/application.h>
#include <glibmm.h>

#include "app.h"
#include "gui_app.h"
#include "dparted.h"
#include "log.h"
#include "log_trace.h"

GuiAppPtr gui_app;

/**
 * GuiApp
 */
GuiApp::GuiApp() :
	Gtk::Application("org.flatcap.test.area", Gio::APPLICATION_HANDLES_OPEN)
{
	//LOG_TRACE;
	Glib::set_application_name("dparted");
}

/**
 * ~GuiApp
 */
GuiApp::~GuiApp()
{
}


/**
 * on_startup
 */
void
GuiApp::on_startup (void)
{
	//LOG_TRACE;
	Gtk::Application::on_startup();

	Glib::RefPtr<Gtk::IconTheme> theme = Gtk::IconTheme::get_default();

	theme->append_search_path ("/home/flatcap/work/gtk-app/icons");

	add_action("preferences", sigc::mem_fun(*this, &GuiApp::menu_preferences));
	add_action("help",        sigc::mem_fun(*this, &GuiApp::menu_help));
	add_action("about",       sigc::mem_fun(*this, &GuiApp::menu_about));
	add_action("quit",        sigc::mem_fun(*this, &GuiApp::menu_quit));

	Glib::RefPtr<Gio::Menu> menu = Gio::Menu::create();

	menu->append ("_Preferences", "app.preferences");
	menu->append ("_Help",        "app.help");
	menu->append ("_About",       "app.about");
	menu->append ("_Quit",        "app.quit");

	set_app_menu (menu);

	Glib::RefPtr<Gio::SimpleAction> action;

	action = add_action("apple",      sigc::mem_fun(*this, &GuiApp::menu_preferences));
	action = add_action("banana",     sigc::mem_fun(*this, &GuiApp::menu_preferences));
	action = add_action("cherry",     sigc::mem_fun(*this, &GuiApp::menu_preferences));
	action = add_action("damson",     sigc::mem_fun(*this, &GuiApp::menu_preferences));
	action = add_action("elderberry", sigc::mem_fun(*this, &GuiApp::menu_preferences));

	menu = Gio::Menu::create();
	Glib::RefPtr<Gio::Menu> sub  = Gio::Menu::create();

	sub->append ("_Apple",      "app.apple");
	sub->append ("_Banana",     "app.banana");
	sub->append ("_Cherry",     "app.cherry");
	sub->append ("_Damson",     "app.damson");
	sub->append ("_Elderberry", "app.elderberry");
	menu->append_submenu ("_Fruit", sub);

	//set_menubar (menu);

	Gio::ActionMap*                  m = dynamic_cast<Gio::ActionMap*>(this);
	Glib::RefPtr<Gio::Action>        a = m->lookup_action("banana");
	Glib::RefPtr<Gio::SimpleAction>  s = Glib::RefPtr<Gio::SimpleAction>::cast_dynamic(a);
	s->set_enabled(false);
}

/**
 * on_activate
 */
void
GuiApp::on_activate()
{
	//LOG_TRACE;
	Gtk::Application::on_activate();

	for (auto win : Gtk::Application::get_windows()) {
		win->present();
	}

	DParted* dp = new DParted();
	dp->set_data (top_level);
	add_window(*dp);	// App now owns DParted
	dp->show();
}


/**
 * on_window_added
 */
void
GuiApp::on_window_added  (Gtk::Window* window)
{
	//LOG_TRACE;
	Gtk::Application::on_window_added (window);
}

/**
 * on_window_removed
 */
void
GuiApp::on_window_removed(Gtk::Window* window)
{
	//LOG_TRACE;
	Gtk::Application::on_window_removed (window);
}


/**
 * on_open
 */
void
GuiApp::on_open (const type_vec_files& files, const Glib::ustring& hint)
{
	//LOG_TRACE;
	Gtk::Application::on_open (files, hint);
}

/**
 * on_command_line
 */
int
GuiApp::on_command_line(const Glib::RefPtr<Gio::ApplicationCommandLine>& command_line)
{
	//LOG_TRACE;
	return Gtk::Application::on_command_line (command_line);
}


/**
 * menu_preferences
 */
void
GuiApp::menu_preferences (void)
{
	//LOG_TRACE;
}

/**
 * menu_help
 */
void
GuiApp::menu_help (void)
{
	//LOG_TRACE;
}

/**
 * menu_about
 */
void
GuiApp::menu_about (void)
{
	const std::vector<Glib::ustring> authors     = {"Jim", "Bob", "Dave", "Mike", "Phil"};
	const std::vector<Glib::ustring> documenters = {"Mary", "Jane", "Gladys"};
	const std::vector<Glib::ustring> artists     = {"Van Gogh", "Monet", "Turner"};

	Gtk::AboutDialog about;

	about.set_name("DParted");
	about.set_program_name("dparted");
	about.set_version("0.1");
	about.set_logo_icon_name("dparted");
	about.set_copyright("Copyright \xc2\xa9 2014 Richard Russon (FlatCap)\n");
	about.set_comments("Reorganise your disk.");
	about.set_website("http://flatcap.org/rich");
	about.set_website_label("Rich's homepage");
	about.set_authors(authors);
	about.set_documenters(documenters);
	about.set_translator_credits("Bjorn ,Volker, Pierre");
	about.set_artists(artists);
	about.set_license_type(Gtk::LICENSE_GPL_3_0);

	//about.set_transient_for(recent_changes);

	about.set_icon_name("dparted");

	about.run();
}

/**
 * menu_quit
 */
void
GuiApp::menu_quit (void)
{
	quit();
}


/**
 * ask
 */
bool
GuiApp::ask (Question& q)
{
	Gtk::MessageDialog dialog (q.question, false, Gtk::MessageType::MESSAGE_QUESTION, Gtk::ButtonsType::BUTTONS_NONE, true);

	dialog.set_title (q.title);

	int id = 1001;
	for (auto a : q.answers) {
		dialog.add_button (a, id++);
	}

	dialog.run();

	return true;
}

/**
 * notify
 */
bool
GuiApp::notify (Message& m)
{
	// Might need to queue these until we're ready to confront the user
	return false;
}


/**
 * get_theme
 */
ThemePtr
GuiApp::get_theme (void)
{
	return theme;
}


/**
 * set_config
 */
bool
GuiApp::set_config (const std::string& filename)
{
	if (!App::set_config (filename))
		return false;

	// Pick out any gui options we want

	return true;
}

/**
 * set_theme
 */
bool
GuiApp::set_theme (const std::string& filename)
{
	ThemePtr tp;

	tp = Theme::read_file (filename);
	if (!tp) {
		//notify the user
		return false;
	}

	if (theme) {
		//if modified ask user if they're sure
	}

	theme = tp;
	//tp->dump_config();

	return true;
}

