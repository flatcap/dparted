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


#include <glibmm.h>

#include "password_dialog.h"
// #include "log.h"

PasswordDialog::PasswordDialog (void) :
	MessageDialog ("", true, Gtk::MessageType::MESSAGE_OTHER, Gtk::ButtonsType::BUTTONS_NONE, false)
{
	// log_ctor ("ctor PasswordDialog");

	image.set_from_icon_name ("dialog-password", Gtk::BuiltinIconSize::ICON_SIZE_DIALOG);
	set_image (image);

	Gtk::Box* ma = get_message_area();
	ma->pack_start (text1);

	Gtk::Box* ca = get_content_area();
	ca->pack_start (text2);
	ca->pack_start (text3);

	add_button ("b_1", 101);
	add_button ("b_2", 102);
	add_button ("b_3", 103);

	signal_response().connect (std::bind (&PasswordDialog::on_dialog_response, this, std::placeholders::_1));

	// set_title ("title");
	set_message ("message message message message message");
	set_secondary_text ("secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text");

	show_all();
}

PasswordDialog::~PasswordDialog()
{
	// log_dtor ("dtor PasswordDialog");
}


PasswordDialogPtr
PasswordDialog::create (void)
{
	PasswordDialogPtr p (new PasswordDialog());
	p->self = p;

	return p;
}

int
PasswordDialog::run (void)
{
	return Gtk::MessageDialog::run();
}

void
PasswordDialog::on_dialog_response (int response_id)
{
	log_debug ("Button: %d\n", response_id);
}

