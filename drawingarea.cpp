#include <iostream>
#include <stdio.h>

#include <gtkmm.h>

#include "drawingarea.h"
#include "container.h"

/**
 * DPDrawingArea
 */
DPDrawingArea::DPDrawingArea()
{
	set_size_request (0, 0);
	set_hexpand (true);
	set_vexpand (false);

	add_events (Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);

	signal_button_press_event() .connect (sigc::mem_fun (*this, &DPDrawingArea::on_mouse_click));
	signal_motion_notify_event().connect (sigc::mem_fun (*this, &DPDrawingArea::on_mouse_motion));
}

/**
 * ~DPDrawingArea
 */
DPDrawingArea::~DPDrawingArea()
{
}


/**
 * draw_rect
 */
void DPDrawingArea::draw_rect (const Cairo::RefPtr<Cairo::Context>& cr, double x, double y, double width, double height, double red, double green, double blue, double alpha)
{
	cr->set_source_rgba (red, green, blue, alpha);
	cr->rectangle (x, y, width, height);
	cr->fill();
}

/**
 * draw_box
 */
void DPDrawingArea::draw_box (const Cairo::RefPtr<Cairo::Context>& cr, double x, double y, double width, double height, double line_width, double red, double green, double blue, double alpha)
{
	cr->set_line_width(line_width);
	cr->set_source_rgba (red, green, blue, alpha);

	double half = line_width/2;

	Cairo::Antialias a = cr->get_antialias();
	cr->set_antialias (Cairo::ANTIALIAS_NONE);

	//cr->set_line_cap (Cairo::LINE_CAP_ROUND);
	//cr->set_line_cap (Cairo::LINE_CAP_SQUARE);
	cr->set_line_join (Cairo::LINE_JOIN_ROUND);

	cr->move_to (x + half,         y + half);
	cr->line_to (x + width - half, y + half);
	cr->line_to (x + width - half, y + height - half);
	cr->line_to (x + half,         y + height - half);
	cr->close_path();

	cr->stroke();
	cr->set_antialias (a);
}


/**
 * get_colour
 */
void DPDrawingArea::get_colour (std::string &name, double &red, double &green, double &blue)
{
	if (name == "disk") {
		red   = 1.0;
		green = 0.0;
		blue  = 0.0;
	} else if (name == "msdos") {
		red   = 1.0;
		green = 1.0;
		blue  = 0.0;
	} else if (name == "partition") {
		red   = 0.0;
		green = 1.0;
		blue  = 0.0;
	} else if (name == "ext2") {
		red   = 0.0;
		green = 0.0;
		blue  = 1.0;
	} else if (name == "ntfs") {
		red   = 0.5;
		green = 0.5;
		blue  = 0.0;
	} else if (name == "swap") {
		red   = 0.3;
		green = 0.3;
		blue  = 0.9;
	} else if (name == "loop") {
		red   = 0.7;
		green = 0.0;
		blue  = 0.7;
	} else if (name == "extended") {
		red   = 0.0;
		green = 0.7;
		blue  = 0.9;
	} else {
		red   = 0.2;
		green = 0.5;
		blue  = 0.2;
	}
}

/**
 * draw_container
 */
void DPDrawingArea::draw_container (const Cairo::RefPtr<Cairo::Context>& cr, int x, int y, int width, int height, DPContainer *c)
{
	double red   = 1.0;
	double green = 1.0;
	double blue  = 1.0;

	//printf ("draw x = %d, y = %d, width = %d, height = %d\n", x, y, width, height);
	get_colour (c->name, red, green, blue);
	draw_rect (cr, x, y, 16,    height,    red, green, blue, 1.0);
	draw_box  (cr, x, y, width, height, 3, red, green, blue, 1.0);
	x      += 16;
	y      +=  3;
	width  -= 16;
	height -=  6;

	// we have got width pixels for c->bytes_size bytes
	//
	double bytes_per_pixel = c->bytes_size / width;
	//printf ("container is %lld bytes (%.0f bytes per pixel)\n", c->bytes_size, bytes_per_pixel);

	unsigned int num = c->children.size();
	for (unsigned int i = 0; i < num; i++) {
		double offset = c->children[i]->parent_offset / bytes_per_pixel;
		width = c->children[i]->bytes_size / bytes_per_pixel;
		//printf ("\e[32m%s\e[0m\n", c->children[i]->name.c_str());
		//printf ("\tparent offset = %lld (%.0f pixels)\n", c->children[i]->parent_offset, offset);
		//printf ("\tsize = %lld (%d pixels)\n", c->children[i]->bytes_size, width);
		draw_container (cr, x + offset, y, width, height, c->children[i]);
	}

}

/**
 * on_draw
 */
bool DPDrawingArea::on_draw (const Cairo::RefPtr<Cairo::Context>& cr)
{
	Gtk::Allocation allocation = get_allocation();

	int width  = allocation.get_width();
	//int height = allocation.get_height();

	//printf ("allocation = %dx%d\n", width, height);

#if 0
	cr->set_antialias (Cairo::ANTIALIAS_NONE);
	cr->set_source_rgba (0, 0, 0, 0.3);
	cr->set_line_width(1);

	for (int i = 0; i < width; i += 50) {
		cr->move_to (i, 0);
		cr->line_to (i, height);
	}
	cr->stroke();
#endif

	//width -= 100;

	if (m_c) {
		int children = m_c->children.size();
		//printf ("%d children\n", children);
		//RAR should only be one child

		for (int c = 0; c < children; c++) {
			draw_container (cr, 0, c*50, width, 47, m_c->children[c]);
		}
	}

#if 0
	Glib::RefPtr<Gtk::StyleContext> style;
	style = get_style_context();

	Gdk::RGBA fg;
	Gdk::RGBA bg;

	fg = style->get_color (Gtk::STATE_FLAG_NORMAL);
	bg = style->get_background_color(Gtk::STATE_FLAG_NORMAL);

	std::cout << "fg = " << fg.get_red() << "," << fg.get_green() << "," << fg.get_blue() << "," << fg.get_alpha() << std::endl;
	std::cout << "bg = " << bg.get_red() << "," << bg.get_green() << "," << bg.get_blue() << "," << bg.get_alpha() << std::endl;
#endif

#if 0
	Glib::RefPtr<Pango::Layout> layout = Pango::Layout::create(cr);

	Pango::FontDescription font; // ("Wingdings 48");
	font.set_family ("Liberation Sans");
	font.set_size (18 * Pango::SCALE);

	//font = layout->get_font_description();
	//std::cout << font.get_family() << std::endl;
	//std::cout << (font.get_size() / Pango::SCALE) << std::endl;

	int stringWidth = 0;
	int stringHeight = 0;

	layout->set_font_description (font);
	layout->set_markup("<b>X---X</b>");

	layout->get_pixel_size(stringWidth, stringHeight);
	std::cout << "text: " << stringWidth << "x" << stringHeight << std::endl;
	draw_rect (cr, (width-stringWidth)/2, (height-stringHeight)/2, stringWidth, stringHeight, 1, 1, 1, 1);
	draw_box  (cr, (width-stringWidth)/2-1, (height-stringHeight)/2-1, stringWidth+2, stringHeight+2, 1, 0, 0, 0, 1);
	cr->move_to ((width-stringWidth)/2, (height-stringHeight)/2);

	//cr->move_to (20, 100);
	cr->set_source_rgb(0.0, 1.0, 0.0);

	layout->update_from_cairo_context (cr);
	layout->show_in_cairo_context (cr);
#endif

	return true;
}


/**
 * on_mouse_motion
 */
bool DPDrawingArea::on_mouse_motion (GdkEventMotion *event)
{
	//std::cout << "mouse motion: (" << event->x << "," << event->y << ")" << std::endl;

	if ((event->x >= 100) && (event->x < 200) &&
	    (event->y >= 50)  && (event->y < 150)) {
	}

	return true;
}

/**
 * on_mouse_click
 */
bool DPDrawingArea::on_mouse_click (GdkEventButton *event)
{
	//std::cout << "mouse click: (" << event->x << "," << event->y << ")" << std::endl;

	if ((event->x >= 100) && (event->x < 200) &&
	    (event->y >= 50)  && (event->y < 150)) {
	}
#if 0
	std::cout << event->type << std::endl;
	std::cout << event->state << std::endl;
	std::cout << event->button << std::endl;
	std::cout << event->time << std::endl;
	std::cout << event->x << std::endl;
	std::cout << event->y << std::endl;

	GdkEventType type;
	GdkWindow *window;
	gint8 send_event;
	guint32 time;
	gdouble x;
	gdouble y;
	gdouble *axes;
	guint state;
	guint button;
	GdkDevice *device;
	gdouble x_root, y_root;
#endif

	return true;
}

/**
 * set_data
 */
void DPDrawingArea::set_data (DPContainer *c)
{
	m_c = c;
	if (!m_c)
		return;

	// invalidate window
	unsigned int children = c->children.size();

	set_size_request (400, 50 * children);
}

