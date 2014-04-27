
#include <glibmm.h>

#include "change_password_dialog.h"
// #include "log.h"

ChangePasswordDialog::ChangePasswordDialog (void) :
	MessageDialog ("", true, Gtk::MessageType::MESSAGE_OTHER, Gtk::ButtonsType::BUTTONS_NONE, false)
{
	// log_ctor ("ctor ChangePasswordDialog");

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
	sp_toggle.signal_toggled().connect (sigc::mem_fun(this,&ChangePasswordDialog::on_sp_toggle));

	sp_label.set_use_underline (true);
	sp_label.set_text_with_mnemonic ("Show _Password");
	sp_label.set_mnemonic_widget (sp_toggle);

	text1.set_visibility (false);
	text2.set_visibility (false);
	text3.set_visibility (false);

	add_button ("b_1", 101);
	add_button ("b_2", 102);
	add_button ("b_3", 103);

	signal_response().connect (std::bind (&ChangePasswordDialog::on_dialog_response, this, std::placeholders::_1));

	// set_title ("title");
	set_message ("message message message message message");
	set_secondary_text ("secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text");

	show_all();
}

ChangePasswordDialog::~ChangePasswordDialog()
{
	// log_dtor ("dtor ChangePasswordDialog");
}


ChangePasswordDialogPtr
ChangePasswordDialog::create (void)
{
	ChangePasswordDialogPtr p (new ChangePasswordDialog());
	return p;
}

int
ChangePasswordDialog::run (void)
{
	Gtk::Button help ("_Help", true);
	if (!help_url.empty()) {
		Gtk::ButtonBox* bb = get_action_area();
		bb->pack_end (help);
		bb->set_child_secondary (help);
		help.signal_clicked().connect (sigc::mem_fun (this, &ChangePasswordDialog::on_help));
	}

	set_title (title);
	set_message (primary);
	set_secondary_text (secondary);

	for (auto& i : buttons) {
		add_button (i.first, i.second);
	}

	show_all();

	return Gtk::MessageDialog::run();
}

void
ChangePasswordDialog::on_dialog_response (int response_id)
{
	log_debug ("Button: %d\n", response_id);
}

void
ChangePasswordDialog::on_sp_toggle (void)
{
	text1.set_visibility (sp_toggle.get_active());
	text2.set_visibility (sp_toggle.get_active());
	text3.set_visibility (sp_toggle.get_active());
}

void
ChangePasswordDialog::on_help (void)
{
	GError *error = nullptr;
	gtk_show_uri (nullptr, help_url.c_str(), 0, &error);
	if (error) {
		log_debug ("Can't open uri: %s\n", error->message);
		g_error_free (error);
	}
	log_debug ("HELP: %s\n", help_url.c_str());
}


