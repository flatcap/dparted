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

#ifndef _CHANGE_PASSWORD_DIALOG_H_
#define _CHANGE_PASSWORD_DIALOG_H_

#include <gtkmm/grid.h>
#include <gtkmm/entry.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/label.h>

#include "dialog.h"

typedef std::shared_ptr<class ChangePasswordDialog> ChangePasswordDialogPtr;

class ChangePasswordDialog : public Dialog
{
public:
	virtual ~ChangePasswordDialog();

	static ChangePasswordDialogPtr create (QuestionPtr q);

	virtual int run (void);		// Hide Dialog::run

protected:
	ChangePasswordDialog (QuestionPtr q);
	void response (int button_id);

	void on_sp_toggle (void);

	Gtk::Label old;
	Gtk::Entry text1;
	Gtk::Label new1;
	Gtk::Entry text2;
	Gtk::Label new2;
	Gtk::Entry text3;
	Gtk::CheckButton sp_toggle;
	Gtk::Box sp_box;
	Gtk::Label sp_label;
	Gtk::Grid grid;
};

#endif // _CHANGE_PASSWORD_DIALOG_H_

