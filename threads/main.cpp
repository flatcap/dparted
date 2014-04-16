#include "window.h"
#include <gtkmm/application.h>

int
main (int argc, char* argv[])
{
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "org.flatcap.example");

	Window window;

	return app->run(window);
}

