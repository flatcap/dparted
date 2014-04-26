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
#define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))

QuestionDialog::QuestionDialog (void) :
	MessageDialog ("", true, Gtk::MessageType::MESSAGE_QUESTION, Gtk::ButtonsType::BUTTONS_NONE, false)
{
	signal_response().connect(sigc::mem_fun(this,&QuestionDialog::on_dialog_response));
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
QuestionDialog::on_dialog_response (int button_id)
{
	log_debug ("Button: %d\n", button_id);
}

int
QuestionDialog::run (void)
{
	Gtk::Button help ("_Help", true);
	if (!help_url.empty()) {
		Gtk::ButtonBox* bb = get_action_area();
		bb->pack_end (help);
		bb->set_child_secondary (help);
		help.signal_clicked().connect (sigc::mem_fun (this, &QuestionDialog::on_help));
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
QuestionDialog::on_help (void)
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
QuestionDialog::on_key_press_event (GdkEventKey* event)
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
QuestionDialog::on_event (GdkEvent* event)
{
	// Prevent Alt-F4 (delete event)
	return (event->type == GdkEventType::GDK_DELETE);
}

