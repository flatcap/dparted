#ifndef _QUESTION_DIALOG_H_
#define _QUESTION_DIALOG_H_

#include <gtkmm/messagedialog.h>
#include <gtkmm/entry.h>
#include <gtkmm/button.h>

class QuestionDialog : public Gtk::MessageDialog
{
public:
	QuestionDialog (void);
	virtual ~QuestionDialog();

protected:
	Gtk::Entry text1;
	Gtk::Entry text2;
	Gtk::Image i;

	void on_dialog_response (int response_id);
};

#endif // _QUESTION_DIALOG_H_

