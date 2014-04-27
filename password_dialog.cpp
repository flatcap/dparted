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
#include "log.h"
#include "utils.h"

PasswordDialog::PasswordDialog (void) :
	MessageDialog ("", true, Gtk::MessageType::MESSAGE_OTHER, Gtk::ButtonsType::BUTTONS_NONE, false),
	sp_box (Gtk::ORIENTATION_HORIZONTAL)
{
	image.set_from_icon_name ("dialog-password", Gtk::BuiltinIconSize::ICON_SIZE_DIALOG);
	set_image (image);

	Gtk::Box* ma = get_message_area();
	ma->pack_start (text1);

	sp_box.pack_start (sp_toggle, Gtk::PackOptions::PACK_SHRINK);
	sp_box.pack_start (sp_label, Gtk::PackOptions::PACK_SHRINK);
	ma->pack_start (sp_box);
	sp_toggle.set_active (false);
	sp_toggle.signal_toggled().connect (sigc::mem_fun(this,&PasswordDialog::on_sp_toggle));

	sp_label.set_text ("Show password");

	text1.set_visibility (false);
	//text1.set_invisible_char ('@');

	Gtk::Box* ca = get_content_area();
	ca->pack_start (text2);
	ca->pack_start (text3);

	add_button ("b_1", 101);
	add_button ("b_2", 102);
	add_button ("b_3", 103);

	signal_response().connect(sigc::mem_fun(this,&PasswordDialog::on_dialog_response));
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
PasswordDialog::on_dialog_response (int button_id)
{
	log_debug ("Button: %d\n", button_id);
}

int
PasswordDialog::run (void)
{
	Gtk::Button help ("_Help", true);
	if (!help_url.empty()) {
		Gtk::ButtonBox* bb = get_action_area();
		bb->pack_end (help);
		bb->set_child_secondary (help);
		help.signal_clicked().connect (sigc::mem_fun (this, &PasswordDialog::on_help));
	}

	for (auto& i : buttons) {
		add_button (i.first, i.second);
	}

	set_title (title);
	set_message (primary);
	set_secondary_text (secondary);

	show_all();
	show_all_children();

	return Gtk::MessageDialog::run();
}

void
PasswordDialog::on_help (void)
{
	GError *error = nullptr;
	gtk_show_uri (nullptr, help_url.c_str(), 0, &error);
	if (error) {
		log_debug ("Can't open uri: %s\n", error->message);
		g_error_free (error);
	}
	log_debug ("HELP: %s\n", help_url.c_str());
}

bool
PasswordDialog::on_key_press_event (GdkEventKey* event)
{
	if (event->keyval == GDK_KEY_Escape) {
		// Ignore the escape key
		return true;
	} else {
		// Leave everything else alone
		return MessageDialog::on_key_press_event (event);
	}
}

bool
PasswordDialog::on_event (GdkEvent* event)
{
	// Prevent Alt-F4 (delete event)
	return (event->type == GdkEventType::GDK_DELETE);
}

void
PasswordDialog::on_sp_toggle (void)
{
	text1.set_visibility (sp_toggle.get_active());
}

