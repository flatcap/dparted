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

#include "change_password_dialog.h"

ChangePasswordDialog::ChangePasswordDialog (QuestionPtr q) :
	Dialog(q),
	old  ("", 0.0, 0.5),
	new1 ("", 0.0, 0.5),
	new2 ("", 0.0, 0.5)
{
	log_ctor ("ctor ChangePasswordDialog");

	Gtk::Box* ca = get_content_area();
	// ca->set_spacing(0);

	ca->pack_start (grid);

#if 0
	ca->pack_start (old);
	ca->pack_start (text1);
	ca->pack_start (new1);
	ca->pack_start (text2);
	ca->pack_start (new2);
	ca->pack_start (text3);
#else
	grid.set_row_spacing(6);
	grid.set_column_spacing (12);
	grid.attach (old,   0, 0, 1, 1);
	grid.attach (text1, 1, 0, 1, 1);
	grid.attach (new1,  0, 1, 1, 1);
	grid.attach (text2, 1, 1, 1, 1);
	grid.attach (new2,  0, 2, 1, 1);
	grid.attach (text3, 1, 2, 1, 1);
#endif

	text1.set_width_chars (40);
	text2.set_width_chars (40);
	text3.set_width_chars (40);

	sp_box.pack_start (sp_toggle, Gtk::PackOptions::PACK_SHRINK);
	sp_box.pack_start (sp_label, Gtk::PackOptions::PACK_SHRINK);
	ca->pack_start (sp_box);
	sp_toggle.set_active (false);
	sp_toggle.signal_toggled().connect (sigc::mem_fun (this, &ChangePasswordDialog::on_sp_toggle));

	old.set_use_underline (true);
	old.set_text_with_mnemonic ("_Old Password");
	old.set_mnemonic_widget (text1);

	new1.set_use_underline (true);
	new1.set_text_with_mnemonic ("_New Password");
	new1.set_mnemonic_widget (text2);

	new2.set_use_underline (true);
	new2.set_text_with_mnemonic ("_Repeat New Password");
	new2.set_mnemonic_widget (text3);

	sp_label.set_use_underline (true);
	sp_label.set_text_with_mnemonic ("Show _Password");
	sp_label.set_mnemonic_widget (sp_toggle);

	text1.set_visibility (false);
	text2.set_visibility (false);
	text3.set_visibility (false);
}

ChangePasswordDialog::~ChangePasswordDialog()
{
	log_dtor ("dtor ChangePasswordDialog");
}

ChangePasswordDialogPtr
ChangePasswordDialog::create (QuestionPtr q)
{
	return_val_if_fail (q, nullptr);
	return ChangePasswordDialogPtr (new ChangePasswordDialog(q));
}

void
ChangePasswordDialog::response (int button_id)
{
	return_if_fail (question);
	question->result = button_id;
	question->done();
	log_debug ("ChangePasswordDialog::response = %d\n", button_id);
}

int
ChangePasswordDialog::run (void)
{
	return_val_if_fail (question, Gtk::ResponseType::RESPONSE_NONE);
	LOG_TRACE;

	std::string str;

	str = question->get_input ("image");
	if (str.empty()) {
		str = "dialog-password";
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
		add_button ("Cancel", Gtk::ResponseType::RESPONSE_CANCEL);
		add_button ("OK",     Gtk::ResponseType::RESPONSE_OK);
		set_default_response (Gtk::ResponseType::RESPONSE_OK);
	}

	show_all();
	return Dialog::run();
}

void
ChangePasswordDialog::on_sp_toggle (void)
{
	text1.set_visibility (sp_toggle.get_active());
	text2.set_visibility (sp_toggle.get_active());
	text3.set_visibility (sp_toggle.get_active());
}

