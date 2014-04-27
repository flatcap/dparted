#ifndef _CHANGE_PASSWORD_DIALOG_H_
#define _CHANGE_PASSWORD_DIALOG_H_

#include <memory>

#include <gtkmm/image.h>
#include <gtkmm/entry.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/label.h>

#include <gtkmm/messagedialog.h>

class ChangePasswordDialog;

typedef std::shared_ptr<ChangePasswordDialog> ChangePasswordDialogPtr;

class ChangePasswordDialog : public Gtk::MessageDialog
{
public:
	virtual ~ChangePasswordDialog();

	static ChangePasswordDialogPtr create (void);

	std::string title;
	std::string primary;
	std::string secondary;
	std::string help_url;

	std::vector<std::pair<std::string,int>> buttons;

	int run (void);		// Hide Dialog::run

protected:
	ChangePasswordDialog();
	void on_dialog_response (int response_id);
	void on_sp_toggle (void);
	void on_help (void);

	Gtk::Entry text1;
	Gtk::Entry text2;
	Gtk::Entry text3;
	Gtk::Image image;

	Gtk::CheckButton sp_toggle;
	Gtk::Box sp_box;
	Gtk::Label sp_label;
};

#endif // _CHANGE_PASSWORD_DIALOG_H_

