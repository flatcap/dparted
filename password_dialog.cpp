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

PasswordDialog::PasswordDialog (void) :
	Dialog (Gtk::MessageType::MESSAGE_OTHER),
	sp_box (Gtk::ORIENTATION_HORIZONTAL),
	pass_label ("Passphrase:", 0.0, 0.5)
{
	image.set_from_icon_name ("dialog-password", Gtk::BuiltinIconSize::ICON_SIZE_DIALOG);
	set_image (image);

	sp_label.set_use_underline (true);
	sp_label.set_label ("Show _Password");
	sp_label.set_mnemonic_widget (sp_toggle);

	text.set_visibility (false);

	sp_toggle.set_active (false);
	sp_toggle.signal_toggled().connect (sigc::mem_fun (this,&PasswordDialog::on_sp_toggle));

	Gtk::Box* ca = get_content_area();

	ca->pack_start (pass_label);
	ca->pack_start (text);

	sp_box.pack_start (sp_toggle, Gtk::PackOptions::PACK_SHRINK);
	sp_box.pack_start (sp_label, Gtk::PackOptions::PACK_SHRINK);
	ca->pack_start (sp_box);

	show_all();
}

PasswordDialog::~PasswordDialog()
{
}

PasswordDialogPtr
PasswordDialog::create (void)
{
	return PasswordDialogPtr (new PasswordDialog());
}

void
PasswordDialog::response (int button_id)
{
	log_debug ("PasswordDialog::response = %d\n", button_id);
}

int
PasswordDialog::run (void)
{
	add_buttons();

	set_title (title);
	set_message (primary);
	set_secondary_text (secondary);

	return Dialog::run();
}

void
PasswordDialog::on_sp_toggle (void)
{
	text.set_visibility (sp_toggle.get_active());
}

