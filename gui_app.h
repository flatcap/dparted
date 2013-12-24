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

#ifndef _GUI_APP_H_
#define _GUI_APP_H_

#include <gtkmm/application.h>

#include "app.h"
#include "pointers.h"

class Container;

/**
 * class GuiApp
 */
class GuiApp :
	public App,
	public Gtk::Application
{
public:
	GuiApp (ContainerPtr& top_level);
	virtual ~GuiApp();

	virtual bool ask (Question& q);

protected:
	virtual void on_activate (void);
	virtual void on_startup  (void);

	virtual void on_window_added   (Gtk::Window* window);
	virtual void on_window_removed (Gtk::Window* window);

	virtual void on_open         (const type_vec_files& files, const Glib::ustring& hint);
	virtual int  on_command_line (const Glib::RefPtr<Gio::ApplicationCommandLine>& command_line);

	void menu_preferences (void);
	void menu_help        (void);
	void menu_about       (void);
	void menu_quit        (void);

	ContainerPtr top_level;
};


#endif // _GUI_APP_H_

