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


#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <gtkmm/object.h>
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
#include "properties_dialog.h"
#include "log.h"
#include "option_group.h"
#include "utils.h"

#include "change_password_dialog.h"
#include "delete_dialog.h"
#include "error_dialog.h"
#include "info_dialog.h"
#include "password_dialog.h"
#include "properties_dialog.h"
#include "question_dialog.h"
#include "warning_dialog.h"

GuiAppPtr gui_app;

GuiApp::GuiApp (void) :
	Gtk::Application ("org.flatcap.test.area", Gio::ApplicationFlags (Gio::APPLICATION_HANDLES_COMMAND_LINE))
{
	LOG_CTOR;
	Glib::set_application_name ("dparted");

	dispatcher_ask.connect  (sigc::mem_fun (*this, &GuiApp::on_dispatch_ask));
	dispatcher_scan.connect (sigc::mem_fun (*this, &GuiApp::on_dispatch_scan));
}

GuiApp::~GuiApp()
{
	LOG_DTOR;
}


#if 0
bool
GuiApp::my_idle (void)
{
	LOG_TRACE;
	//XXX check that dialog's object hasn't gone away
	if (passwd) {
		passwd->set_title ("Password for X");
		passwd->set_message ("text message");
		passwd->set_secondary_text ("secondary text");
		Gtk::Button cancel ("Cancel");
		Gtk::Image i;
		i.set_from_icon_name ("dialog-password", Gtk::BuiltinIconSize::ICON_SIZE_DIALOG);
		passwd->set_image(i);

		Gtk::Frame frame ("Password:");
		Gtk::RadioButton r1 ("Forget it immediately");
		Gtk::RadioButton r2 ("Remember it until the application closes");
		Gtk::RadioButton r3 ("Save it to disk (encrypted)");
		Gtk::CheckButton c1 ("Remember this setting");

		Gtk::RadioButton::Group group = r1.get_group();
		r2.set_groupn (group);
		r3.set_group (group);
		r2.set_active (true);
		c1.set_active (true);
		Gtk::Box box (Gtk::Orientation::ORIENTATION_VERTICAL, 0);
		frame.add (box);
		box.pack_start (r1, Gtk::PackOptions::PACK_SHRINK, 0);
		box.pack_start (r2, Gtk::PackOptions::PACK_SHRINK, 0);
		box.pack_start (r3, Gtk::PackOptions::PACK_SHRINK, 0);
		box.pack_start (c1, Gtk::PackOptions::PACK_SHRINK, 0);
		passwd->get_content_area()->pack_start (frame);

		passwd->get_action_area()->pack_start (cancel);
		passwd->set_transient_for (*get_active_window());
		passwd->show_all();
		passwd->run();
		passwd = nullptr;
		return false;	// detach
	}

	log_debug ("GuiApp is idle");
	return false;	// continue
}

#endif

void
GuiApp::on_startup (void)
{
	LOG_TRACE;
	Gtk::Application::on_startup();

	Glib::RefPtr<Gtk::IconTheme> itheme = Gtk::IconTheme::get_default();

	itheme->append_search_path ("/home/flatcap/work/dparted/icons");		//XXX theme. hard-coded

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
}

void
GuiApp::on_activate()
{
	LOG_TRACE;
	Gtk::Application::on_activate();

#if 0
	// make all windows visible
	for (auto& win : Gtk::Application::get_windows()) {
		win->present();
	}
#endif
}


void
GuiApp::on_window_added (Gtk::Window* window)
{
	LOG_TRACE;
	Gtk::Application::on_window_added (window);
}

void
GuiApp::on_window_removed (Gtk::Window* window)
{
	LOG_TRACE;
	Gtk::Application::on_window_removed (window);
}


void
GuiApp::show_window (void)
{
	LOG_TRACE;

	if (window) {
		window->show();
		window->present();
	}
}

void
GuiApp::on_open (const type_vec_files& files, const Glib::ustring& hint)
{
	LOG_TRACE;
	log_debug ("Open files:");
	for (auto& f : files) {
		log_debug (f->get_uri());
	}

	log_debug ("hint = %s", SP(hint));
	Gtk::Application::on_open (files, hint);
}


void
GuiApp::scan_callback (ContainerPtr c)
{
	return_if_fail(c);
	return_if_fail (window);
	LOG_TRACE;

	scan_result = c;
	dispatcher_scan.emit();
}

int
GuiApp::on_command_line (const Glib::RefPtr<Gio::ApplicationCommandLine>& command_line)
{
	LOG_TRACE;
	int argc = 0;
	char** argv = command_line->get_arguments (argc);

	Glib::OptionContext context;
	OptionGroup group;
	context.set_main_group (group);
	context.set_summary ("My summary text");
	context.set_description ("My descriptive text");

	try {
		context.parse (argc, argv);
	} catch (const Glib::Error& ex) {
		log_debug ("Exception parsing command-line: %s", SP(ex.what()));
		log_debug (context.get_help());
		//XXX if running, don't kill the app
		return EXIT_FAILURE;
	}

	if (group.quit) {
		if (window) {
			//XXX need to ASK the window to close => WindowPtr
			window = nullptr;
		}
		return EXIT_SUCCESS;
	}

	if (!window) {
		window = std::make_shared<Window>();
		add_window (*window);		// App now owns Window
	}

	std::vector<std::string> disks;		// Mop up any remaining args
	for (; argc > 1; --argc, ++argv) {
		disks.push_back (argv[1]);
	}

	if (group.config.size()) {
		log_debug ("config:");
		for (auto& c : group.config) {
			log_debug ("\t%s", SP(c));
			window->load_config(c);
		}
	}

	gui_app->set_config ("config/dparted.conf");	//XXX temporary defaults
	gui_app->set_theme  ("config/theme.conf");	//XXX temporary defaults

	if (group.theme.size()) {
		log_debug ("theme:");
		for (auto& t : group.theme) {
			log_debug ("\t%s", SP(t));
			window->load_theme(t);
		}
	}

	if ((group.x > -1) || (group.y > -1) || (group.w > -1) || (group.h > -1)) {
		window->set_geometry (group.x, group.y, group.w, group.h);
	}

	show_window();
	activate();

	ContainerPtr top_level;
#ifdef DP_TEST
	if (group.test.empty()) {
#endif
		top_level = scan (disks, std::bind (&GuiApp::scan_callback, this, std::placeholders::_1));
#ifdef DP_TEST
	} else {
		top_level = scan_test (group.test, std::bind (&GuiApp::scan_callback, this, std::placeholders::_1));
	}
#endif
	window->set_data (top_level);

	return EXIT_SUCCESS;
}


void
GuiApp::menu_preferences (void)
{
	LOG_TRACE;
}

void
GuiApp::menu_help (void)
{
	LOG_TRACE;
}

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

	// about.set_transient_for (recent_changes);

	about.set_icon_name ("dparted");

	about.run();
}

void
GuiApp::menu_quit (void)
{
	quit();
}


void
GuiApp::on_action_file_open (void)
{
	LOG_TRACE;
}

void
GuiApp::on_action_file_close (void)
{
	LOG_TRACE;
}

void
GuiApp::on_action_file_quit (void)
{
	LOG_TRACE;
}

void
GuiApp::on_action_plugin (void)
{
	LOG_TRACE;
}

void
GuiApp::on_action_help (void)
{
	LOG_TRACE;
}


bool
GuiApp::ask (QuestionPtr q)
{
	return_val_if_fail (q, false);

	vq.push_back(q);
	dispatcher_ask.emit();
	return true;
}


#ifdef DP_AREA
void
GuiApp::properties (GfxContainerPtr c)
{
	PropertiesDialog* p = new PropertiesDialog (c, get_active_window());
	p->show();
	//XXX keep weak pointers to PropertiesDialog dialogs
}

#endif
ThemePtr
GuiApp::get_theme (void)
{
	return theme;
}


bool
GuiApp::set_config (const std::string& filename)
{
	if (!App::set_config (filename))
		return false;

	// Pick out any gui options we want

	return true;
}

bool
GuiApp::set_theme (const std::string& filename)
{
	ThemePtr tp;

	tp = Theme::read_file (filename);
	if (!tp) {
		// notify the user
		return false;
	}

	if (theme) {
		// if modified ask user if they're sure
	}

	log_listener ("Old Theme %p, New Theme %p", VP(theme), VP(tp));
	theme = tp;
	for (auto& i : theme_listeners) {
		ThemeListenerPtr tl = i.lock();
		if (tl) {
			log_listener ("New Theme %p, notify %p", VP(theme), VP(tl));
			tl->theme_changed (theme);
		} else {
			log_code ("remove listener from the collection");	//XXX remove it from the collection
		}
	}
	// tp->dump_config();

	return true;
}


void
GuiApp::on_dispatch_ask (void)
{
	if (vq.empty()) {
		return;
	}

	QuestionPtr q = vq.front();
	vq.pop_front();

	DialogPtr dlg;
	switch (q->type) {
		case Question::Type::ChangePassword:
			dlg = ChangePasswordDialog::create(q);
			break;
		case Question::Type::Delete:
			dlg = DeleteDialog::create(q);
			break;
		case Question::Type::Error:
			dlg = ErrorDialog::create(q);
			break;
		case Question::Type::Information:
			dlg = InfoDialog::create(q);
			break;
		case Question::Type::Password:
			dlg = PasswordDialog::create(q);
			break;
		case Question::Type::Question:
			dlg = QuestionDialog::create(q);
			break;
		case Question::Type::Warning:
			dlg = WarningDialog::create(q);
			break;
		default:
			log_error ("Unknown question type %d", (int) q->type);
			break;
	}

	if (dlg) {
		dlg->run();
	}

#if 0
	Gtk::MessageDialog dialog (q->question, false, Gtk::MessageType::MESSAGE_QUESTION, Gtk::ButtonsType::BUTTONS_NONE, true);

	dialog.set_title (q->title);

	int id = 0;
	for (auto& a : q->answers) {
		dialog.add_button (a, ++id);
	}

	q->result = dialog.run();
	log_debug ("question = %d", q->result);
	q->reply = "password";
	q->done();	//XXX another thread?  it might take a while.  meanwhile the dialog is still visible
#endif

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
}

void
GuiApp::on_dispatch_scan (void)
{
	// log_info ("scan has finished: ignoring callback");
	// window->set_data (scan_result);
}


bool
GuiApp::open_uri (const std::string& uri)
{
	GError *error = NULL;
	gtk_show_uri (nullptr, uri.c_str(), 0, &error);
	if (error) {
		log_error ("Can't open uri: %s", error->message);
		g_error_free (error);
		return false;
	}

	return true;
}


void
GuiApp::add_listener (const ThemeListenerPtr& tl)
{
	return_if_fail (tl);

	log_listener ("Theme %p add listener: %p", (void*) this, VP(tl));
	theme_listeners.push_back (tl);
}


