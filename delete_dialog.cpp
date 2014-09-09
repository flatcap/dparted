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

#include <gtkmm/checkbutton.h>
#include <gtkmm/label.h>

#include "delete_dialog.h"
#include "utils.h"

DeleteDialog::DeleteDialog (QuestionPtr q) :
	Dialog(q)
{
	LOG_CTOR;
}

DeleteDialog::~DeleteDialog()
{
	LOG_DTOR;
}

DeleteDialogPtr
DeleteDialog::create (QuestionPtr q)
{
	return_val_if_fail (q, nullptr);
	return DeleteDialogPtr (new DeleteDialog(q));
}

void
DeleteDialog::response (int button_id)
{
	return_if_fail (question);
	question->result = button_id;
	question->done();
	log_debug ("DeleteDialog::response = %d", button_id);
}

int
DeleteDialog::run (void)
{
	return_val_if_fail (question, Gtk::ResponseType::RESPONSE_NONE);
	LOG_TRACE;

	std::string str;

	str = question->get_input ("image");
	if (str.empty()) {
		str = "gtk-delete";
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
		add_button ("Cancel",  Gtk::ResponseType::RESPONSE_CANCEL);
		add_button ("_Delete", 99);
		set_default_response (Gtk::ResponseType::RESPONSE_NONE);
	}

	if (!question->options.empty()) {
		Gtk::Box* ca = get_content_area();
		ca->pack_start (grid);
		grid.set_row_spacing(6);
		grid.set_column_spacing (12);
		int row = 0;

		for (auto& o : question->options) {
			Gtk::CheckButton* control = Gtk::manage (new Gtk::CheckButton());
			Gtk::Label*       desc    = Gtk::manage (new Gtk::Label());
			Gtk::Label*       notes   = Gtk::manage (new Gtk::Label());

			control->set_active (o.value == "1");
			control->set_sensitive (!o.read_only);

			control->signal_toggled().connect (std::bind (&DeleteDialog::control_toggled, this, control, std::ref(o)));

			desc->set_text  (o.description);
			desc->set_alignment (0, 0.5);

			notes->set_text (o.notes);
			notes->set_alignment (0, 0.5);

			grid.attach (*control, 0, row, 1, 1);
			grid.attach (*desc,    1, row, 1, 1);
			grid.attach (*notes,   2, row, 1, 1);
			++row;
		}
	}

	show_all();
	return Dialog::run();
}

void
DeleteDialog::control_toggled (Gtk::CheckButton* check, Option& o)
{
	bool active = check->get_active();
	log_info ("Control toggled: [%c] : %s", active ? 'X' : ' ', SP(o.description));
	o.value = std::to_string ((int) active);
}

