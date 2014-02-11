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

#ifndef _PASSWORD_DIALOG_H_
#define _PASSWORD_DIALOG_H_

#include <memory>

//#include <gtkmm/dialog.h>
#include <gtkmm/messagedialog.h>

class PasswordDialog;

typedef std::shared_ptr<PasswordDialog> PasswordDialogPtr;

#if 0
MessageDialog (
	Gtk::Window& parent,
	const Glib::ustring& message,
	bool use_markup=false,
	MessageType type=MESSAGE_INFO,
	ButtonsType buttons=BUTTONS_OK,
	bool modal=false
)
#endif

class PasswordDialog : public Gtk::MessageDialog
{
public:
	PasswordDialog (const Glib::ustring& message);
	virtual ~PasswordDialog();

	static PasswordDialogPtr create (void);

	std::string title;

protected:
	std::weak_ptr<PasswordDialog> weak;

};

#endif // _PASSWORD_DIALOG_H_

