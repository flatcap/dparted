#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <thread>

#include <gtkmm.h>

#include "worker.h"

class Window : public Gtk::Window
{
public:
	Window (void);
	virtual ~Window();

	void notify (void);

private:
	void on_start_button_clicked (void);
	void on_stop_button_clicked (void);
	void on_quit_button_clicked (void);

	void update_start_stop_buttons (void);
	void update_widgets (void);

	void on_notification_from_worker_thread (void);

	Gtk::Box m_VBox;
	Gtk::ButtonBox m_ButtonBox;
	Gtk::Button m_ButtonStart;
	Gtk::Button m_ButtonStop;
	Gtk::Button m_ButtonQuit;
	Gtk::ProgressBar m_ProgressBar;
	Gtk::Entry m_Entry;

	Glib::Dispatcher m_Dispatcher;
	Worker m_Worker;
	std::thread *m_WorkerThread = nullptr;
};

#endif // _WINDOW_H_

