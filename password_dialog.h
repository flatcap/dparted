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

#include <gtkmm/entry.h>
#include <gtkmm/image.h>
#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>

#include "dialog.h"

class PasswordDialog;

typedef std::shared_ptr<PasswordDialog> PasswordDialogPtr;

class PasswordDialog : public Dialog
{
public:
	virtual ~PasswordDialog();

	static PasswordDialogPtr create (void);

	virtual int run (void);		// Hide Dialog::run

protected:
	PasswordDialog (void);
	void response (int button_id);

	void on_sp_toggle (void);

	Gtk::Image image;
	Gtk::Entry text;
	Gtk::CheckButton sp_toggle;
	Gtk::Box sp_box;
	Gtk::Label pass_label;
	Gtk::Label sp_label;
};

#endif // _PASSWORD_DIALOG_H_

