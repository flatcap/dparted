#include <iostream>

#include <stdio.h>
#include <gtkmm.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "drawingarea.h"
#include "container.h"
#include "block.h"
#include "table.h"
#include "partition.h"
#include "utils.h"
#include "filesystem.h"
#include "theme.h"
#include "segment.h"
#include "whole.h"

const double ARC_N = 3*M_PI_2;		// Compass points in radians
const double ARC_E = 0;
const double ARC_S = M_PI_2;
const double ARC_W = M_PI;

const int    GAP   = 3;			// Space between partitions

/**
 * DPDrawingArea
 */
DPDrawingArea::DPDrawingArea() :
	//Glib::ObjectBase ("MyDrawingArea"),
	m_c (NULL),
	theme (NULL),
	sel_x (-1),
	sel_y (-1),
	mouse_close (false)
{
	set_size_request (800, 77);
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
	//std::cout << "GType name: " << G_OBJECT_TYPE_NAME(gobj()) << "\n";

	theme = new Theme();
}

/**
 * ~DPDrawingArea
 */
DPDrawingArea::~DPDrawingArea()
{
	delete theme;
}


/**
 * on_timeout
 */
bool DPDrawingArea::on_timeout(int timer_number)
{
	std::cout << "timer" << "\n";
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
 * draw_icon
 */
void DPDrawingArea::draw_icon (const Cairo::RefPtr<Cairo::Context>& cr, const std::string &name, int x, int y)
{
	Glib::RefPtr<Gdk::Pixbuf> pb;

	pb = theme->get_icon (name);
	if (!pb) {
		return;
	}

	Gdk::Cairo::set_source_pixbuf(cr, pb, x, y);
	cr->rectangle(x, y, pb->get_width(), pb->get_height());
	cr->fill();
}

/**
 * draw_border
 */
void DPDrawingArea::draw_border (const Cairo::RefPtr<Cairo::Context>& cr, int x, int y, int w, int h, int r)
{
#if 0
	printf ("border: %d, %d, %d, %d\n", x, y, w, h);
	cr->set_line_width (1);
	cr->save();
	cr->set_source_rgb (0.0, 0.5, 0.0);
	cr->move_to (x, 0);
	cr->rel_line_to (0, 100);
	cr->stroke();
	cr->move_to (x+w-1, 0);
	cr->rel_line_to (0, 100);
	cr->stroke();
	cr->restore();
#endif
	cr->move_to (x, y+8);
	cr->arc (x+  r, y+  r, r, ARC_W, ARC_N);
	cr->arc (x+w-r, y+  r, r, ARC_N, ARC_E);
	cr->arc (x+w-r, y+h-r, r, ARC_E, ARC_S);
	cr->arc (x+  r, y+h-r, r, ARC_S, ARC_W);
	cr->close_path();
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
void DPDrawingArea::draw_container (const Cairo::RefPtr<Cairo::Context>& cr, int x, int y, int w, int h, DPContainer *c)
{
	w -= GAP;

	//std::cout << c << "\n";
	//std::string s = get_size (c->bytes_size);
	//printf ("container size: %lld (%s)\n", c->bytes_size, s.c_str());
	//assert (w > 0)
	long bytes_per_pixel = c->bytes_size / w;
	//printf ("width = %d, bytes_per_pixel = %ld\n", w, bytes_per_pixel);

	int num = c->children.size();
	for (int i = 0; i < num; i++) {
		DPContainer *child = c->children[i];
		if (child->is_a ("partition")) {
			printf ("partition\n");
		} else if (child->is_a ("filesystem")) {
			printf ("filesystem\n");
		} else if (child->is_a ("table")) {
			printf ("table\n");
		} else {
			printf ("CHILD is a %s\n", child->name.c_str());
		}

		std::string label1;
		std::string label2;

		size_t pos = child->device.find_last_of ('/');
		if (pos == std::string::npos) {
			label1 = child->device;
			if (label1.empty())
				label1 = "<none>";	//RAR tmp
		} else {
			label1 = child->device.substr (pos+1);
		}
		label2 = get_size (child->bytes_size);

		int child_width = (child->bytes_size / bytes_per_pixel);
		int child_usage = (child->bytes_used / bytes_per_pixel);

		if ((child->children.size() == 1) && child->children[0]->is_a ("filesystem")) {
			if (child->children.size() == 0)
				return;
			Filesystem *fs = dynamic_cast<Filesystem*> (child->children[0]);
			child_usage = fs->bytes_used / bytes_per_pixel;

			if (!fs->label.empty()) {
				label1 += " \"" + fs->label + "\"";
			}
			label2 = get_size (fs->bytes_size);
		}
#if 1
		printf ("child bytes = %lld, child used = %lld\n", child->bytes_size, child->bytes_used);
		printf ("child width = %d, child usage = %d\n", child_width, child_usage);
		printf ("label1: %s\n", label1.c_str());
		printf ("label2: %s\n", label2.c_str());
#endif
		int offset = x + (child->parent_offset / bytes_per_pixel);
		if (child->is_a ("table")) {
			Gdk::RGBA colour = theme->get_colour ("extended");
			int h2 = h;
			int y2 = y;
			draw_tabframe  (cr, offset, y2, child_width, h2, colour);

			draw_icon (cr, "table", offset-24, 8);

			draw_container (cr, offset, y2, child_width, h2, child);
		} else if (child->is_a ("filesystem")) {
			Filesystem *fs = dynamic_cast<Filesystem*> (child);
			if (!fs)
				return;
			Gdk::RGBA colour = theme->get_colour (fs->name);
			draw_partition (cr, offset, y, child_width-GAP, h, child_width-GAP, child_usage, colour);

			Pango::FontDescription font;
			font.set_family ("Liberation Sans");

			Glib::RefPtr<Pango::Layout> layout = Pango::Layout::create(cr);
			layout->set_font_description (font);

			layout->set_text (label1 + "\n" + label2);
			cr->set_source_rgb (0.0, 0.0, 0.0);
			cr->move_to (offset - child_width + GAP + 4, y + 12);
			layout->set_width (Pango::SCALE * (child_width - GAP - 8));
			layout->set_ellipsize (Pango::ELLIPSIZE_END);
			layout->update_from_cairo_context (cr);
			layout->show_in_cairo_context (cr);
		} else {
			if (child->children.size() == 0)
				return;
			Filesystem *fs = dynamic_cast<Filesystem*> (child->children[0]);
			if (!fs)
				return;
			Gdk::RGBA colour = theme->get_colour (fs->name);
			draw_partition (cr, offset, y, child_width-GAP, h, child_width-GAP, child_usage, colour);

			Pango::FontDescription font;
			font.set_family ("Liberation Sans");

			Glib::RefPtr<Pango::Layout> layout = Pango::Layout::create(cr);
			layout->set_font_description (font);

			layout->set_text (label1 + "\n" + label2);
			cr->set_source_rgb (0.0, 0.0, 0.0);
			cr->move_to (offset - child_width + GAP + 4, y + 12);
			layout->set_width (Pango::SCALE * (child_width - GAP - 8));
			layout->set_ellipsize (Pango::ELLIPSIZE_END);
			layout->update_from_cairo_context (cr);
			layout->show_in_cairo_context (cr);
		}
	}
}

/**
 * write_label
 */
void DPDrawingArea::write_label (const Cairo::RefPtr<Cairo::Context>& cr, const Glib::ustring &text, long size)
{
	//std::cout << text << "\n";

	Glib::RefPtr<Pango::Layout> layout = Pango::Layout::create(cr);

	Pango::FontDescription font;
	font.set_family ("Liberation Sans");		//XXX from theme
	font.set_size (12 * Pango::SCALE);

	int w = 0;
	int h = 0;

	layout->set_font_description (font);
	layout->set_markup(text);

	layout->get_pixel_size(w, h);

	int radius = 2 + (h/2);
	//int bar    = w - (h/2);

	//std::cout << "text   = " << w << "," << h << "\n";
	//std::cout << "radius = " << radius << "\n";
	//std::cout << "bar    = " << bar << "\n";

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
	int y = 44;

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

	font.set_size (8 * Pango::SCALE);
	layout->set_font_description (font);
	std::string s = get_size (size);
	//layout->set_markup("<b>" + s + "</b>");
	layout->set_markup(s);
	cr->move_to (x, y+17);
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

	vRange.clear();

#if 1
	Gtk::Allocation allocation = get_allocation();
	int width  = allocation.get_width();
	int height = allocation.get_height();
	//printf ("allocation = %dx%d\n", width, height);
#endif

#if 0
	cr->save();
	cr->set_antialias (Cairo::ANTIALIAS_NONE);
	cr->set_source_rgb (1, 0, 0);
	cr->set_line_width (1);
	cr->rectangle (0.5, 0.5, width-1, height-1);
	cr->stroke();
	cr->restore();
#endif

	//std::cout << m_c << "\n";

#if 1
	std::string name;
	if      (m_c->is_a ("loop")) name = "loop";
	else if (m_c->is_a ("disk")) name = "disk";
	else if (m_c->is_a ("file")) name = "file";
	else                         name = "network";

	draw_icon (cr, name, 2, 0);

	std::string label;
	size_t pos = m_c->device.find_last_of ('/');
	if (pos == std::string::npos) {
		label = m_c->device;
	} else {
		label = m_c->device.substr (pos+1);
	}
	label = "<b>" + label + "</b>";
	write_label (cr, label, m_c->bytes_size);
#endif

	//if (m_c->is_a ("loop")) return true;
	//if (m_c->device != "/dev/sdc") return true;

	int x = 52;
	int y = 1;
	int w = width - x;
	int h = height - 3;

	Block *block = dynamic_cast<Block*> (m_c);

	// block, empty
	if (block->children.size() == 0) {
		Gdk::RGBA colour ("dark grey");
		cr->set_source_rgb (colour.get_red(), colour.get_green(), colour.get_blue());
		draw_border (cr, x, y, w, h, 8);
		cr->stroke();

		Pango::FontDescription font;
		font.set_family ("Liberation Sans");

		Glib::RefPtr<Pango::Layout> layout = Pango::Layout::create(cr);
		layout->set_font_description (font);

		layout->set_markup ("<b>Unallocated</b>");
		cr->set_source_rgb (0.0, 0.0, 0.0);
		cr->move_to (x+6, y+6);
		//layout->set_width (Pango::SCALE * (child_width - GAP - 8));
		//layout->set_ellipsize (Pango::ELLIPSIZE_END);
		layout->update_from_cairo_context (cr);
		layout->show_in_cairo_context (cr);
		return true;
	}

	//ASSERT (block->children.size() == 1)	//RAR
	//std::cout << "block->children = " << block->children.size() << "\n";

	//std::cout << "child = " << m_c->children[0] << "\n";

	if (m_c->children[0]->is_a ("filesystem")) {
		draw_container (cr, x, y, w, h, m_c);
		return true;
	}

	if (m_c->children[0]->is_a ("segment")) {
		// hopefully a volume segment
		Segment *seg = dynamic_cast<Segment*> (m_c->children[0]);
		Whole *whole = seg->whole;
		std::cout << whole << std::endl;
		return true;
	}

	if (!m_c->children[0]->is_a ("table")) {
		std::cout << "not a table" << std::endl;
		return true;
	}

	// block, table, empty
	Table *table = dynamic_cast<Table*> (m_c->children[0]);
	if (table->children.size() == 0) {
		Gdk::RGBA colour = theme->get_colour ("table");
		int w2 = 28;
		draw_partition (cr, x, y, w2, h, w2, 0, colour);

		x = x - w2 + 2;
		y += 8;
		w -= w2;

		draw_icon (cr, "table", x, y);

		x = x + w2 - 2;
		y -= 8;

		colour = theme->get_colour ("unallocated");
		draw_frame (cr, x, y, w, h, colour);
		return true;
	}

	if ((table->children.size() == 1) &&
	    (table->children[0]->is_a ("partition"))) {
		Partition *p = dynamic_cast<Partition*> (table->children[0]);
		//std::cout << "ptype = " << p->ptype << "\n";
		if ((p->ptype == 0xEE) || (p->ptype == 0x42)) {
			// block, table, partition (protective)
			//	partition.size == table.size (delta)
			//std::cout << "protective partition\n";

			Gdk::RGBA colour = theme->get_colour ("table");
			int w2 = 28;
			draw_partition (cr, x, y, w2, h, w2, 0, colour);

			x = x - w2 + 2;
			y += 8;
			w -= w2;

			draw_icon (cr, "shield", x,    y);
			draw_icon (cr, "table",  x+26, y);

			x = x + w2 - 2;
			y -= 8;

			colour.set ("grey");
			draw_frame (cr, x, y, w, h, colour);
			return true;
		}
	}

	if ((table->children.size() != 0) &&
	    (table->children[0]->is_a ("partition"))) {
		//Partition *p = dynamic_cast<Partition*> (table->children[0]);
		//std::cout << "ptype = " << p->ptype << "\n";

		// block, table, partition
		Gdk::RGBA colour = theme->get_colour ("table");
		int w2 = 28;
		//printf ("%d, %d, %d, %d\n", x, y, w2, h);
		draw_partition (cr, x, y, w2, h, w2, 0, colour);

		x = x - w2 + 2;
		y += 8;
		w -= w2;

		draw_icon (cr, "table", x, y);

		x = x + w2 - 2 + GAP;
		y -= 8;

		draw_container (cr, x, y, w, h, table);
		return true;
	}

#if 0
	// move to main window, add accessor function
	Glib::RefPtr<Gtk::StyleContext> style;
	style = get_style_context();

	Gdk::RGBA fg;
	Gdk::RGBA bg;

	fg = style->get_color (Gtk::STATE_FLAG_NORMAL);
	bg = style->get_background_color(Gtk::STATE_FLAG_NORMAL);

	std::cout << "fg = " << fg.get_red() << "," << fg.get_green() << "," << fg.get_blue() << "\n";
	std::cout << "bg = " << bg.get_red() << "," << bg.get_green() << "," << bg.get_blue() << "\n";
#endif

	return true;
}


/**
 * on_mouse_motion
 */
bool DPDrawingArea::on_mouse_motion (GdkEventMotion *event)
{
	//std::cout << "mouse motion: (" << event->x << "," << event->y << ")\n";

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
	//std::cout << "mouse click: (" << event->x << "," << event->y << ")\n";

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

	//get_window()->invalidate (false); //RAR everything for now

#if 0
	if ((event->x >= 100) && (event->x < 200) &&
	    (event->y >= 50)  && (event->y < 150)) {
	}
#endif
#if 0
	std::cout << event->type << "\n";
	std::cout << event->state << "\n";
	std::cout << event->button << "\n";
	std::cout << event->time << "\n";
	std::cout << event->x << "\n";
	std::cout << event->y << "\n";

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
	//set_size_request (400, 50 * children);
}

