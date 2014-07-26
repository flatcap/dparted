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

QuestionDialog::QuestionDialog (QuestionPtr q) :
	Dialog(q)
{
	log_ctor ("ctor QuestionDialog");
}

QuestionDialog::~QuestionDialog()
{
	log_dtor ("dtor QuestionDialog");
}

QuestionDialogPtr
QuestionDialog::create (QuestionPtr q)
{
	return_val_if_fail (q, nullptr);
	return QuestionDialogPtr (new QuestionDialog(q));
}

void
QuestionDialog::response (int button_id)
{
	return_if_fail (question);
	question->result = button_id;
	question->done();
	log_debug ("QuestionDialog::response = %d", button_id);
}

int
QuestionDialog::run (void)
{
	return_val_if_fail (question, Gtk::ResponseType::RESPONSE_NONE);
	LOG_TRACE;

	std::string str;

	str = question->get_input ("image");
	if (str.empty()) {
		str = "dialog-question";
	}

	image.set_from_icon_name (str, Gtk::BuiltinIconSize::ICON_SIZE_DIALOG);
	set_image (image);

	str = question->get_input ("title");
	if (!str.empty()) {
		set_title (str);
	}

	str = question->get_input ("primary");
	if (str.empty()) {
		str = "Error";
	}
	set_message (str);

	str = question->get_input ("secondary");
	set_secondary_text (str);

	if (!add_buttons()) {
		add_button ("_Close", Gtk::ResponseType::RESPONSE_CLOSE);
		set_default_response (Gtk::ResponseType::RESPONSE_CLOSE);
	}

	show_all();
	return Dialog::run();
}

