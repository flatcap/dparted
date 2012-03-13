#include <iostream>

#include <stdio.h>
#include <gtkmm.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

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
	} else if (name == "red") {
		red   = 1.00;
		green = 0.00;
		blue  = 0.00;
	} else if (name == "green") {
		red   = 0.00;
		green = 1.00;
		blue  = 0.00;
	} else if (name == "blue") {
		red   = 0.00;
		green = 0.00;
		blue  = 1.00;
	} else {
		red   = 0.2;
		green = 0.5;
		blue  = 0.2;
	}
}

/**
 * draw_frame
 */
void DPDrawingArea::draw_frame (const Cairo::RefPtr<Cairo::Context>& cr,
				int x, int y, int width, int height,
				DPContainer *c,
				std::string label_left, std::string label_right,
				int width_fs, int width_usage,
				Glib::RefPtr<Gdk::Pixbuf> icon1,
				Glib::RefPtr<Gdk::Pixbuf> icon2,
				Gdk::RGBA &colour)
{
	cr->save();
	cr->arc (x+8,       y+       8, 8,   M_PI,   3*M_PI_2);
	cr->arc (x+width-8, y+       8, 8, 3*M_PI_2,      0);
	cr->arc (x+width-8, y+height-8, 8,      0,     M_PI_2);
	cr->arc (x+8,       y+height-8, 8,   M_PI_2,   M_PI);
	cr->clip();

	cr->set_line_width (2);
	cr->set_source_rgb (colour.get_red(), colour.get_green(), colour.get_blue());
	cr->arc (x+8,       y+8, 8,   M_PI,   3*M_PI_2);
	cr->arc (x+width-8, y+8, 8, 3*M_PI_2, 0);
	cr->stroke();

	cr->move_to (x, y+4);
	cr->set_line_width (8);
	cr->rel_line_to (width, 0);
	cr->stroke();

	cr->set_line_width (2);
	cr->move_to (x+1, y+8);
	cr->rel_line_to (0, height-16);

	cr->move_to (x+width-1, y+8);
	cr->rel_line_to (0, height-16);

	cr->stroke();

	cr->set_line_width (4);
	cr->arc_negative (x+8,       y+height-8, 8, M_PI, M_PI_2);
	cr->arc_negative (x+width-8, y+height-8, 8, M_PI_2, 0);
	cr->stroke();

	cr->restore();
}

/**
 * draw_partition
 */
void DPDrawingArea::draw_partition (const Cairo::RefPtr<Cairo::Context>& cr,
				    int x, int y, int width, int height,
				    DPContainer *c,
				    std::string label_left, std::string label_right,
				    int width_fs, int width_usage,
				    Glib::RefPtr<Gdk::Pixbuf> icon1,
				    Glib::RefPtr<Gdk::Pixbuf> icon2,
				    Gdk::RGBA &colour)
{
	cr->save();
	cr->arc (x+8,       y+       8, 8,   M_PI,   3*M_PI_2);
	cr->arc (x+width-8, y+       8, 8, 3*M_PI_2,      0);
	cr->arc (x+width-8, y+height-8, 8,      0,     M_PI_2);
	cr->arc (x+8,       y+height-8, 8,   M_PI_2,   M_PI);
	cr->clip();

	draw_rect (cr, x, y+8, width_usage, height-8, 0.96, 0.96, 0.72, 1.0); // Yellow usage
	draw_rect (cr, x+width_usage, y+8,  width_fs-width_usage, height-8, 1.00, 1.00, 1.00, 1.0);	// White background

	Cairo::RefPtr<Cairo::LinearGradient> grad;
	grad = Cairo::LinearGradient::create (0.0, 0.0, 0.0, height);
	grad->add_color_stop_rgba (0.00, 0.0, 0.0, 0.0, 0.3);
	grad->add_color_stop_rgba (0.50, 0.0, 0.0, 0.0, 0.0);
	grad->add_color_stop_rgba (1.00, 0.0, 0.0, 0.0, 0.1);
	cr->set_source (grad);
	cr->rectangle (x, y, width, height);	// contents
	cr->fill();

	cr->set_line_width (2);
	cr->set_source_rgb (colour.get_red(), colour.get_green(), colour.get_blue());
	cr->arc (x+8,       y+8, 8,   M_PI,   3*M_PI_2);
	cr->arc (x+width-8, y+8, 8, 3*M_PI_2, 0);
	cr->stroke();

	cr->move_to (x, y+4);
	cr->set_line_width (8);
	cr->rel_line_to (width, 0);
	cr->stroke();

	cr->set_line_width (2);
	cr->move_to (x+1, y+8);
	cr->rel_line_to (0, height-16);

	cr->move_to (x+width-1, y+8);
	cr->rel_line_to (0, height-16);

	cr->stroke();

	cr->set_line_width (4);
	cr->arc_negative (x+8,       y+height-8, 8, M_PI, M_PI_2);
	cr->arc_negative (x+width-8, y+height-8, 8, M_PI_2, 0);
	cr->stroke();

	cr->restore();
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
			draw_rect (cr, x, y, 28, height, red, green, blue, 1.0);
#if 1
			Glib::RefPtr<Gdk::Pixbuf> pb3;

			int ix = x+2;

			pb3 = Gdk::Pixbuf::create_from_file ("icons/table.png");
			Gdk::Cairo::set_source_pixbuf(cr, pb3, ix, 2);
			cr->rectangle(ix, 2, pb3->get_width(), pb3->get_height());
			cr->fill();
#endif
			x += 18;
			width -= 18;
		} else {
			draw_rect  (cr, x, y, width, height, 1, 1, 1, 1.0); // White background
#if 1
			int bytes_per_pixel = c->bytes_size / width;
			int uw = c->bytes_used / bytes_per_pixel;
			draw_rect  (cr, x, y, uw, height, 0.96, 0.96, 0.72, 1.0); // Yellow usage

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
			//layout->set_markup("<b>" + label + "</b>");
			layout->set_text (label);
			layout->get_pixel_size(w, h);
			if ((w + 10) < width) {
				cr->set_source_rgb (0.0, 0.0, 0.0);
				//cr->move_to (x + (width-w)/2, y + (height-h)/2);
				//cr->move_to (x + 10, y + (height-h)/2);
				cr->move_to (x + 4, y + 4);
				layout->update_from_cairo_context (cr);
				layout->show_in_cairo_context (cr);
			}
#if 1
			Glib::RefPtr<Gdk::Pixbuf> pb2;

			int ix = x+5;
#if 0
			pb2 = Gdk::Pixbuf::create_from_file ("icons/partition.png");
			Gdk::Cairo::set_source_pixbuf(cr, pb2, ix, 23);
			cr->rectangle(ix, 23, pb2->get_width(), pb2->get_height());
			cr->fill();
			ix += 24;
#endif

#if 0
			pb2 = Gdk::Pixbuf::create_from_file ("icons/filesystem.png");
			Gdk::Cairo::set_source_pixbuf(cr, pb2, ix, 20);
			cr->rectangle(ix, 20, pb2->get_width(), pb2->get_height());
			cr->fill();
			ix += 24;
#endif

			pb2 = Gdk::Pixbuf::create_from_file ("icons/locked.png");
			Gdk::Cairo::set_source_pixbuf(cr, pb2, ix, 20);
			cr->rectangle(ix, 20, pb2->get_width(), pb2->get_height());
			cr->fill();
			//return true;
#endif

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
	//int bar    = w - (h/2);

	//std::cout << "text   = " << w << "," << h << std::endl;
	//std::cout << "radius = " << radius << std::endl;
	//std::cout << "bar    = " << bar << std::endl;

#if 0
	if (text == "<b>loop3</b>") {
		cr->set_source_rgb (0.79, 0.59, 0.39);
	} else {
		cr->set_source_rgba (1.0, 1.0, 1.0, 0.3);
	}
#endif

#if 0
	cr->set_source_rgba (1.0, 1.0, 1.0, 0.3);

	cr->arc (2+radius, 45-radius, radius, M_PI_2, 3*M_PI_2);
	cr->arc (2+radius+bar, 45-radius, radius, 3*M_PI_2, M_PI_2);
	cr->rectangle (2+radius, 45-(2*radius), bar, 2*radius);
	cr->fill();
#endif

#if 0
	cr->set_source_rgb(0.0, 0.0, 0.0);
	cr->set_line_width(1);

	cr->arc (2+radius, 45-radius, radius, M_PI_2, 3*M_PI_2);
	cr->arc (2+radius+bar, 45-radius, radius, 3*M_PI_2, M_PI_2);
	cr->move_to (2+radius, 45-(2*radius));
	cr->line_to (2+radius+bar, 45-(2*radius));
	cr->move_to (2+radius, 45);
	cr->line_to (2+radius+bar, 45);
	cr->stroke();
#endif

	int x = 3+radius-(radius/2);
	int y = 44-h;

#if 1
	cr->set_source_rgb(1.0, 1.0, 1.0);

	cr->move_to (x+2, y+0); layout->update_from_cairo_context (cr); layout->show_in_cairo_context (cr);
	cr->move_to (x-2, y+0); layout->update_from_cairo_context (cr); layout->show_in_cairo_context (cr);
	cr->move_to (x+0, y+2); layout->update_from_cairo_context (cr); layout->show_in_cairo_context (cr);
	cr->move_to (x+0, y-2); layout->update_from_cairo_context (cr); layout->show_in_cairo_context (cr);
#endif

	cr->set_source_rgb(0.0, 0.0, 0.0);
	cr->move_to (x, y);
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
	if (m_c->device != "/dev/sdc") return true;

	Gtk::Allocation allocation = get_allocation();

	int width  = allocation.get_width();
	int height = allocation.get_height();

	printf ("allocation = %dx%d\n", width, height);

	if (m_c->device == "/dev/sdc") {
		int x = 50;
		int y = 0;
		int w = 400;
		int h = 77;	//47;
		DPContainer *c = NULL;
		std::string left;
		std::string right;
		int width_fs = w;
		int width_usage = 35*w/100;
		Glib::RefPtr<Gdk::Pixbuf> icon1;
		Glib::RefPtr<Gdk::Pixbuf> icon2;
		Gdk::RGBA colour;

		double red, green, blue;
		std::string name;

		name = "ext2";
		get_colour (name, red, green, blue);
		colour.set_rgba (red, green, blue);

		draw_partition (cr, x, y, w, h, c, left, right, width_fs, width_usage, icon1, icon2, colour);
		x += w + 4;

		name = "extended";
		get_colour (name, red, green, blue);
		colour.set_rgba (red, green, blue);
		w = 800;
		width_usage = 85*w/100;

		draw_frame (cr, x, y, w, h, c, left, right, width_fs, width_usage, icon1, icon2, colour);

		name = "red";
		get_colour (name, red, green, blue);
		colour.set_rgba (red, green, blue);
		w = 250;
		width_usage = 15*w/100;

		x += 2; y += 8; h -= 10;

		draw_partition (cr, x, y, w, h, c, left, right, width_fs, width_usage, icon1, icon2, colour);
		x += w+2;

		name = "green";
		get_colour (name, red, green, blue);
		colour.set_rgba (red, green, blue);
		w = 350;
		width_usage = 15*w/100;

		draw_frame (cr, x, y, w, h, c, left, right, width_fs, width_usage, icon1, icon2, colour);

		return true;
	}

#if 0	// gradient
#if 1
	draw_rect  (cr, 20, 2, 170, 46, 1, 1, 1, 1.0); // White background
	draw_rect  (cr, 20, 2, 85, 46, 0.96, 0.96, 0.72, 1.0); // Yellow usage
	//draw_box   (cr, 20, 2, 170, 46, 4, 0.9, 0.0, 0, 1.0);
#endif

#if 1
	Glib::RefPtr<Pango::Layout> layout = Pango::Layout::create(cr);
	cr->set_source_rgb(0.0, 0.0, 0.0);
	layout->set_markup ("<b>hello world</b>");
	cr->move_to (30, 20);
	layout->update_from_cairo_context (cr);
	layout->show_in_cairo_context (cr);
#endif

#if 1
	Cairo::RefPtr<Cairo::LinearGradient> grad;
	grad = Cairo::LinearGradient::create (0.0, 0.0, 0.0, 38.0);
	grad->add_color_stop_rgba (0.00, 0.0, 0.0, 0.0, 0.4);
	grad->add_color_stop_rgba (0.50, 0.0, 0.0, 0.0, 0.0);
	grad->add_color_stop_rgba (1.00, 0.0, 0.0, 0.0, 0.2);
	cr->set_source (grad);
	//cr->set_source_rgb (0, 0, 0);
	cr->rectangle (24, 6, 162, 38);	// contents
	//cr->rectangle (20, 2, 170, 46);	// whole
	//cr->rectangle (0, 0, 1400, 50);
	cr->fill();
#endif

#if 0
	cr->set_source_rgba (0, 0, 0, 1);
	cr->set_line_width (1);
	cr->move_to (250, 0);
	cr->rel_line_to (0, 50);
	cr->stroke();
	cr->move_to (750, 0);
	cr->rel_line_to (0, 50);
	cr->stroke();
#endif
	return true;
#endif // gradient

#if 0
	Glib::RefPtr<Gdk::Pixbuf> pb;
	pb = render_icon_pixbuf (Gtk::Stock::HARDDISK, Gtk::IconSize (Gtk::ICON_SIZE_MENU));
	for (int i = 0; i < 400; i += 50) {
		Gdk::Cairo::set_source_pixbuf(cr, pb, i, 0);
		cr->rectangle(i, 0, pb->get_width(), pb->get_height());
		cr->fill();
	}
	return true;
#endif

#if 1
	const char *file = NULL;
	if (m_c->device.substr (0, 9) == "/dev/loop")
		file = "/usr/share/icons/gnome/48x48/actions/gtk-refresh.png";
	else if (m_c->device.substr (0, 7) == "/dev/sd")
		file = "/usr/share/icons/gnome/48x48/devices/harddrive.png";
	else if (m_c->device.substr (0, 5) != "/dev/")
		file = "/usr/share/icons/gnome/48x48/mimetypes/txt.png";
	else
		file = "/usr/share/icons/gnome/48x48/status/connect_established.png";

	Glib::RefPtr<Gdk::Pixbuf> pb;
	pb = Gdk::Pixbuf::create_from_file (file);
	Gdk::Cairo::set_source_pixbuf(cr, pb, 2, 0);
	cr->rectangle(2, 0, pb->get_width(), pb->get_height());
	cr->fill();
#endif

#if 0
	cr->save();
	cr->rectangle (0, 0, 25, 50);
	cr->clip();
	cr->restore();
#endif

#if 0
	Glib::RefPtr<Gdk::Pixbuf> pb;
	pb = render_icon_pixbuf (Gtk::Stock::HARDDISK, Gtk::IconSize (Gtk::ICON_SIZE_LARGE_TOOLBAR));
	Gdk::Cairo::set_source_pixbuf(cr, pb, 2, 0);
	cr->rectangle(2, 0, pb->get_width()+2, pb->get_height());
	cr->fill();
	pb = render_icon_pixbuf (Gtk::Stock::INDEX, Gtk::IconSize (Gtk::ICON_SIZE_LARGE_TOOLBAR));
	Gdk::Cairo::set_source_pixbuf(cr, pb, 26, 2);
	cr->rectangle(26, 2, pb->get_width()+2, pb->get_height());
	cr->fill();
#endif

#if 1
	Glib::RefPtr<Gdk::Pixbuf> pb2;

	draw_rect (cr, 52, 0, 30, 48,    1, 1, 1, 1);
	draw_box  (cr, 52, 0, 30, 48, 2, 0, 0, 0, 0.3);

	pb2 = Gdk::Pixbuf::create_from_file ("icons/table.png");
	Gdk::Cairo::set_source_pixbuf(cr, pb2, 56, 1);
	cr->rectangle(56, 1, pb2->get_width(), pb2->get_height());
	cr->fill();

	pb2 = Gdk::Pixbuf::create_from_file ("icons/shield.png");
	Gdk::Cairo::set_source_pixbuf(cr, pb2, 55, 24);
	cr->rectangle(55, 24, pb2->get_width(), pb2->get_height());
	cr->fill();
	//return true;
#endif

#if 0
	Glib::RefPtr<Gdk::Pixbuf> pb3;

	draw_rect (cr, 84, 0, 30, 48,    1, 1, 1, 1);
	draw_box  (cr, 84, 0, 30, 48, 2, 0, 0, 0, 0.3);

	pb3 = Gdk::Pixbuf::create_from_file ("icons/table.png");
	Gdk::Cairo::set_source_pixbuf(cr, pb3, 88, 1);
	cr->rectangle(88, 1, pb3->get_width(), pb3->get_height());
	cr->fill();

	//return true;
#endif

	if (m_c->children.size() > 0) {
		int children = m_c->children.size();
		//printf ("%d children\n", children);
		//RAR should only be one child
		//printf ("child is a %s\n", m_c->children[0]->type.c_str());

		for (int c = 0; c < children; c++) {
			draw_container (cr, 84, c*50, width-84, 47, m_c->children[c]);
		}
		if (!mouse_close) {	//RAR just clip the name to the size of the block image
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

#if 0
	// move to main window, add accessor function
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

#if 0	// lines
	cr->set_antialias (Cairo::ANTIALIAS_NONE);
	cr->set_source_rgba (0, 0, 0, 0.3);

	for (int i = 0; i < width; i += 50) {
		if ((i % 100) == 0) {
			cr->set_line_width(2);
		} else {
			cr->set_line_width(1);
		}

		cr->move_to (i, 0);
		cr->line_to (i, height);
		cr->stroke();
	}
#endif	// lines
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
	//unsigned int children = c->children.size();

	if (m_c->device == "/dev/sdc")
		set_size_request (400, 150);
	else
		set_size_request (400, 25);
	//set_size_request (400, 50 * children);
}

