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

ChangePasswordDialog::ChangePasswordDialog (void) :
	Dialog (Gtk::MessageType::MESSAGE_OTHER)
{
	image.set_from_icon_name ("dialog-password", Gtk::BuiltinIconSize::ICON_SIZE_DIALOG);
	set_image (image);

	Gtk::Box* ma = get_message_area();
	ma->pack_start (text1);

	Gtk::Box* ca = get_content_area();
	ca->pack_start (text2);
	ca->pack_start (text3);

	sp_box.pack_start (sp_toggle, Gtk::PackOptions::PACK_SHRINK);
	sp_box.pack_start (sp_label, Gtk::PackOptions::PACK_SHRINK);
	ca->pack_start (sp_box);
	sp_toggle.set_active (false);
	sp_toggle.signal_toggled().connect (sigc::mem_fun (this,&ChangePasswordDialog::on_sp_toggle));

	sp_label.set_use_underline (true);
	sp_label.set_text_with_mnemonic ("Show _Password");
	sp_label.set_mnemonic_widget (sp_toggle);

	text1.set_visibility (false);
	text2.set_visibility (false);
	text3.set_visibility (false);

	add_button ("b_1", 101);
	add_button ("b_2", 102);
	add_button ("b_3", 103);
}

ChangePasswordDialog::~ChangePasswordDialog()
{
}

ChangePasswordDialogPtr
ChangePasswordDialog::create (void)
{
	return ChangePasswordDialogPtr (new ChangePasswordDialog());
}

void
ChangePasswordDialog::response (int button_id)
{
	log_debug ("ChangePasswordDialog::response = %d\n", button_id);
}

int
ChangePasswordDialog::run (void)
{
	add_buttons();

	set_title (title);
	set_message (primary);
	set_secondary_text (secondary);

	return Dialog::run();
}

void
ChangePasswordDialog::on_sp_toggle (void)
{
	text1.set_visibility (sp_toggle.get_active());
	text2.set_visibility (sp_toggle.get_active());
	text3.set_visibility (sp_toggle.get_active());
}

