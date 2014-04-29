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

#include "error_dialog.h"

ErrorDialog::ErrorDialog (QuestionPtr q) :
	Dialog(q)
{
}

ErrorDialog::~ErrorDialog()
{
}

ErrorDialogPtr
ErrorDialog::create (QuestionPtr q)
{
	return ErrorDialogPtr (new ErrorDialog(q));
}

void
ErrorDialog::response (int button_id)
{
	log_debug ("ErrorDialog::response = %d\n", button_id);
}

int
ErrorDialog::run (void)
{
	add_buttons();

	image.set_from_icon_name ("dialog-error", Gtk::BuiltinIconSize::ICON_SIZE_DIALOG);
	set_image (image);

	add_button ("_Close", Gtk::ResponseType::RESPONSE_CLOSE);
	set_default_response (Gtk::ResponseType::RESPONSE_CLOSE);

	set_title          (question->input["title"]);	//XXX might create empty map entry
	set_message        (question->input["primary"]);
	set_secondary_text (question->input["secondary"]);

	return Dialog::run();
}

