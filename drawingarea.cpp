#include <iostream>

#include <gtkmm.h>

#include "drawingarea.h"
#include "container.h"

/**
 * DrawingArea
 */
DrawingArea::DrawingArea()
{
	set_size_request (0, 0);
	set_hexpand (true);
	set_vexpand (false);

	//Targets:
	std::vector<Gtk::TargetEntry> listTargets;
	listTargets.push_back( Gtk::TargetEntry("STRING") );
	listTargets.push_back( Gtk::TargetEntry("text/plain") );

	// we are both source and destination of drag'n'drop
	//drag_source_set(listTargets);
	//drag_dest_set(listTargets);

	//signal_drag_data_get().connect(sigc::mem_fun(*this, &DrawingArea::on_drag));
	//signal_drag_data_received().connect(sigc::mem_fun(*this, &DrawingArea::on_drop));

	add_events (Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);

	signal_button_press_event() .connect (sigc::mem_fun (*this, &DrawingArea::on_mouse_click));
	signal_motion_notify_event().connect (sigc::mem_fun (*this, &DrawingArea::on_mouse_motion));

	cursor = Gdk::Cursor::create (Gdk::FLEUR);
}

/**
 * ~DrawingArea
 */
DrawingArea::~DrawingArea()
{
}


/**
 * draw_rect
 */
void DrawingArea::draw_rect (const Cairo::RefPtr<Cairo::Context>& cr, double x, double y, double width, double height, double red, double green, double blue, double alpha)
{
	cr->set_source_rgba (red, green, blue, alpha);
	cr->rectangle (x, y, width, height);
	cr->fill();
}

/**
 * draw_box
 */
void DrawingArea::draw_box (const Cairo::RefPtr<Cairo::Context>& cr, double x, double y, double width, double height, double line_width, double red, double green, double blue, double alpha)
{
	cr->set_line_width(line_width);
	cr->set_source_rgba (red, green, blue, alpha);

	double half = line_width/2;

	Cairo::Antialias a = cr->get_antialias();
	cr->set_antialias (Cairo::ANTIALIAS_NONE);

	//cr->set_line_cap (Cairo::LINE_CAP_ROUND);
	cr->set_line_cap (Cairo::LINE_CAP_SQUARE);
	//cr->set_line_join (Cairo::LINE_JOIN_ROUND);

	cr->move_to (x + half,         y + half);
	cr->line_to (x + width - half, y + half);
	cr->line_to (x + width - half, y + height - half);
	cr->line_to (x + half,         y + height - half);
	cr->close_path();

	cr->stroke();
	cr->set_antialias (a);
}


/**
 * on_draw
 */
bool DrawingArea::on_draw (const Cairo::RefPtr<Cairo::Context>& cr)
{
	//std::cout << __PRETTY_FUNCTION__ << std::endl;
#if 0
	Gtk::Allocation allocation = get_allocation();

	int width  = allocation.get_width();
	int height = allocation.get_height();

	double line_width = 10;

	double red   = 1.0;
	double green = 0.0;
	double blue  = 0.0;
#endif

	draw_rect (cr, 100, 50, 100, 100, 1, 0, 0, 1);
	draw_rect (cr, 300, 50, 100, 100, 0, 1, 0, 1);

#if 0
	cr->set_antialias (Cairo::ANTIALIAS_NONE);
	cr->set_line_width(1);
	cr->set_source_rgba (0, 0, 0, 1);

	for (int j = 0; j < 400; j += 10) {
		cr->move_to (0, j);
		cr->line_to (width, j);
	}
	cr->stroke();
#endif

#if 0
	double x = 0;
	double y = 0;

	for (int i = 0; i < 4; i++) {
		if (i % 2) {
			red = 1.0; green = 0.0;
		} else {
			red = 0.0; green = 1.0;
		}

		draw_box (cr, x, y, width, height, line_width, red, green, blue, 1.0);

		x += 1*line_width;
		y += 1*line_width;

		width  -= (2 * line_width);
		height -= (2 * line_width);
	}
#endif

#if 0
	//Gtk::StockID id ("gtk-dialog-warning");
	//Gtk::IconSize best_size = Gtk::ICON_SIZE_MENU;
	Gtk::Image image (Gtk::Stock::SAVE, Gtk::ICON_SIZE_MENU);
	Glib::RefPtr<Gdk::Pixbuf> pb = image.get_pixbuf();
	std::cout << "pixbuf = " << pb << std::endl;
	if (pb) {
		Gdk::Cairo::set_source_pixbuf (cr, pb, 100, 80);
		cr->rectangle (200, 100, pb->get_width(), pb->get_height());
		cr->fill();
	}
#endif

#if 0
	Glib::RefPtr<Pango::Layout> layout = Pango::Layout::create(cr);

	Pango::FontDescription font; // ("Wingdings 48");
	font.set_family ("Liberation Sans");
	font.set_size (48 * Pango::SCALE);

	//font = layout->get_font_description();
	//std::cout << font.get_family() << std::endl;
	//std::cout << (font.get_size() / Pango::SCALE) << std::endl;

	layout->set_font_description (font);
	layout->set_text("hello world");

	int stringWidth = 0;
	int stringHeight = 0;
	layout->get_pixel_size(stringWidth, stringHeight);
	//std::cout << "text: " << stringWidth << "x" << stringHeight << std::endl;
	cr->move_to ((width-stringWidth)/2, (height-stringHeight)/2);

	//cr->move_to (20, 100);
	cr->set_source_rgb(0.0, 1.0, 0.0);

	layout->update_from_cairo_context (cr);
	layout->show_in_cairo_context (cr);
#endif

#if 0
	draw_box  (cr, 400, 150, 50, 100, 1, 0, 0, 0, 1.0);
	draw_rect (cr, 400, 150, 50, 100, 1, 0, 0, 0.5);
#endif

	return true;
}


/**
 * on_drag
 */
void DrawingArea::on_drag (const Glib::RefPtr<Gdk::DragContext>&, Gtk::SelectionData& selection_data, guint info, guint time)
{
	selection_data.set(selection_data.get_target(), 8 /* 8 bits format */,
					(const guchar*)"I'm Data!",
					9 /* the length of I'm Data! in bytes */);
}

/**
 * on_drop
 */
void DrawingArea::on_drop (const Glib::RefPtr<Gdk::DragContext>& context, int, int, const Gtk::SelectionData& selection_data, guint info, guint time)
{
	const int length = selection_data.get_length();
	if((length >= 0) && (selection_data.get_format() == 8))
	{
		std::cout << "Received \"" << selection_data.get_data_as_string() << "\" in label " << std::endl;
	}

	context->drag_finish(false, false, time);
}


/**
 * on_mouse_motion
 */
bool DrawingArea::on_mouse_motion (GdkEventMotion *event)
{
	//std::cout << "mouse motion: (" << event->x << "," << event->y << ")" << std::endl;

	if ((event->x >= 100) && (event->x < 200) &&
	    (event->y >= 50)  && (event->y < 150)) {
		get_window()->set_cursor (cursor);
	} else {
		get_window()->set_cursor();
	}

	return true;
}

/**
 * on_mouse_click
 */
bool DrawingArea::on_mouse_click (GdkEventButton *event)
{
	//std::cout << "mouse click: (" << event->x << "," << event->y << ")" << std::endl;

	if ((event->x >= 100) && (event->x < 200) &&
	    (event->y >= 50)  && (event->y < 150)) {

		GdkEvent ev;
		ev.button = *event;
		Glib::RefPtr<Gtk::TargetList> targets = drag_dest_get_target_list();
		Glib::RefPtr<Gdk::DragContext> dc;

		dc = Gtk::Widget::drag_begin (targets, Gdk::ACTION_COPY, 1, &ev);
	}

	return true;
}

/**
 * set_data
 */
void DrawingArea::set_data (DPContainer *c)
{
	m_c = c;
	// invalidate window
}

