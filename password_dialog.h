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

#include <gtkmm/image.h>
#include <gtkmm/entry.h>

#include <gtkmm/messagedialog.h>

class PasswordDialog;

typedef std::shared_ptr<PasswordDialog> PasswordDialogPtr;

class PasswordDialog : public Gtk::MessageDialog
{
public:
	virtual ~PasswordDialog();

	static PasswordDialogPtr create (void);

	std::string title;

	int run (void);		// Hide Dialog::run

protected:
	PasswordDialog();
	void on_dialog_response (int response_id);

	Gtk::Entry text1;
	Gtk::Entry text2;
	Gtk::Entry text3;
	Gtk::Image image;

	std::weak_ptr<PasswordDialog> self;
};

#endif // _PASSWORD_DIALOG_H_

