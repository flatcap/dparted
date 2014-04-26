#ifndef _CHANGE_PASSWORD_DIALOG_H_
#define _CHANGE_PASSWORD_DIALOG_H_

#include <memory>

#include <gtkmm/image.h>
#include <gtkmm/entry.h>

#include <gtkmm/messagedialog.h>

class ChangePasswordDialog;

typedef std::shared_ptr<ChangePasswordDialog> ChangePasswordDialogPtr;

class ChangePasswordDialog : public Gtk::MessageDialog
{
public:
	virtual ~ChangePasswordDialog();

	static ChangePasswordDialogPtr create (void);

	std::string title;

	int run (void);		// Hide Dialog::run

protected:
	ChangePasswordDialog();
	void on_dialog_response (int response_id);

	Gtk::Entry text1;
	Gtk::Entry text2;
	Gtk::Entry text3;
	Gtk::Image image;

	std::weak_ptr<ChangePasswordDialog> self;
};

#endif // _CHANGE_PASSWORD_DIALOG_H_

