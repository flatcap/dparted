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

#ifndef _DIALOG_H_
#define _DIALOG_H_

#include <memory>

#include <gtkmm/messagedialog.h>

#include "log.h"

class Dialog;

typedef std::shared_ptr<Dialog> DialogPtr;

class Dialog : public Gtk::MessageDialog
{
public:
	Dialog (Gtk::MessageType type);
	virtual ~Dialog();

	std::string title;
	std::string primary;
	std::string secondary;

	std::string help_url;

	std::vector<std::pair<std::string,int>> buttons;

	bool ignore_escape = false;

protected:
	void on_help (void);
	void add_buttons (void);

	virtual void response (int button) = 0;
	virtual bool on_event (GdkEvent* event);
};

#endif // _DIALOG_H_

