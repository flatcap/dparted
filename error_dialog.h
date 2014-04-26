#ifndef _ERROR_DIALOG_H_
#define _ERROR_DIALOG_H_

#include <gtkmm/messagedialog.h>
#include <gtkmm/entry.h>
#include <gtkmm/button.h>

class ErrorDialog : public Gtk::MessageDialog
{
public:
	ErrorDialog (void);
	virtual ~ErrorDialog();

protected:
	Gtk::Entry text1;
	Gtk::Entry text2;
	Gtk::Image i;

	void on_dialog_response (int response_id);
};

#endif // _ERROR_DIALOG_H_

