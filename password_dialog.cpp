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

#include "password_dialog.h"

PasswordDialog::PasswordDialog (QuestionPtr q) :
	Dialog(q),
	sp_box (Gtk::ORIENTATION_HORIZONTAL),
	pass_label ("Passphrase:", 0.0, 0.5)
{
	image.set_from_icon_name ("dialog-password", Gtk::BuiltinIconSize::ICON_SIZE_DIALOG);
	set_image (image);

	sp_label.set_use_underline (true);
	sp_label.set_label ("Show _Password");
	sp_label.set_mnemonic_widget (sp_toggle);

	text.set_visibility (false);
	text.set_activates_default (true);

	sp_toggle.set_active (false);
	sp_toggle.signal_toggled().connect (sigc::mem_fun (this,&PasswordDialog::on_sp_toggle));

	Gtk::Box* ca = get_content_area();

	ca->pack_start (pass_label);
	ca->pack_start (text);

	sp_box.pack_start (sp_toggle, Gtk::PackOptions::PACK_SHRINK);
	sp_box.pack_start (sp_label, Gtk::PackOptions::PACK_SHRINK);
	ca->pack_start (sp_box);

}

PasswordDialog::~PasswordDialog()
{
}

PasswordDialogPtr
PasswordDialog::create (QuestionPtr q)
{
	return_val_if_fail(q,nullptr);
	return PasswordDialogPtr (new PasswordDialog(q));
}

void
PasswordDialog::response (int button_id)
{
	return_if_fail (question);
	question->output["password"] = text.get_text();
	question->done();
	log_debug ("PasswordDialog::response = %d\n", button_id);
}

int
PasswordDialog::run (void)
{
	std::string str;
	std::string device = question->get_input ("device");

	str = question->get_input ("title");
	if (!str.empty()) {
		set_title (str);
	}

	str = question->get_input ("primary");
	if (str.empty()) {
		str = "Enter password";
	}
	set_message (str);

	str = question->get_input ("secondary");
	if (str.empty()) {
		str = "for device: " + device;
	}
	set_secondary_text (str);

	if (!add_buttons()) {
		add_button ("OK", Gtk::ResponseType::RESPONSE_OK);
		set_default_response (Gtk::ResponseType::RESPONSE_OK);
	}

	show_all();
	return Dialog::run();
}

void
PasswordDialog::on_sp_toggle (void)
{
	text.set_visibility (sp_toggle.get_active());
}

