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
#include <gtkmm/radiobutton.h>
#include <gtkmm/frame.h>
#include <glibmm.h>

#include "app.h"
#include "gui_app.h"
#include "window.h"
#include "log.h"
#include "log_trace.h"
#include "properties_dialog.h"

GuiAppPtr gui_app;

/**
 * GuiApp
 */
GuiApp::GuiApp() :
	Gtk::Application ("org.flatcap.test.area", Gio::APPLICATION_HANDLES_OPEN /*| Gio::APPLICATION_HANDLES_COMMAND_LINE*/)
{
	//LOG_TRACE;
	Glib::set_application_name ("dparted");
	Glib::signal_idle().connect (sigc::mem_fun (*this, &GuiApp::my_idle));
}

/**
 * ~GuiApp
 */
GuiApp::~GuiApp()
{
}


/**
 * my_idle
 */
bool
GuiApp::my_idle (void)
{
	//XXX check that dialog's object hasn't gone away
	if (false && passwd) {
		passwd->set_title ("Password for X");
		passwd->set_message ("text message");
		passwd->set_secondary_text ("secondary text");
		Gtk::Image i;
		Gtk::Button cancel ("Cancel");
		i.set_from_icon_name ("dialog-password", Gtk::BuiltinIconSize::ICON_SIZE_DIALOG);
		passwd->set_image(i);

		Gtk::Frame frame ("Password:");
		Gtk::RadioButton r1 ("Forget it immediately");
		Gtk::RadioButton r2 ("Remember it until the application closes");
		Gtk::RadioButton r3 ("Save it to disk (encrypted)");
		Gtk::CheckButton c1 ("Remember this setting");

		Gtk::RadioButton::Group group = r1.get_group();
		r2.set_group(group);
		r3.set_group(group);
		r2.set_active (true);
		c1.set_active (true);
		Gtk::Box box (Gtk::Orientation::ORIENTATION_VERTICAL, 0);
		frame.add (box);
		box.pack_start(r1, Gtk::PackOptions::PACK_SHRINK, 0);
		box.pack_start(r2, Gtk::PackOptions::PACK_SHRINK, 0);
		box.pack_start(r3, Gtk::PackOptions::PACK_SHRINK, 0);
		box.pack_start(c1, Gtk::PackOptions::PACK_SHRINK, 0);
		passwd->get_content_area()->pack_start(frame);

		passwd->get_action_area()->pack_start (cancel);
		passwd->set_transient_for (*get_active_window());
		passwd->show_all();
		passwd->run();
		passwd = nullptr;
		return false;	// detach
	}

	//std::cout << "GuiApp is idle" << std::endl;
	return false;	// continue
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

	theme->append_search_path ("/home/flatcap/work/gtk-app/icons");		//XXX hard-coded

	add_action ("preferences", sigc::mem_fun (*this, &GuiApp::menu_preferences));
	add_action ("help",        sigc::mem_fun (*this, &GuiApp::menu_help));
	add_action ("about",       sigc::mem_fun (*this, &GuiApp::menu_about));
	add_action ("quit",        sigc::mem_fun (*this, &GuiApp::menu_quit));

	Glib::RefPtr<Gio::Menu> menu = Gio::Menu::create();

	menu->append ("_Preferences", "app.preferences");
	menu->append ("_Help",        "app.help");
	menu->append ("_About",       "app.about");
	menu->append ("_Quit",        "app.quit");

	set_app_menu (menu);
#if 0
	Gio::ActionMap*                  m = dynamic_cast<Gio::ActionMap*> (this);
	Glib::RefPtr<Gio::Action>        a = m->lookup_action ("banana");
	Glib::RefPtr<Gio::SimpleAction>  s = Glib::RefPtr<Gio::SimpleAction>::cast_dynamic(a);
	s->set_enabled (false);
#endif
}

/**
 * on_activate
 */
void
GuiApp::on_activate()
{
	//LOG_TRACE;
	Gtk::Application::on_activate();

	Window* dp = new Window();
	add_window (*dp);	// App now owns Window
	dp->show();

#if 0
	// make all windows visible
	for (auto win : Gtk::Application::get_windows()) {
		win->present();
	}
#endif
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
GuiApp::on_window_removed (Gtk::Window* window)
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
	std::cout << "Open files:" << std::endl;
	for (auto f : files) {
		std::cout << f->get_uri() << std::endl;
	}

	std::cout << "hint = " << hint << std::endl << std::endl;
	Gtk::Application::on_open (files, hint);
}

/**
 * on_command_line
 */
int
GuiApp::on_command_line (const Glib::RefPtr<Gio::ApplicationCommandLine>& command_line)
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
	LOG_TRACE;
}

/**
 * menu_help
 */
void
GuiApp::menu_help (void)
{
	LOG_TRACE;
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

	about.set_name ("Window");
	about.set_program_name ("dparted");
	about.set_version ("0.1");
	about.set_logo_icon_name ("dparted");
	about.set_copyright ("Copyright \xc2\xa9 2014 Richard Russon (FlatCap)\n");
	about.set_comments ("Reorganise your disk.");
	about.set_website ("http://flatcap.org/rich");
	about.set_website_label ("Rich's homepage");
	about.set_authors (authors);
	about.set_documenters (documenters);
	about.set_translator_credits ("Bjorn, Volker, Pierre");
	about.set_artists (artists);
	about.set_license_type (Gtk::LICENSE_GPL_3_0);

	//about.set_transient_for (recent_changes);

	about.set_icon_name ("dparted");

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
 * on_action_file_open
 */
void
GuiApp::on_action_file_open (void)
{
	LOG_TRACE;
}

/**
 * on_action_file_close
 */
void
GuiApp::on_action_file_close (void)
{
	LOG_TRACE;
}

/**
 * on_action_file_quit
 */
void
GuiApp::on_action_file_quit (void)
{
	LOG_TRACE;
}

/**
 * on_action_plugin
 */
void
GuiApp::on_action_plugin (void)
{
	LOG_TRACE;
}

/**
 * on_action_help
 */
void
GuiApp::on_action_help (void)
{
	LOG_TRACE;
}


/**
 * ask
 */
bool
GuiApp::ask (QuestionPtr q)
{
	//vq.push_back (q);
	Gtk::MessageDialog dialog (q->question, false, Gtk::MessageType::MESSAGE_QUESTION, Gtk::ButtonsType::BUTTONS_NONE, true);

	dialog.set_title (q->title);

	int id = 0;
	for (auto a : q->answers) {
		dialog.add_button (a, id++);
	}

	q->result = dialog.run();
	std::cout << "question = " << q->result << std::endl;
	q->done();

#if 0
	enum Gtk::ResponseType
	{
		RESPONSE_NONE = -1,
		RESPONSE_REJECT = -2,
		RESPONSE_ACCEPT = -3,
		RESPONSE_DELETE_EVENT = -4,
		RESPONSE_OK = -5,
		RESPONSE_CANCEL = -6,
		RESPONSE_CLOSE = -7,
		RESPONSE_YES = -8,
		RESPONSE_NO = -9,
		RESPONSE_APPLY = -10,
		RESPONSE_HELP = -11
	};
#endif


	return true;
}

/**
 * ask_pass
 */
bool
GuiApp::ask_pass (PasswordDialogPtr pw)
{
	//Queue it for later
	passwd = pw;
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
 * properties
 */
void
GuiApp::properties (GfxContainerPtr c)
{
	PropertiesDialog* p = new PropertiesDialog(c, get_active_window());
	p->show();
	//XXX keep weak pointers to PropertiesDialog dialogs
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

