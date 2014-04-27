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

#include "warning_dialog.h"

WarningDialog::WarningDialog (void) :
	Dialog (Gtk::MessageType::MESSAGE_WARNING)
{
	add_button ("_Close", Gtk::ResponseType::RESPONSE_CLOSE);
	set_default_response (Gtk::ResponseType::RESPONSE_CLOSE);
}

WarningDialog::~WarningDialog()
{
}

WarningDialogPtr
WarningDialog::create (void)
{
	return WarningDialogPtr (new WarningDialog());
}

void
WarningDialog::response (int button_id)
{
	log_debug ("WarningDialog::response = %d\n", button_id);
}

int
WarningDialog::run (void)
{
	add_buttons();

	set_title (title);
	set_message (primary);
	set_secondary_text (secondary);

	return Dialog::run();
}

