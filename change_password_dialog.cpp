
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
	p->self = p;

	return p;
}

int
ChangePasswordDialog::run (void)
{
	return Gtk::MessageDialog::run();
}

void
ChangePasswordDialog::on_dialog_response (int response_id)
{
	log_debug ("Button: %d\n", response_id);
}

