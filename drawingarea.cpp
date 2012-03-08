#include <iostream>

#include <stdio.h>
#include <gtkmm.h>
#include <stdlib.h>

#include "drawingarea.h"
#include "container.h"

/**
 * DPDrawingArea
 */
DPDrawingArea::DPDrawingArea() :
	mouse_close (false)
{
	set_size_request (400, 50);
	set_hexpand (true);
	set_vexpand (false);

	add_events (Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::LEAVE_NOTIFY_MASK);

	signal_button_press_event() .connect (sigc::mem_fun (*this, &DPDrawingArea::on_mouse_click));
	signal_motion_notify_event().connect (sigc::mem_fun (*this, &DPDrawingArea::on_mouse_motion));
	signal_leave_notify_event() .connect (sigc::mem_fun (*this, &DPDrawingArea::on_mouse_leave));
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
	//std::cout << name << std::endl;
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
		red   = 0.61;
		green = 0.72;
		blue  = 0.82;
	} else if (name == "ntfs") {
		red   = 0.25;
		green = 0.89;
		blue  = 0.67;
	} else if (name == "swap") {
		red   = 0.75;
		green = 0.39;
		blue  = 0.35;
	} else if (name == "loop") {
		red   = 0.0;
		green = 0.0;
		blue  = 1.0;
	} else if (name == "extended") {
		red   = 0.48;
		green = 0.98;
		blue  = 0.99;
	} else if (name == "volume") {
		red   = 0.79;
		green = 0.59;
		blue  = 0.39;
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

	//if (c->type == "extended") {
	if ((c->type != "msdos") && (c->type != "vg segment") && (c->type != "partition")) {
		//printf ("draw x = %4d, y = %4d, width = %4d, height = %4d\n", x, y, width, height);
		get_colour (c->name, red, green, blue);
		if (c->type == "extended") {
			draw_rect (cr, x, y, 10, height, red, green, blue, 1.0);
		} else {
			draw_rect  (cr, x, y, width, height, 1, 1, 1, 1.0);
#if 1
			int r = rand() % (width - 5);
			draw_rect  (cr, x, y, r, height, 0.96, 0.96, 0.72, 1.0);

			Pango::FontDescription font;
			font.set_family ("Liberation Sans");

			int w = 0;
			int h = 0;

			Glib::RefPtr<Pango::Layout> layout = Pango::Layout::create(cr);
			layout->set_font_description (font);
			std::string label;
			size_t pos = m_c->device.find_last_of ('/');
			if (pos == std::string::npos) {
				label = m_c->device;
			} else {
				label = m_c->device.substr (pos+1);
			}
			layout->set_markup("<b>" + label + "</b>");
			layout->get_pixel_size(w, h);
			if ((w + 10) < width) {
				cr->set_source_rgb (0.0, 0.0, 0.0);
				//cr->move_to (x + (width-w)/2, y + (height-h)/2);
				cr->move_to (x + 10, y + (height-h)/2);
				layout->update_from_cairo_context (cr);
				layout->show_in_cairo_context (cr);
			}
#endif
		}
		draw_box  (cr, x, y, width, height, 3, red, green, blue, 1.0);
		x      += 10;
		y      +=  3;
		width  -= 10;
		height -=  6;

	}

	//width -= 3;

	// we have got width pixels for c->bytes_size bytes

	double bytes_per_pixel = c->bytes_size / width;
	//printf ("container (%s) is %lld bytes (%.0f bytes per pixel)\n", c->name.c_str(), c->bytes_size, bytes_per_pixel);

	unsigned int num = c->children.size();
	for (unsigned int i = 0; i < num; i++) {
		double offset = c->children[i]->parent_offset / bytes_per_pixel;
		width = c->children[i]->bytes_size / bytes_per_pixel;
		width -= 2;
		//printf ("\e[32m%s\e[0m\n", c->children[i]->name.c_str());
		//printf ("\tparent offset = %lld (%.0f pixels)\n", c->children[i]->parent_offset, offset);
		//printf ("\tsize = %lld (%d pixels)\n", c->children[i]->bytes_size, width);
		draw_container (cr, x + offset, y, width, height, c->children[i]);
	}
}

/**
 * write_label
 */
void DPDrawingArea::write_label (const Cairo::RefPtr<Cairo::Context>& cr, const Glib::ustring &text)
{
	//std::cout << text << std::endl;

	Glib::RefPtr<Pango::Layout> layout = Pango::Layout::create(cr);

	Pango::FontDescription font;
	font.set_family ("Liberation Sans");
	//font.set_size (18 * Pango::SCALE);

	int w = 0;
	int h = 0;

	layout->set_font_description (font);
	layout->set_markup(text);

	layout->get_pixel_size(w, h);

	int radius = 2 + (h/2);
	int bar    = w - (h/2);

	//std::cout << "text   = " << w << "," << h << std::endl;
	//std::cout << "radius = " << radius << std::endl;
	//std::cout << "bar    = " << bar << std::endl;

	if (text == "<b>loop3</b>") {
		cr->set_source_rgb (0.79, 0.59, 0.39);
	} else {
		cr->set_source_rgb (1.0, 1.0, 1.0);
	}

	cr->arc (2+radius, 45-radius, radius, M_PI/2, 3*M_PI/2);
	cr->arc (2+radius+bar, 45-radius, radius, 3*M_PI/2, M_PI/2);
	cr->rectangle (2+radius, 45-(2*radius), bar, 2*radius);
	cr->fill();

	cr->set_source_rgb(0.0, 0.0, 0.0);
	cr->set_line_width(1);

	cr->arc (2+radius, 45-radius, radius, M_PI/2, 3*M_PI/2);
	cr->arc (2+radius+bar, 45-radius, radius, 3*M_PI/2, M_PI/2);
	cr->move_to (2+radius, 45-(2*radius));
	cr->line_to (2+radius+bar, 45-(2*radius));
	cr->move_to (2+radius, 45);
	cr->line_to (2+radius+bar, 45);
	cr->stroke();

	cr->set_source_rgb(0.0, 0.0, 0.0);
	cr->move_to (3+radius-(radius/2), 44-h);
	layout->update_from_cairo_context (cr);
	layout->show_in_cairo_context (cr);
}

/**
 * on_draw
 */
bool DPDrawingArea::on_draw (const Cairo::RefPtr<Cairo::Context>& cr)
{
	if (!m_c)
		return true;

	//if (m_c->device != "/dev/loop3") return true;

	Gtk::Allocation allocation = get_allocation();

	int width  = allocation.get_width();
	//int height = allocation.get_height();

	//printf ("allocation = %dx%d\n", width, height);

	//width -= 100;

	if (m_c && (m_c->children.size() > 0)) {
		int children = m_c->children.size();
		//printf ("%d children\n", children);
		//RAR should only be one child
		//printf ("child is a %s\n", m_c->children[0]->type.c_str());

		for (int c = 0; c < children; c++) {
			draw_container (cr, 50, c*50, width-50, 47, m_c->children[c]);
		}
		if (!mouse_close) {
			std::string label;
			size_t pos = m_c->device.find_last_of ('/');
			if (pos == std::string::npos) {
				label = m_c->device;
			} else {
				label = m_c->device.substr (pos+1);
			}
			write_label (cr, "<b>" + label + "</b>");
		}
	}

#if 1
	Glib::RefPtr<Gdk::Pixbuf> pb = render_icon_pixbuf (Gtk::Stock::HARDDISK, Gtk::IconSize (Gtk::ICON_SIZE_LARGE_TOOLBAR));
	Gdk::Cairo::set_source_pixbuf(cr, pb, 2, 0);
	cr->rectangle(2, 0, pb->get_width()+2, pb->get_height());
	cr->fill();
#endif
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
	return true;
}


/**
 * on_mouse_motion
 */
bool DPDrawingArea::on_mouse_motion (GdkEventMotion *event)
{
	//std::cout << "mouse motion: (" << event->x << "," << event->y << ")" << std::endl;

	bool old = mouse_close;

	mouse_close = ((event->y > 25) and (event->x < 90));

	if (mouse_close != old) {
		get_window()->invalidate (false); //RAR everything for now
	}

	return true;
}

/**
 * on_mouse_leave
 */
bool DPDrawingArea::on_mouse_leave (GdkEventCrossing *event)
{
	if (mouse_close) {
		mouse_close = false;
		get_window()->invalidate (false); //RAR everything for now
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

