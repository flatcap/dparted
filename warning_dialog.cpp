
#include "warning_dialog.h"

WarningDialog::WarningDialog (void) :
	MessageDialog ("", true, Gtk::MessageType::MESSAGE_OTHER, Gtk::ButtonsType::BUTTONS_NONE, false)
{
#if 0
	MESSAGE_INFO
	MESSAGE_WARNING
	MESSAGE_QUESTION
	MESSAGE_ERROR
	MESSAGE_OTHER
#endif
	// set_size_request (400, 400);

	Gtk::Box* ma = get_message_area();
	ma->pack_start (text1);

	Gtk::Box* ca = get_content_area();
	ca->pack_start (text2);

	// Gtk::ButtonBox* bb = get_action_area();

	add_button ("b_1", 101);
	add_button ("b_2", 102);
	add_button ("b_3", 103);

	signal_response().connect (std::bind (&WarningDialog::on_dialog_response, this, std::placeholders::_1));

#if 1
	const char* icon =
		"Information"		// key
		//"dialog-password"		// key
		//"dialog-information"		// light bulb
		//"dialog-warning"		// yellow triangle
		//"dialog-error"		// red cross
		//"dialog-apply"		// green tick
	;
	i.set_from_icon_name (icon, Gtk::BuiltinIconSize::ICON_SIZE_DIALOG);
	set_image(i);
#endif

	// set_title ("title");
	set_message ("message message message message message");
	set_secondary_text ("secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text, secondary text");

	show_all();
}

void
WarningDialog::on_dialog_response (int response_id)
{
	log_debug ("Button: %d\n", response_id);
}

WarningDialog::~WarningDialog()
{
}

