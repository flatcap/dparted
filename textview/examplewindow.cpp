#include <iostream>

#include "examplewindow.h"

ExampleWindow::ExampleWindow() :
	box(Gtk::ORIENTATION_VERTICAL),
	m_Button_Quit("_Quit", true)
{
	set_border_width(5);
	set_default_size(400, 200);

	add(box);

	box.pack_start(m_TextView);
	box.pack_start(m_ButtonBox, Gtk::PACK_SHRINK);

	m_ButtonBox.pack_start(m_Button_Quit, Gtk::PACK_SHRINK);
	m_ButtonBox.set_border_width(5);
	m_ButtonBox.set_spacing(5);
	m_ButtonBox.set_layout(Gtk::BUTTONBOX_END);

	m_Button_Quit.signal_clicked().connect(sigc::mem_fun(*this, &ExampleWindow::hide) );

	m_refTextBuffer1 = Gtk::TextBuffer::create();

	m_refTagBold = m_refTextBuffer1->create_tag ("bold");
	m_refTagBold->property_weight() = Pango::WEIGHT_BOLD;
	//m_refTagBold->property_scale() = Pango::SCALE_LARGE;

	m_TextView.set_buffer(m_refTextBuffer1);

	Glib::RefPtr<Pango::Context> ctx = m_TextView.get_pango_context();	// std::cout << "ctx = " << ctx << std::endl;
	Glib::RefPtr<Pango::Layout> lo = Pango::Layout::create (ctx);		// std::cout << "lo = " << lo << std::endl;
	lo->set_markup ("<b>mmmmmmmmmmmmm</b>");

	int width = 0;
	int height = 0;
	lo->get_pixel_size (width, height);
	std::cout << "width = " << width << ", height = " << height << std::endl;

	Pango::TabArray tabs (1, true);
	tabs.set_tab (0, Pango::TabAlign::TAB_LEFT, width+10);
	m_TextView.set_tabs (tabs);

	Gtk::TextBuffer::iterator iter = m_refTextBuffer1->get_iter_at_offset(0);

	iter = m_refTextBuffer1->insert_with_tag(iter, "a", "bold");
	iter = m_refTextBuffer1->insert(iter, "\tb\n");
	iter = m_refTextBuffer1->insert_with_tag(iter, "iiiiii", "bold");
	iter = m_refTextBuffer1->insert(iter, "\tmmmmmm\n");
	iter = m_refTextBuffer1->insert_with_tag(iter, "mmmmmmmmmmmmm", "bold");
	iter = m_refTextBuffer1->insert(iter, "\tiiiiii\n");

	m_TextView.set_editable (false);
	m_TextView.set_cursor_visible (false);

	show_all_children();
}

int main()
{
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "org.gtkmm.example");

	ExampleWindow window;

	return app->run(window);
}

