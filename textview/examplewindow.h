#ifndef _EXAMPLEWINDOW_H_
#define _EXAMPLEWINDOW_H_

#include <gtkmm.h>

class ExampleWindow : public Gtk::Window
{
public:
	ExampleWindow();
	virtual ~ExampleWindow() {}

protected:
	Gtk::Box box;

	Gtk::TextView m_TextView;

	Glib::RefPtr<Gtk::TextBuffer> m_refTextBuffer1;

	Gtk::ButtonBox m_ButtonBox;
	Gtk::Button m_Button_Quit;

	Glib::RefPtr<Gtk::TextTag> m_refTagBold;
};

#endif // _EXAMPLEWINDOW_H_

