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

#include "dialog.h"

Dialog::Dialog (Gtk::MessageType type) :
	MessageDialog ("", true, type, Gtk::ButtonsType::BUTTONS_NONE, false)
{
	signal_response().connect ([&] (int id) { response (id); });
}

Dialog::~Dialog()
{
}

void
Dialog::on_help (void)
{
	GError *error = nullptr;
	gtk_show_uri (nullptr, help_url.c_str(), 0, &error);
	if (error) {
		log_debug ("Can't open uri: %s\n", error->message);
		g_error_free (error);
	} else {
		log_debug ("HELP: %s\n", help_url.c_str());
	}
}

void
Dialog::add_buttons (void)
{
	Gtk::Button help ("_Help", true);
	if (!help_url.empty()) {
		Gtk::ButtonBox* bb = get_action_area();
		bb->pack_end (help);
		bb->set_child_secondary (help);
		help.signal_clicked().connect (std::bind (&Dialog::on_help, this));
	}

	for (auto& i : buttons) {
		add_button (i.first, i.second);
	}
}

bool
Dialog::on_event (GdkEvent* event)
{
	if (ignore_escape && (event->type == GdkEventType::GDK_DELETE)) {
		return true;
	}

	return Gtk::MessageDialog::on_event (event);
}

