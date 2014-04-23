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

#ifndef _GUI_APP_H_
#define _GUI_APP_H_

#include <gtkmm/application.h>
#include <glibmm/dispatcher.h>
#include <memory>

#include "app.h"
#include "message.h"
#include "gfx_container.h"
#include "theme.h"
#include "password_dialog.h"
#include "window.h"

class GuiApp;

typedef std::shared_ptr<GuiApp> GuiAppPtr;

extern GuiAppPtr gui_app;

class GuiApp :
	public App,
	public Gtk::Application
{
public:
	GuiApp (void);
	virtual ~GuiApp();

	virtual bool ask      (QuestionPtr q);
	virtual bool notify (Message& m);
	virtual bool ask_pass (PasswordDialogPtr pw);
	virtual void properties (GfxContainerPtr c);
	ThemePtr get_theme (void);
	void set_theme (ThemePtr theme);

	bool set_config (const std::string& filename);
	bool set_theme  (const std::string& filename);

	void on_action_file_open  (void);
	void on_action_file_close (void);
	void on_action_file_quit  (void);
	void on_action_plugin     (void);
	void on_action_help       (void);

protected:
	virtual void on_activate (void);
	virtual void on_startup  (void);

	virtual void on_window_added   (Gtk::Window* window);
	virtual void on_window_removed (Gtk::Window* window);

	virtual void on_open         (const type_vec_files& files, const Glib::ustring& hint);
	virtual int  on_command_line (const Glib::RefPtr<Gio::ApplicationCommandLine>& command_line);

	void show_window   (void);

	void menu_preferences (void);
	void menu_help        (void);
	void menu_about       (void);
	void menu_quit        (void);

	bool on_mouse_click (GdkEventButton* event);

	std::vector<QuestionPtr> vq;
	PasswordDialogPtr passwd;
	ThemePtr theme;
	WindowPtr window;
	Glib::Dispatcher dispatcher;
	void on_dispatch (void);
	void scan_callback (ContainerPtr c);
};


#endif // _GUI_APP_H_

