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

#include "question_dialog.h"

QuestionDialog::QuestionDialog (void) :
	Dialog (Gtk::MessageType::MESSAGE_QUESTION)
{
}

QuestionDialog::~QuestionDialog()
{
}

QuestionDialogPtr
QuestionDialog::create (void)
{
	return QuestionDialogPtr (new QuestionDialog());
}

void
QuestionDialog::response (int button_id)
{
	log_debug ("QuestionDialog::response = %d\n", button_id);
}

int
QuestionDialog::run (void)
{
	add_buttons();

	set_title (title);
	set_message (primary);
	set_secondary_text (secondary);

	return Dialog::run();
}

