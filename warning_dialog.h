#ifndef _WARNING_DIALOG_H_
#define _WARNING_DIALOG_H_

#include <gtkmm/messagedialog.h>
#include <gtkmm/entry.h>
#include <gtkmm/button.h>

class WarningDialog : public Gtk::MessageDialog
{
public:
	WarningDialog (void);
	virtual ~WarningDialog();

protected:
	Gtk::Entry text1;
	Gtk::Entry text2;
	Gtk::Image i;

	void on_dialog_response (int response_id);
};

#endif // _WARNING_DIALOG_H_

