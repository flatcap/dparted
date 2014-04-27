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

#include "info_dialog.h"
#include "log.h"

InfoDialog::InfoDialog (void) :
	MessageDialog ("", true, Gtk::MessageType::MESSAGE_INFO, Gtk::ButtonsType::BUTTONS_NONE, false)
{
	add_button ("_Close", Gtk::ResponseType::RESPONSE_CLOSE);
	set_default_response (Gtk::ResponseType::RESPONSE_CLOSE);

	signal_response().connect(sigc::mem_fun(this,&InfoDialog::on_dialog_response));
}

InfoDialog::~InfoDialog()
{
}

InfoDialogPtr
InfoDialog::create (void)
{
	return InfoDialogPtr (new InfoDialog());
}

void
InfoDialog::on_dialog_response (int button_id)
{
	log_debug ("Button: %d\n", button_id);
}

int
InfoDialog::run (void)
{
	Gtk::Button help ("_Help", true);
	if (!help_url.empty()) {
		Gtk::ButtonBox* bb = get_action_area();
		bb->pack_end (help);
		bb->set_child_secondary (help);
		help.signal_clicked().connect (sigc::mem_fun (this, &InfoDialog::on_help));
	}

	set_title (title);
	set_message (primary);
	set_secondary_text (secondary);

	show_all();

	return Gtk::MessageDialog::run();
}

void
InfoDialog::on_help (void)
{
	GError *error = nullptr;
	gtk_show_uri (nullptr, help_url.c_str(), 0, &error);
	if (error) {
		log_debug ("Can't open uri: %s\n", error->message);
		g_error_free (error);
	}
	log_debug ("HELP: %s\n", help_url.c_str());
}

