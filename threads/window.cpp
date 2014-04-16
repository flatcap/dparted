#include <functional>
#include <iostream>
#include <string>
#include <thread>

#include "window.h"

Window::Window (void) :
	m_VBox (Gtk::ORIENTATION_VERTICAL, 5),
	m_ButtonBox (Gtk::ORIENTATION_HORIZONTAL),
	m_ButtonStart ("Start work"),
	m_ButtonStop ("Stop work"),
	m_ButtonQuit ("_Quit", true)
{
	add (m_VBox);

	m_VBox.pack_start (m_ProgressBar, Gtk::PACK_SHRINK);

	m_ProgressBar.set_text ("Fraction done");
	m_ProgressBar.set_show_text();

	m_VBox.pack_start (m_Entry);

	m_Entry.set_editable (false);

	m_VBox.pack_start (m_ButtonBox, Gtk::PACK_SHRINK);

	m_ButtonBox.pack_start (m_ButtonStart, Gtk::PACK_SHRINK);
	m_ButtonBox.pack_start (m_ButtonStop, Gtk::PACK_SHRINK);
	m_ButtonBox.pack_start (m_ButtonQuit, Gtk::PACK_SHRINK);
	m_ButtonBox.set_border_width(5);
	m_ButtonBox.set_spacing(5);
	m_ButtonBox.set_layout (Gtk::BUTTONBOX_END);

	m_ButtonStart.signal_clicked().connect (sigc::mem_fun (*this, &Window::on_start_button_clicked));
	m_ButtonStop .signal_clicked().connect (sigc::mem_fun (*this, &Window::on_stop_button_clicked));
	m_ButtonQuit .signal_clicked().connect (sigc::mem_fun (*this, &Window::on_quit_button_clicked));

	m_Dispatcher.connect (sigc::mem_fun (*this, &Window::on_notification_from_worker_thread));

	update_start_stop_buttons();

	show_all_children();
}

Window::~Window()
{
}

void Window::on_start_button_clicked (void)
{
	if (m_WorkerThread) {
		std::cout << "Can't start a worker thread while another one is running." << std::endl;
	} else {
		m_WorkerThread = new std::thread (std::bind (&Worker::do_work, &m_Worker, (notify_t) std::bind(&Window::notify, this)));

		// Expands to:
		//	notify_t n = std::bind(&Window::notify, this);
		//	auto w = std::bind (&Worker::do_work, &m_Worker, n);
		//	m_WorkerThread = new std::thread (w);
	}
	update_start_stop_buttons();
}

void Window::on_stop_button_clicked (void)
{
	if (!m_WorkerThread) {
		std::cout << "Can't stop a worker thread. None is running." << std::endl;
	} else {
		m_Worker.stop_work();
		m_ButtonStop.set_sensitive (false);
	}
}

void Window::update_start_stop_buttons (void)
{
	const bool thread_is_running = (m_WorkerThread != 0);

	m_ButtonStart.set_sensitive (!thread_is_running);
	m_ButtonStop.set_sensitive (thread_is_running);
}

void Window::update_widgets (void)
{
	double fraction_done;
	std::string message_from_worker_thread;
	m_Worker.get_data (fraction_done, message_from_worker_thread);

	m_ProgressBar.set_fraction (fraction_done);

	if (message_from_worker_thread != m_Entry.get_text()) {
		m_Entry.set_text (message_from_worker_thread);
	}
}

void Window::on_quit_button_clicked (void)
{
	if (m_WorkerThread) {
		m_Worker.stop_work();
		m_WorkerThread->join();
		delete m_WorkerThread;
		m_WorkerThread = nullptr;
	}
	hide();
}

// notify() is called from Worker::do_work(). It is executed in the worker
// thread. It triggers a call to on_notification_from_worker_thread(), which is
// executed in the GUI thread.
void Window::notify (void)
{
	m_Dispatcher.emit();
}

void Window::on_notification_from_worker_thread (void)
{
	if (m_WorkerThread && m_Worker.has_stopped()) {
		// Work is done.
		m_WorkerThread->join();
		m_WorkerThread = nullptr;
		update_start_stop_buttons();
	}
	update_widgets();
}

