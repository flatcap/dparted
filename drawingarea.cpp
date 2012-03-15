#include <iostream>

#include <stdio.h>
#include <gtkmm.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "drawingarea.h"
#include "container.h"

const double ARC_N = 3*M_PI_2;		// Compass points in radians
const double ARC_E = 0;
const double ARC_S = M_PI_2;
const double ARC_W = M_PI;

/**
 * DPDrawingArea
 */
DPDrawingArea::DPDrawingArea() :
	//Glib::ObjectBase ("MyDrawingArea"),
	sel_x (-1),
	sel_y (-1),
	mouse_close (false)
{
	set_size_request (800, 50);
	set_hexpand (true);
	set_vexpand (false);

	add_events (Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::LEAVE_NOTIFY_MASK);

	signal_button_press_event() .connect (sigc::mem_fun (*this, &DPDrawingArea::on_mouse_click));
	signal_motion_notify_event().connect (sigc::mem_fun (*this, &DPDrawingArea::on_mouse_motion));
	signal_leave_notify_event() .connect (sigc::mem_fun (*this, &DPDrawingArea::on_mouse_leave));

#if 0
	sigc::slot<bool> my_slot = sigc::bind (sigc::mem_fun (*this, &DPDrawingArea::on_timeout), 0);
	sigc::connection conn = Glib::signal_timeout().connect(my_slot, 800); // ms
#endif
	//std::cout << "GType name: " << G_OBJECT_TYPE_NAME(gobj()) << std::endl;
}

/**
 * ~DPDrawingArea
 */
DPDrawingArea::~DPDrawingArea()
{
}


/**
 * on_timeout
 */
bool DPDrawingArea::on_timeout(int timer_number)
{
	std::cout << "timer" << std::endl;
	get_window()->invalidate (false); //RAR everything for now
	//return (m_c->device == "/dev/sdc");
	return false;
}

/**
 * draw_rect
 */
void DPDrawingArea::draw_rect (const Cairo::RefPtr<Cairo::Context>& cr, double x, double y, double width, double height, double red, double green, double blue)
{
	cr->set_source_rgb (red, green, blue);
	cr->rectangle (x, y, width, height);
	cr->fill();
}

/**
 * draw_box
 */
void DPDrawingArea::draw_box (const Cairo::RefPtr<Cairo::Context>& cr, double x, double y, double width, double height, double line_width, double red, double green, double blue)
{
	cr->set_line_width(line_width);
	cr->set_source_rgb (red, green, blue);

	double half = line_width/2;

	//Cairo::Antialias a = cr->get_antialias();
	//cr->set_antialias (Cairo::ANTIALIAS_NONE);

	//cr->set_line_cap (Cairo::LINE_CAP_ROUND);
	//cr->set_line_cap (Cairo::LINE_CAP_SQUARE);
	cr->set_line_join (Cairo::LINE_JOIN_ROUND);

	cr->move_to (x + half,         y + half);
	cr->line_to (x + width - half, y + half);
	cr->line_to (x + width - half, y + height - half);
	cr->line_to (x + half,         y + height - half);
	cr->close_path();

	cr->stroke();
	//cr->set_antialias (a);
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
 * get_focus
 */
bool DPDrawingArea::get_focus (int &x, int &y, int &w, int &h)
{
	if ((sel_x < 0) || (sel_y < 0))
		return false;

	bool b = false;
	std::deque<Range>::iterator it;
	for (it = vRange.begin(); it != vRange.end(); it++) {
		b = ((sel_x >= (*it).x) && (sel_x < ((*it).x + (*it).w)) &&
		     (sel_y >= (*it).y) && (sel_y < ((*it).y + (*it).h)));
		if (b) {
			x = (*it).x;
			y = (*it).y;
			w = (*it).w;
			h = (*it).h;
			break;
		}
	}

	return b;
}


/**
 * draw_border
 */
void DPDrawingArea::draw_border (const Cairo::RefPtr<Cairo::Context>& cr, int x, int y, int w, int h, int r)
{
	cr->arc (x+  r, y+  r, r, ARC_W, ARC_N);
	cr->arc (x+w-r, y+  r, r, ARC_N, ARC_E);
	cr->arc (x+w-r, y+h-r, r, ARC_E, ARC_S);
	cr->arc (x+  r, y+h-r, r, ARC_S, ARC_W);
}

/**
 * draw_focus
 */
void DPDrawingArea::draw_focus (const Cairo::RefPtr<Cairo::Context>& cr, int x, int y, int w, int h)
{
	const int r = 8;					// Radius

	std::vector<double> dashes;
	dashes.push_back (5);
	dashes.push_back (5);

	cr->save();
	draw_border (cr, x, y, w, h, r);			// Set clipping area
	cr->clip();

	cr->set_line_width (4);

	cr->set_dash (dashes, 0);
	cr->set_source_rgb (0, 0, 0);
	draw_border (cr, x, y, w, h, r);
	cr->close_path();
	cr->stroke();

	cr->set_dash (dashes, 5);
	cr->set_source_rgb (1, 1, 1);
	draw_border (cr, x, y, w, h, r);
	cr->close_path();
	cr->stroke();

	cr->restore();						// End clipping
}

/**
 * draw_frame
 */
void DPDrawingArea::draw_frame (const Cairo::RefPtr<Cairo::Context>& cr, int &x, int &y, int &w, int &h, const Gdk::RGBA &colour)
{
	const int r = 8;					// Radius

	cr->save();
	draw_border (cr, x, y, w, h, r);			// Set clipping area
	cr->clip();

	cr->set_source_rgb (colour.get_red(), colour.get_green(), colour.get_blue());

	cr->set_line_width (r);					// Thick top bar
	cr->move_to (x, y+(r/2));
	cr->rel_line_to (w, 0);
	cr->stroke();

	cr->set_line_width (2);					// Thin side bars
	cr->move_to (x+1, y+r);
	cr->rel_line_to (0, h-(2*r));
	cr->move_to (x+w-1, y+r);
	cr->rel_line_to (0, h-(2*r));
	cr->stroke();

	cr->set_line_width (4);					// Thin bottom bar
	cr->arc (x+w-r, y+h-r, r, ARC_E, ARC_S);
	cr->arc (x+  r, y+h-r, r, ARC_S, ARC_W);
	cr->stroke();

	cr->restore();						// End clipping

	Range rg = { x, y, w, h, NULL };
	vRange.push_front (rg);

	x += 2;							// The space remaining inside
	y += r;
	w -= 4;
	h -= r+2;
}

/**
 * draw_tabframe
 */
void DPDrawingArea::draw_tabframe (const Cairo::RefPtr<Cairo::Context>& cr, int &x, int &y, int &w, int &h, const Gdk::RGBA &colour)
{
	const int r = 8;					// Radius
	const int t = 26;					// Tab width

	cr->save();
	draw_border (cr, x, y, w, h, r);			// Set clipping area
	cr->clip();

	cr->set_source_rgb (colour.get_red(), colour.get_green(), colour.get_blue());

	cr->set_line_width (r);					// Thick top bar
	cr->move_to (x, y+4);
	cr->rel_line_to (w, 0);
	cr->stroke();

	cr->set_line_width (t);					// Thick side bar
	cr->move_to (x+(t/2), y+r);
	cr->rel_line_to (0, h-r);
	cr->stroke();

	cr->move_to (x+t, y+r);					// Curvy inner corners
	cr->arc (x+t+r, y+(2*r), r, ARC_W, ARC_N);
	cr->fill();
	cr->move_to (x+t, y+h-2);
	cr->arc (x+t+r, y+h-10, r, ARC_S, ARC_W);
	cr->fill();

	cr->set_line_width (2);					// Thin side bar
	cr->move_to (x+w-1, y+r);
	cr->rel_line_to (0, h-16);
	cr->stroke();

	cr->set_line_width (4);					// Thin bottom bar
	cr->arc (x+w-r, y+h-r, r, ARC_E, ARC_S);
	cr->arc (x+  r, y+h-r, r, ARC_S, ARC_W);
	cr->stroke();

	cr->restore();						// End clipping

	Range rg = { x, y, w, h, NULL };
	vRange.push_front (rg);

	x += t;							// The space remaining inside
	y += r;
	w -= t+2;
	h -= r+2;
}

/**
 * draw_partition
 */
void DPDrawingArea::draw_partition (const Cairo::RefPtr<Cairo::Context>& cr,
				    int &x, int y, int w, int h,
				    int width_fs, int width_usage,
				    const Gdk::RGBA &colour)
{
	const int r = 8;						// Radius

	cr->save();
	draw_border (cr, x, y, w, h, r);				// Set clipping area
	cr->clip();

	draw_rect (cr, x, y+r, width_usage, h-r, 0.96, 0.96, 0.72);	 // Yellow usage
	draw_rect (cr, x+width_usage, y+r,  width_fs-width_usage, h-r, 1.00, 1.00, 1.00);	// White background

	if ((width_fs - width_usage) > 1) {
		cr->set_source_rgb (1, 1, 1);
		cr->move_to (x+width_usage, y+r);			// Curvy corners
		cr->arc (x+width_usage-r, y+(2*r), r, ARC_N, ARC_E);
		cr->fill();
		cr->move_to (x+width_usage, y+h-2);
		cr->arc (x+width_usage-r, y+h-2-r, r, ARC_E, ARC_S);
		cr->fill();
	}

	Cairo::RefPtr<Cairo::LinearGradient> grad;			// Gradient shading
	grad = Cairo::LinearGradient::create (0.0, 0.0, 0.0, h);
	grad->add_color_stop_rgba (0.00, 0.0, 0.0, 0.0, 0.2);
	grad->add_color_stop_rgba (0.50, 0.0, 0.0, 0.0, 0.0);
	grad->add_color_stop_rgba (1.00, 0.0, 0.0, 0.0, 0.1);
	cr->set_source (grad);
	cr->rectangle (x, y, w, h);
	cr->fill();

	cr->set_source_rgb (colour.get_red(), colour.get_green(), colour.get_blue());

	cr->set_line_width (r);						// Thick top bar
	cr->move_to (x, y+4);
	cr->rel_line_to (w, 0);
	cr->stroke();

	cr->set_line_width (2);						// Thin side bars
	cr->move_to (x+1, y+r);
	cr->rel_line_to (0, h-(2*r));
	cr->move_to (x+w-1, y+r);
	cr->rel_line_to (0, h-(2*r));
	cr->stroke();

	cr->set_line_width (4);						// Thin bottom bar
	cr->arc (x+w-r, y+h-r, r, ARC_E, ARC_S);
	cr->arc (x+  r, y+h-r, r, ARC_S, ARC_W);
	cr->stroke();

	cr->restore();							// End clipping

	Range rg = { x, y, w, h, NULL };
	vRange.push_front (rg);

	x += w;								// The space remaining inside
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
			draw_rect (cr, x, y, 28, height, red, green, blue);
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
			draw_rect  (cr, x, y, width, height, 1, 1, 1);		// White background
#if 1
			int bytes_per_pixel = c->bytes_size / width;
			int uw = c->bytes_used / bytes_per_pixel;
			draw_rect  (cr, x, y, uw, height, 0.96, 0.96, 0.72);	 // Yellow usage

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
		draw_box  (cr, x, y, width, height, 3, red, green, blue);
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

	vRange.clear();

#if 0
	Gtk::Allocation allocation = get_allocation();
	int width  = allocation.get_width();
	int height = allocation.get_height();
	printf ("allocation = %dx%d\n", width, height);
#endif

	static int i = 0;
	i++;
	if (i < 2)
		return true;

	std::cout << m_c << std::endl;

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

	// block, empty
	//	children.size() == 0
	// block, table, empty
	//	children.size() == 1
	//	children[0] == table
	//	table.children.size() == 0
	// block, table, partition (protective)
	//	children.size() == 1
	//	children[0] == table
	//	table.children.size() == 1
	//	table.children[1] == partition
	//	partition.size == table.size (delta)

	std::cout << m_c->children[0] << std::endl;

#if 0
	int x = 50;
	int y = 0;
	int w = 400;
	int h = 77;	//47;
	//DPContainer *c = NULL;
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

	draw_partition (cr, x, y, w, h, width_fs, width_usage, colour);

	name = "extended";
	get_colour (name, red, green, blue);
	colour.set_rgba (red, green, blue);
	w = 800;
	width_usage = 85*w/100;

	draw_tabframe (cr, x, y, w, h, colour);

	const char *file = "icons/table.png";

	Glib::RefPtr<Gdk::Pixbuf> pb;
	pb = Gdk::Pixbuf::create_from_file (file);
	Gdk::Cairo::set_source_pixbuf(cr, pb, x-24, y+2);
	cr->rectangle(x-24, y+2, pb->get_width(), pb->get_height());
	cr->fill();

	name = "red";
	get_colour (name, red, green, blue);
	colour.set_rgba (red, green, blue);
	w = 250;
	width_usage = 15*w/100;

	draw_partition (cr, x, y, w, h, width_fs, width_usage, colour);

	name = "green";
	get_colour (name, red, green, blue);
	colour.set_rgba (red, green, blue);
	w = 350;
	width_usage = 15*w/100;

	draw_frame (cr, x, y, w, h, colour);

	int fx, fy, fh, fw;
	if (get_focus (fx, fy, fw, fh)) {
		draw_focus (cr, fx, fy, fw, fh);
	}

	return true;
#endif

#if 0
	Glib::RefPtr<Gdk::Pixbuf> pb;
	pb = render_icon_pixbuf (Gtk::Stock::INDEX, Gtk::IconSize (Gtk::ICON_SIZE_LARGE_TOOLBAR));
	Gdk::Cairo::set_source_pixbuf(cr, pb, 26, 2);
	cr->rectangle(26, 2, pb->get_width()+2, pb->get_height());
	cr->fill();
#endif

#if 0
	Glib::RefPtr<Gdk::Pixbuf> pb2;

	draw_rect (cr, 52, 0, 30, 48,    1, 1, 1);
	draw_box  (cr, 52, 0, 30, 48, 2, 0, 0, 0);

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
#endif

#if 0
	// move to main window, add accessor function
	Glib::RefPtr<Gtk::StyleContext> style;
	style = get_style_context();

	Gdk::RGBA fg;
	Gdk::RGBA bg;

	fg = style->get_color (Gtk::STATE_FLAG_NORMAL);
	bg = style->get_background_color(Gtk::STATE_FLAG_NORMAL);

	std::cout << "fg = " << fg.get_red() << "," << fg.get_green() << "," << fg.get_blue() << std::endl;
	std::cout << "bg = " << bg.get_red() << "," << bg.get_green() << "," << bg.get_blue() << std::endl;
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
	draw_rect (cr, (width-stringWidth)/2, (height-stringHeight)/2, stringWidth, stringHeight, 1, 1);
	draw_box  (cr, (width-stringWidth)/2-1, (height-stringHeight)/2-1, stringWidth+2, stringHeight+2, 1, 0, 0, 0);
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

#if 0
	bool old = mouse_close;

	mouse_close = ((event->y > 25) and (event->x < 90));

	if (mouse_close != old) {
		get_window()->invalidate (false); //RAR everything for now
	}
#endif

	return true;
}

/**
 * on_mouse_leave
 */
bool DPDrawingArea::on_mouse_leave (GdkEventCrossing *event)
{
#if 0
	if (mouse_close) {
		mouse_close = false;
		get_window()->invalidate (false); //RAR everything for now
	}
#endif
	return true;
}

/**
 * on_mouse_click
 */
bool DPDrawingArea::on_mouse_click (GdkEventButton *event)
{
	std::cout << "mouse click: (" << event->x << "," << event->y << ")" << std::endl;

	sel_x = event->x;
	sel_y = event->y;

#if 0
	std::deque<Range>::iterator it;
	for (it = vRange.begin(); it != vRange.end(); it++) {
		bool b = ((event->x >= (*it).x) && (event->x < ((*it).x + (*it).w)) &&
			  (event->y >= (*it).y) && (event->y < ((*it).y + (*it).h)));
		if (b) printf ("\e[31m");
		printf ("Range: %d,%d %d,%d %p\n", (*it).x, (*it).y, (*it).w, (*it).h, (*it).p);
		if (b) printf ("\e[0m");
	}
	printf ("\n");
#endif

	get_window()->invalidate (false); //RAR everything for now

#if 0
	if ((event->x >= 100) && (event->x < 200) &&
	    (event->y >= 50)  && (event->y < 150)) {
	}
#endif
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
		set_size_request (800, 150);
	else
		set_size_request (800, 25);
	//set_size_request (400, 50 * children);
}

