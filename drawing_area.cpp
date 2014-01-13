/* Copyright (c) 2014 Richard Russon (FlatCap)
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Library General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <unistd.h>
#include <gtkmm.h>
#include <pangomm.h>
#include <stdlib.h>
#include <gdk/gdkwindow.h>

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include "drawing_area.h"
#include "log.h"
#include "utils.h"
#include "log_trace.h"
#include "gui_app.h"
#include "dparted.h"
#include "table.h"

const double ARC_N = 3*M_PI_2;	// Compass points in radians
const double ARC_E = 0*M_PI_2;
const double ARC_S = 1*M_PI_2;
const double ARC_W = 2*M_PI_2;

const int GAP         =  3;	// Space between partitions
const int RADIUS      =  8;	// Curve radius of corners
const int SIDES       =  2;	// Width of sides and base
const int TAB_WIDTH   = 10;	// Space in left side-bar
const int BLOCK_WIDTH = 24;	// Placeholder for icons

/**
 * DrawingArea
 */
DrawingArea::DrawingArea()
	//Glib::ObjectBase ("MyDrawingArea")
{
	//set_size_request (800, 77);
	set_size_request (400, 70*3);
	set_hexpand (true);
	set_vexpand (false);
	set_can_focus (true);

	add_events (Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::LEAVE_NOTIFY_MASK);

	signal_button_press_event() .connect (sigc::mem_fun (*this, &DrawingArea::on_mouse_click));
	signal_motion_notify_event().connect (sigc::mem_fun (*this, &DrawingArea::on_mouse_motion));
	signal_leave_notify_event() .connect (sigc::mem_fun (*this, &DrawingArea::on_mouse_leave));

	signal_focus_in_event().connect (sigc::mem_fun (*this, &DrawingArea::on_focus_in));
	signal_focus_out_event().connect (sigc::mem_fun (*this, &DrawingArea::on_focus_out));

#if 0
	sigc::slot<bool> my_slot = sigc::bind (sigc::mem_fun (*this, &DrawingArea::on_timeout), 0);
	sigc::connection conn = Glib::signal_timeout().connect (my_slot, 300); // ms
#endif

	//set_tooltip_text("tooltip number 1");

	set_has_tooltip();	// We'll be handling the tooltips ourself
	signal_query_tooltip().connect(sigc::mem_fun(*this, &DrawingArea::on_textview_query_tooltip));

	make_menu();
}

/**
 * ~DrawingArea
 */
DrawingArea::~DrawingArea()
{
}


#if 1
/**
 * operator<< - serialise a Rect
 */
std::ostream&
operator<< (std::ostream& stream, const Rect& r)
{
	stream << "[" << r.x << "," << r.y << "," << r.w << "," << r.h << "]";

	return stream;
}

/**
 * set_colour
 */
void set_colour (const Cairo::RefPtr<Cairo::Context>& cr, const Gdk::RGBA& rgba)
{
	if (!cr)
		return;
	cr->set_source_rgba (rgba.get_red(), rgba.get_green(), rgba.get_blue(), rgba.get_alpha());
}

/**
 * draw_edge - 1px rectangular border
 */
void
draw_edge (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape, const Gdk::RGBA& colour)
{
	cr->save();
	set_colour (cr, colour);
	cr->set_line_width (1);

	cr->rectangle (shape.x+0.5, shape.y+0.5, shape.w-1, shape.h-1);

	cr->stroke();
	cr->restore();
}

#endif

#if 0
/**
 * draw_grid - fine mesh grid
 */
void
draw_grid (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape)
{
	int width  = shape.w;
	int height = shape.h;

	cr->save();

	cr->set_antialias (Cairo::ANTIALIAS_NONE);
	cr->set_source_rgba (1.0, 0.0, 0.0, 0.2);
	cr->set_line_width (1);
	cr->rectangle (0.5, 0.5, width-1, height-1);
	cr->stroke();

	for (int i = 10; i < width; i += 10) {
		if ((i % 100) == 0) {
			cr->set_source_rgba (0.0, 0.0, 1.0, 0.2);
		} else {
			cr->set_source_rgba (0.0, 1.0, 0.0, 0.3);
		}
		cr->move_to (i + 0.5, 0);
		cr->rel_line_to (0, height);
		cr->stroke();
	}

	for (int i = 10; i < width; i += 10) {
		if ((i % 100) == 0) {
			cr->set_source_rgba (0.0, 0.0, 1.0, 0.2);
		} else {
			cr->set_source_rgba (0.0, 1.0, 0.0, 0.3);
		}
		cr->move_to (0, i + 0.5);
		cr->rel_line_to (width, 0);
		cr->stroke();
	}

	cr->restore();
}

/**
 * draw_grid_linear - Major and minor grid lines
 */
void
draw_grid_linear (const Cairo::RefPtr<Cairo::Context>& cr, Rect space, long max_size)
{
	space.w -= 2;

	const int& x = space.x;
	const int& w = space.w;
	const int& h = space.h;

	double bytes_per_pixel = max_size / w;

	int lower = floor (log2 (40.0 * max_size / w));
	int major = floor (log2 (256.0 * max_size / w));
	int minor = major - 1;

	major = 1 << (major - lower);
	minor = 1 << (minor - lower);

	cr->save();

	cr->set_antialias (Cairo::ANTIALIAS_NONE);
	cr->set_source_rgba (1.0, 0.0, 0.0, 0.2);
	cr->set_line_width (1);
	cr->rectangle (0.5+x, 0.5, w-1, h-1);
	cr->stroke();

	double spacing = pow (2, lower) / bytes_per_pixel;

	int count = (w / spacing) + 1;

	for (int i = 0; i <= count; i++) {
		if ((i % major) == 0) {
			cr->set_line_width (3);
			cr->set_source_rgba (0.3, 0.3, 0.8, 0.7);
		} else if ((i % minor) == 0) {
			cr->set_line_width (2);
			cr->set_source_rgba (0.5, 0.5, 0.5, 0.6);	//XXX theme
		} else {
			cr->set_line_width (1);
			cr->set_source_rgba (0.3, 0.3, 0.3, 0.5);
		}

		cr->move_to ((int) (spacing*i) + 0.5 + x, 0);
		cr->rel_line_to (0, h);
		cr->stroke();
	}

	cr->restore();
}

/**
 * draw_grid_log
 */
void
draw_grid_log (const Cairo::RefPtr<Cairo::Context>& cr, Rect space, long max_size)
{
}

#endif

/**
 * draw_border - sketch out curved rectangle
 */
void
draw_border (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape)
{
	const int& r = RADIUS;
	const int& x = shape.x;
	const int& y = shape.y;
	const int& w = shape.w;
	const int& h = shape.h;

	cr->move_to (x, y+r);
	cr->arc (x+  r, y+  r, r, ARC_W, ARC_N);
	cr->arc (x+w-r, y+  r, r, ARC_N, ARC_E);
	cr->arc (x+w-r, y+h-r, r, ARC_E, ARC_S);
	cr->arc (x+  r, y+h-r, r, ARC_S, ARC_W);
	cr->close_path();
}

/**
 * fill_area - fill rounded rectangle
 */
void
fill_area (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape, const Gdk::RGBA& colour)
{
	cr->save();
	set_colour (cr, colour);

	draw_border (cr, shape);

	cr->fill();
	cr->restore();
}

/**
 * fill_rect - fill rectangle
 */
void
fill_rect (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape, const Gdk::RGBA& colour)
{
	cr->save();
	set_colour (cr, colour);

	cr->rectangle (shape.x, shape.y, shape.w, shape.h);

	cr->fill();
	cr->restore();
}

/**
 * checker_rect - checker rect
 */
void
checker_rect (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape, int check_size)
{
	const int& s = check_size;
	const int& x = shape.x;
	const int& y = shape.y;
	const int& w = shape.w;
	const int& h = shape.h;

	for (int i = 0; i <= w; i += s) {
		for (int j = 0; j <= h; j += s) {
			if (((i+j)/s)&1)
				cr->set_source_rgba (0.8, 0.8, 0.8, 1.0);
			else
				cr->set_source_rgba (0.6, 0.6, 0.6, 1.0);

			cr->rectangle (x+i, y+j, s, s);
			cr->fill();
		}
	}
}


/**
 * draw_text - write some text into an area
 */
void
draw_text (const Cairo::RefPtr<Cairo::Context>& cr, Rect shape, const std::string& text)
{
	cr->save();
	draw_border (cr, shape);				// Set clipping area
	cr->clip();

	Pango::FontDescription font;
	font.set_family ("Liberation Sans Bold");	//THEME - icon label font

	Glib::RefPtr<Pango::Layout> layout = Pango::Layout::create (cr);
	layout->set_font_description (font);

	font.set_size (11 * Pango::SCALE);		//THEME - icon label size
	layout->set_font_description (font);

	layout->set_text (text);

	int tw = 0;
	int th = 0;
	layout->get_pixel_size (tw, th);

	cr->set_source_rgba (0.0, 0.0, 0.0, 1.0);	//THEME - icon label colour

	//int left = std::max (GAP, (shape.w - tw) / 2);
	//printf ("left = %d\n", left);
	//cr->move_to (shape.x + left, shape.y + shape.h - th);	// align to bottom
	cr->move_to (shape.x + SIDES, shape.y + RADIUS);	// align to top

#if 0
	layout->set_width (Pango::SCALE * (shape.w - 4));
	layout->set_ellipsize (Pango::ELLIPSIZE_END);
#endif
	layout->update_from_cairo_context (cr);
	layout->show_in_cairo_context (cr);

	cr->restore();
}

/**
 * draw_focus - 2px dashed black/white line
 */
void
draw_focus (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape)
{
	static int start = 0;
	std::vector<double> dashes = {5,5};

	cr->save();
	draw_border (cr, shape);				// Set clipping area
	//cr->clip();

	cr->set_line_width (2);

	cr->set_dash (dashes, start);
	cr->set_source_rgba (0.0, 0.0, 0.0, 1.0);		//RAR focus colours from theme
	draw_border (cr, shape);
	cr->close_path();
	cr->stroke();

	cr->set_dash (dashes, start+5);
	cr->set_source_rgba (1.0, 1.0, 1.0, 1.0);
	draw_border (cr, shape);
	cr->close_path();
	cr->stroke();

	cr->restore();						// End clipping
	start++;
}

/**
 * draw_gradient - apply light shading to an area
 */
void
draw_gradient (const Cairo::RefPtr<Cairo::Context>& cr, Rect shape)
{
	const int& x = shape.x;
	const int& y = shape.y;
	const int& w = shape.w;
	const int& h = shape.h;

	//XXX needs clipping to rounded rectangle

	cr->save();
	draw_border (cr, shape);				// Set clipping area
	cr->clip();

	Cairo::RefPtr<Cairo::LinearGradient> grad;			// Gradient shading
	grad = Cairo::LinearGradient::create (0.0, 0.0, 0.0, h);
	grad->add_color_stop_rgba (0.00, 0.0, 0.0, 0.0, 0.2);
	grad->add_color_stop_rgba (0.50, 0.0, 0.0, 0.0, 0.0);
	grad->add_color_stop_rgba (1.00, 0.0, 0.0, 0.0, 0.1);
	cr->set_source (grad);
	cr->rectangle (x, y, w, h);
	cr->fill();
	cr->restore();
}


/**
 * draw_corner - solid ne/nw/se/sw convex/concave corner
 */
void draw_corner (const Cairo::RefPtr<Cairo::Context>& cr, Rect shape, bool north, bool east, bool convex)
{
	const int& r = RADIUS;
	const int& x = shape.x;
	const int& y = shape.y;
	const int& w = shape.w;
	const int& h = shape.h;

	cr->save();
	cr->rectangle (x, y, w, h);
	cr->clip();

	if (north) {
		if (east) {
			if (convex) {
				// North-East concave
				cr->arc (x+w-r, y+r, r, ARC_N, ARC_E);
				cr->rel_line_to (-r, 0);
				cr->rel_line_to (0, r);
			} else {
				// North-East convex
				cr->arc (x+w-r, y+r, r, ARC_N, ARC_E);
				cr->rel_line_to (0, -r);
				cr->rel_line_to (-r, 0);
			}
		} else {
			if (convex) {
				// North-West concave
				cr->arc (x+r, y+r, r, ARC_W, ARC_N);
				cr->rel_line_to (0, r);
				cr->rel_line_to (-r, 0);
			} else {
				// North-West convex
				cr->arc (x+r, y+r, r, ARC_W, ARC_N);
				cr->rel_line_to (-r, 0);
				cr->rel_line_to (0, r);
			}
		}
	} else {
		if (east) {
			if (convex) {
				// South-East concave
				cr->arc (x+w-r, y+h-r, r, ARC_E, ARC_S);
				cr->rel_line_to (0, -r);
				cr->rel_line_to (r, 0);
			} else {
				// South-East convex
				cr->arc (x+w-r, y+h-r, r, ARC_E, ARC_S);
				cr->rel_line_to (r, 0);
				cr->rel_line_to (0, -r);
			}
		} else {
			if (convex) {
				// South-West concave
				cr->arc (x+r, y+h-r, r, ARC_S, ARC_W);
				cr->rel_line_to (r, 0);
				cr->rel_line_to (0, r);
			} else {
				// South-West convex
				cr->arc (x+r, y+h-r, r, ARC_S, ARC_W);
				cr->rel_line_to (0, r);
				cr->rel_line_to (r, 0);
			}
		}
	}

	cr->fill();
	cr->restore();
}

/**
 * draw_arc - draw a corner line (90 degrees) se/sw
 */
void draw_arc (const Cairo::RefPtr<Cairo::Context>& cr, Rect shape, bool east)
{
	const int& r = RADIUS;
	const int& x = shape.x;
	const int& y = shape.y;
	const int& w = shape.w;
	const int& h = shape.h;

	cr->save();
	cr->set_line_width (SIDES+1);

	if (east) {
		// South-East
		cr->move_to (x+w-r, y+h);
		cr->rel_line_to (0, -r);
		cr->rel_line_to (r, 0);
		cr->rel_line_to (0, r);
		cr->close_path();
		cr->clip();

		cr->arc (x+w-r-(SIDES), y+h-r-SIDES, r+(SIDES/2)+1, ARC_E, ARC_S);
	} else {
		// South-West
		cr->move_to (x, y+h);
		cr->rel_line_to (0, -r);
		cr->rel_line_to (r, 0);
		cr->rel_line_to (0, r);
		cr->close_path();
		cr->clip();

		cr->arc (x+r+SIDES, y+h-r-SIDES, r+(SIDES/2)+1, ARC_S, ARC_W);
	}

	cr->stroke();
	cr->restore();
}


/**
 * draw_block - draw an icon-width, hollow, rounded rectangle
 */
void
DrawingArea::draw_block (const Cairo::RefPtr<Cairo::Context>& cr, GfxContainerPtr& cont, const Rect& shape, Rect& tab, Rect& right)
{
	if (shape.h < (RADIUS*2)) {
		log_info ("draw_block: too short\n");
		return;
	}

	if (shape.w < (BLOCK_WIDTH + (RADIUS*2))) {
		log_info ("draw_block: too narrow\n");
		std::cout << cont << std::endl;
		return;
	}

	Rect work = { shape.x, shape.y, BLOCK_WIDTH + (RADIUS/2), shape.h };

	const int& x = work.x;
	const int& y = work.y;
	const int& w = work.w;
	const int& h = work.h;

	draw_corner (cr, work, true,  false, true);		// Top left corner (1)
	draw_corner (cr, work, true,  true,  true);		// Top right corner (3)

	draw_arc (cr, work, true);				// Thin bottom right corner (12)
	draw_arc (cr, work, false);				// Thin bottom left corner (13)

	cr->set_line_width (RADIUS);				// Thick top bar (2)
	cr->move_to (x+RADIUS, y+(RADIUS/2));
	cr->rel_line_to (w-(2*RADIUS), 0);
	cr->stroke();

	cr->set_line_width (SIDES);				// Thin left bar (4)
	cr->move_to (x+(SIDES/2), y+RADIUS);
	cr->rel_line_to (0, h-(2*RADIUS));
	cr->stroke();

	cr->set_line_width (SIDES);				// Thin right bar (8)
	cr->move_to (x+w-(SIDES/2), y+RADIUS);
	cr->rel_line_to (0, h-(2*RADIUS));
	cr->stroke();

	cr->set_line_width (SIDES);				// Thin bottom bar (10)
	cr->move_to (x+RADIUS, y+h-(SIDES/2));
	cr->rel_line_to (w-(2*RADIUS), 0);
	cr->stroke();

	tab = { x+SIDES, y+RADIUS, BLOCK_WIDTH, h-RADIUS-(SIDES*1) };

	draw_corner (cr, tab, true, true,  false);		// Tab inside top right corner
	draw_corner (cr, tab, true, false, false);		// Tab inside top left corner

	right = { shape.x + work.w + GAP, shape.y, shape.w - work.w - GAP, shape.h};

	vRange.push_front ({shape, cont});
}

/**
 * draw_box - draw a rounded rectangle
 */
void
DrawingArea::draw_box (const Cairo::RefPtr<Cairo::Context>& cr, GfxContainerPtr& cont, const Rect& shape, Rect& inside)
{
	if (shape.h < (RADIUS*2)) {
		log_info ("draw_box: too short\n");
		return;
	}

	if (shape.w < (TAB_WIDTH + (RADIUS*2))) {
		log_info ("draw_box: too narrow\n");
		std::cout << cont << std::endl;
		return;
	}

	const int& x = shape.x;
	const int& y = shape.y;
	const int& w = shape.w;
	const int& h = shape.h;

	draw_corner (cr, shape, true,  false, true);		// Top left corner (1)
	draw_corner (cr, shape, true,  true,  true);		// Top right corner (3)

	draw_arc (cr, shape, true);				// Thin bottom right corner (12)
	draw_arc (cr, shape, false);				// Thin bottom left corner (13)

	cr->set_line_width (RADIUS);				// Thick top bar (2)
	cr->move_to (x+RADIUS, y+(RADIUS/2));
	cr->rel_line_to (w-(2*RADIUS), 0);
	cr->stroke();

	cr->set_line_width (SIDES);				// Thin left bar (4)
	cr->move_to (x+(SIDES/2), y+RADIUS);
	cr->rel_line_to (0, h-(2*RADIUS));
	cr->stroke();

	cr->set_line_width (SIDES);				// Thin right bar (8)
	cr->move_to (x+w-(SIDES/2), y+RADIUS);
	cr->rel_line_to (0, h-(2*RADIUS));
	cr->stroke();

	cr->set_line_width (SIDES);				// Thin bottom bar (10)
	cr->move_to (x+RADIUS, y+h-(SIDES/2));
	cr->rel_line_to (w-(2*RADIUS), 0);
	cr->stroke();

	inside = { x+SIDES, y+RADIUS, w-(SIDES*2), h-RADIUS-SIDES };

	draw_corner (cr, inside, true, false, false);		// Top left inner corner (6)
	draw_corner (cr, inside, true, true,  false);		// Top right inner corner (7)

	vRange.push_front ({shape, cont});
}

/**
 * draw_iconbox - draw a rounded rectangle with a handy tab
 */
void
DrawingArea::draw_iconbox (const Cairo::RefPtr<Cairo::Context>& cr, GfxContainerPtr& cont, const Rect& shape, Rect& tab, Rect& inside)
{
	if (shape.h < (RADIUS*2)) {
		log_info ("draw_iconbox: too short\n");
		return;
	}

	if (shape.w < (BLOCK_WIDTH + (RADIUS*2))) {
		log_info ("draw_iconbox: too narrow\n");
		std::cout << cont << std::endl;
		return;
	}

	const int& x = shape.x;
	const int& y = shape.y;
	const int& w = shape.w;
	const int& h = shape.h;

	draw_corner (cr, shape, true,  false, true);		// Top left corner (1)
	draw_corner (cr, shape, true,  true,  true);		// Top right corner (3)
	draw_corner (cr, shape, false, false, true);		// Bottom left corner (9)

	draw_arc (cr, shape, true);				// Thin bottom right corner (12)

	cr->set_line_width (RADIUS);				// Thick top bar (2)
	cr->move_to (x+RADIUS, y+(RADIUS/2));
	cr->rel_line_to (w-(2*RADIUS), 0);
	cr->stroke();

	cr->set_line_width (SIDES);				// Thin left bar (4)
	cr->move_to (x+(SIDES/2), y+RADIUS);
	cr->rel_line_to (0, h-(2*RADIUS));
	cr->stroke();

	cr->set_line_width (SIDES);				// Thin right bar (8)
	cr->move_to (x+w-(SIDES/2), y+RADIUS);
	cr->rel_line_to (0, h-(2*RADIUS));
	cr->stroke();

	cr->set_line_width (SIDES);				// Thin bottom bar (10)
	cr->move_to (x+RADIUS, y+h-(SIDES/2));
	cr->rel_line_to (w-(2*RADIUS), 0);
	cr->stroke();

	cr->move_to (x+SIDES, y+RADIUS);			// Tab block (5)
	cr->rel_line_to (BLOCK_WIDTH+SIDES, 0);
	cr->rel_line_to (0, h-RADIUS-SIDES);
	cr->rel_line_to (-BLOCK_WIDTH+RADIUS-(SIDES*2), 0);
	cr->rel_line_to (0, -RADIUS+SIDES);
	cr->rel_line_to (-RADIUS+SIDES, 0);
	cr->close_path();
	cr->fill();

	inside = { x+BLOCK_WIDTH+(SIDES*2), y+RADIUS, w-BLOCK_WIDTH-(SIDES*3), h-RADIUS-SIDES };

	draw_corner (cr, inside, false, false, false);		// Bottom left inner corner (11)
	draw_corner (cr, inside, true,  false, false);		// Top left inner corner (6)
	draw_corner (cr, inside, true,  true,  false);		// Top right inner corner (7)

	tab = { x+SIDES, y+RADIUS, BLOCK_WIDTH, h-RADIUS-(SIDES*1) };

	vRange.push_front ({shape, cont});
}

/**
 * draw_tabbox - draw a rounded rectangle with a handy tab
 */
void
DrawingArea::draw_tabbox (const Cairo::RefPtr<Cairo::Context>& cr, GfxContainerPtr& cont, const Rect& shape, Rect& tab, Rect& inside)
{
	if (shape.h < (RADIUS*2)) {
		log_info ("draw_tabbox: too short\n");
		return;
	}

	if (shape.w < (TAB_WIDTH + (RADIUS*2))) {
		log_info ("draw_tabbox: too narrow\n");
		std::cout << cont << std::endl;
		return;
	}

	const int& x = shape.x;
	const int& y = shape.y;
	const int& w = shape.w;
	const int& h = shape.h;

	draw_corner (cr, shape, true,  false, true);		// Top left corner (1)
	draw_corner (cr, shape, true,  true,  true);		// Top right corner (3)
	draw_corner (cr, shape, false, false, true);		// Bottom left corner (9)

	draw_arc (cr, shape, true);				// Thin bottom right corner (12)

	cr->set_line_width (RADIUS);				// Thick top bar (2)
	cr->move_to (x+RADIUS, y+(RADIUS/2));
	cr->rel_line_to (w-(2*RADIUS), 0);
	cr->stroke();

	cr->set_line_width (SIDES);				// Thin left bar (4)
	cr->move_to (x+(SIDES/2), y+RADIUS);
	cr->rel_line_to (0, h-(2*RADIUS));
	cr->stroke();

	cr->set_line_width (SIDES);				// Thin right bar (8)
	cr->move_to (x+w-(SIDES/2), y+RADIUS);
	cr->rel_line_to (0, h-(2*RADIUS));
	cr->stroke();

	cr->set_line_width (SIDES);				// Thin bottom bar (10)
	cr->move_to (x+RADIUS, y+h-(SIDES/2));
	cr->rel_line_to (w-(2*RADIUS), 0);
	cr->stroke();

	cr->move_to (x+SIDES, y+RADIUS);			// Tab block (5)
	cr->rel_line_to (TAB_WIDTH+SIDES, 0);
	cr->rel_line_to (0, h-RADIUS-SIDES);
	cr->rel_line_to (-TAB_WIDTH+RADIUS-(SIDES*2), 0);
	cr->rel_line_to (0, -RADIUS+SIDES);
	cr->rel_line_to (-RADIUS+SIDES, 0);
	cr->close_path();
	cr->fill();

	inside = { x+TAB_WIDTH+(SIDES*2), y+RADIUS, w-TAB_WIDTH-(SIDES*3), h-RADIUS-SIDES };

	draw_corner (cr, inside, false, false, false);		// Bottom left inner corner (11)
	draw_corner (cr, inside, true,  false, false);		// Top left inner corner (6)
	draw_corner (cr, inside, true,  true,  false);		// Top right inner corner (7)

	tab = { x+SIDES, y+RADIUS, TAB_WIDTH, h-RADIUS-(SIDES*1) };

	vRange.push_front ({shape, cont});
}


/**
 * draw_icon
 */
void
DrawingArea::draw_icon (const Cairo::RefPtr<Cairo::Context>& cr, GfxContainerPtr& cont, Glib::RefPtr<Gdk::Pixbuf> icon, const Rect& shape, Rect& below)
{
	Rect work = shape;

	if (!icon)
		return;

	cr->save();

	work.x += ((work.w - icon->get_width()) / 2);	// Centre the icon

	Gdk::Cairo::set_source_pixbuf (cr, icon, work.x, work.y);
	work.w = icon->get_width();
	work.h = icon->get_height();
	//log_info ("icon %d,%d\n", work.w, work.h);

	cr->rectangle (work.x, work.y, work.w, work.h);
	cr->fill();
	cr->restore();
	//draw_edge (cr, work, "red");

	below = { shape.x, shape.y + (work.h + GAP), shape.w, shape.h - (work.h + GAP) };
}


/**
 * on_draw
 */
bool
DrawingArea::on_draw (const Cairo::RefPtr<Cairo::Context>& cr)
{
	//LOG_TRACE;
	if (!top_level)
		return true;

	vRange.clear();

	Gtk::Allocation allocation = get_allocation();

	Rect shape { 0, 0, allocation.get_width(), allocation.get_height() };

#if 1
	checker_rect (cr, shape, 5);
#else
	draw_grid (cr, shape);
	draw_grid_linear (cr, shape, m_c->bytes_size);
	fill_rect (cr, shape, "white");
#endif
	shape.h = 70;
	for (auto c : top_level->children) {
		draw_container (cr, c, shape);
		shape.y += 70;
	}

	return true;
}


#if 0
/**
 * on_timeout
 */
bool
DrawingArea::on_timeout (int timer_number)
{
	std::cout << "timer" << "\n";
	get_window()->invalidate (false); //RAR everything for now
	//return (m_c->device == "/dev/sdc");
	return true;
}

#endif

#if 0
/**
 * dump_range
 */
void
dump_range (const std::deque<Range>& vRange)
{
	std::cout << "Ranges:\n";
	for (auto& rg : vRange) {
		Rect         r = rg.r;
		GfxContainerPtr p = rg.p;
		std::string type = "unknown";
		if (p && p->type.size() > 0) {
			type = p->type.back();
		}

		printf ("\t%d,%d %d,%d %p (%s) - %ld\n", r.x, r.y, r.w, r.h, (void*)p.get(), type.c_str(), p.use_count());
	}
}

#endif
/**
 * on_mouse_motion
 */
bool
DrawingArea::on_mouse_motion (GdkEventMotion* event)
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
bool
DrawingArea::on_mouse_leave (GdkEventCrossing* event)
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
bool
DrawingArea::on_mouse_click (GdkEventButton* event)
{
	//std::cout << "mouse click: (" << event->x << "," << event->y << ")\n";

	if (event->type != GDK_BUTTON_PRESS)
		return false;			// We haven't handled the event

	grab_focus();				// Place the windows focus on the DrawingArea

	DParted *dp = reinterpret_cast<DParted*> (get_toplevel());

	for (const auto& rg : vRange) {
		const Rect& r = rg.r;

		if ((event->x >= r.x) && (event->x < (r.x + r.w)) &&
		    (event->y >= r.y) && (event->y < (r.y + r.h))) {
			if (dp->set_focus (rg.p)) {
				get_window()->invalidate (false);
			}
			break;
		}
	}

	if (event->button == 3) {			// Right-click
		popup_menu (event->x_root, event->y_root);
	}

	return true;		// We've handled the event
}


/**
 * get_coords
 *
 * Get the absolute screen coordinates of the highlighted container.
 */
bool
DrawingArea::get_coords (int& x, int& y)
{
	DParted *dp = reinterpret_cast<DParted*> (get_toplevel());
	if (!dp) {
		std::cout << "No DParted" << std::endl;
		return false;
	}

	GfxContainerPtr c = dp->get_focus();
	if (!c) {
		//std::cout << "No focus" << std::endl;
		return false;
	}

	Glib::RefPtr<Gdk::Window> w = get_window();
	if (!w) {
		return false;
	}

	int ox = 0;
	int oy = 0;
	w->get_origin (ox, oy);		// Coords of DParted's main window (inside chrome)

	Gtk::Widget* window = dynamic_cast<Gtk::Widget*> (get_toplevel());
	if (!window) {
		return false;
	}

	int tx = 0;
	int ty = 0;
	if (!translate_coordinates (*window, 0, 0, tx, ty)) {
		return false;		// Coords of DrawingArea within DParted's window
	}

	Rect r = get_rect (c);		// Size and shape of selected container
	if (r.x < 0) {
		return false;
	}
#if 0
	printf ("o %d, %d\n", ox, oy);
	printf ("t %d, %d\n", tx, ty);
	printf ("r %d, %d, %d\n", r.x, r.y, r.h);
#endif

	x = ox + tx + r.x;
	y = oy + r.y + r.h;
	return true;
}


/**
 * on_menu_file_popup_generic
 */
void
DrawingArea::on_menu_file_popup_generic (void)
{
	//LOG_TRACE;
}

/**
 * make_menu
 */
void
DrawingArea::make_menu (void)
{
	Glib::RefPtr<Gio::SimpleActionGroup> refActionGroup = Gio::SimpleActionGroup::create();

	refActionGroup->add_action("edit",    sigc::mem_fun(*this, &DrawingArea::on_menu_file_popup_generic));
	refActionGroup->add_action("process", sigc::mem_fun(*this, &DrawingArea::on_menu_file_popup_generic));
	refActionGroup->add_action("remove",  sigc::mem_fun(*this, &DrawingArea::on_menu_file_popup_generic));

	insert_action_group("examplepopup", refActionGroup);

	m_refBuilder = Gtk::Builder::create();

	Glib::ustring ui_info =
		"<interface>"
		"	<menu id='menu-examplepopup'>"
		"		<section>"
		"			<item>"
		"				<attribute name='label' translatable='yes'>Edit</attribute>"
		"				<attribute name='action'>examplepopup.edit</attribute>"
		"			</item>"
		"			<item>"
		"				<attribute name='label' translatable='yes'>Process</attribute>"
		"				<attribute name='action'>examplepopup.process</attribute>"
		"			</item>"
		"			<item>"
		"				<attribute name='label' translatable='yes'>Remove</attribute>"
		"				<attribute name='action'>examplepopup.remove</attribute>"
		"			</item>"
		"		</section>"
		"	</menu>"
		"</interface>";

	try {
		m_refBuilder->add_from_string(ui_info);
	} catch(const Glib::Error& ex) {
		std::cerr << "building menus failed: " << ex.what();
	}

	//Get the menu:
	Glib::RefPtr<Glib::Object> object = m_refBuilder->get_object("menu-examplepopup");
	Glib::RefPtr<Gio::Menu> gmenu = Glib::RefPtr<Gio::Menu>::cast_dynamic(object);
	if (!gmenu) {
		g_warning("GMenu not found");
	}

	m_pMenuPopup = new Gtk::Menu(gmenu);

	if (m_pMenuPopup && (!m_pMenuPopup->get_attach_widget())) {
		m_pMenuPopup->attach_to_widget(*this);
	}

	// A lambda to mark the popup menu closure
	m_pMenuPopup->signal_deactivate().connect([this] { menu_active = false; });
}

#if 0
/**
 * draw_container_examples
 */
void
draw_container_examples (const Cairo::RefPtr<Cairo::Context>& cr, GfxContainerPtr cont, Rect shape, Rect* right)
{
	if (!cont)
		return;

#if 0 //XXX icon + label above
	Rect inside;
	Rect below;

	fill_rect (cr, shape, "white");
	cr->set_source_rgba (0.0, 0.0, 0.0, 1.0);

	Pango::FontDescription font;
	font.set_family ("Liberation Sans");

	Glib::RefPtr<Pango::Layout> layout = Pango::Layout::create (cr);
	layout->set_font_description (font);

	std::string title = "<b>" + cont->get_device_name() + "</b> &#8211; " + get_size (cont->bytes_size) + " <small>(" + std::to_string(100*cont->bytes_used/cont->bytes_size) + "% used)</small>";

	// https://developer.gnome.org/pango/stable/PangoMarkupFormat.html
	layout->set_markup (title);

	int tw = 0;
	int th = 0;
	layout->get_pixel_size (tw, th);
	printf ("text width = %d, height = %d\n", tw, th);

	cr->move_to (shape.x + 2, shape.y + 2);
	layout->set_width (Pango::SCALE * (shape.w - 4));
	layout->set_ellipsize (Pango::ELLIPSIZE_END);
	layout->update_from_cairo_context (cr);
	layout->show_in_cairo_context (cr);

	th += 2*2;

	shape.w = 2*TAB_WIDTH + 2*GAP;
	shape.y += th;
	shape.h -= th;

	draw_box (cr, shape, inside);
	//draw_edge (cr, inside, "red");

	cr->set_source_rgba (1.0, 1.0, 1.0, 1.0);
	draw_border (cr, inside);
	cr->fill();

	draw_icon (cr, "disk", inside, below);
#endif

#if 0 //XXX icon + shaded label below
	Rect inside;
	Rect below;

	shape.w = 2*TAB_WIDTH + 2*GAP;

	draw_box (cr, shape, inside);
	//draw_edge (cr, inside, "red");

	cr->set_source_rgba (1.0, 1.0, 1.0, 1.0);
	draw_border (cr, inside);
	cr->fill();

	draw_icon (cr, "disk", inside, below);

	std::string labeld = cont->device;
	std::string labels = get_size (cont->bytes_size);

	Pango::FontDescription font;
	Glib::RefPtr<Pango::Layout> layoutd = Pango::Layout::create (cr);
	Glib::RefPtr<Pango::Layout> layouts = Pango::Layout::create (cr);

	int xd = 0, yd = 0, wd = 0, hd = 0;
	int xs = 0, ys = 0, ws = 0, hs = 0;

	size_t pos = labeld.find_last_of ('/');
	if (pos != std::string::npos) {
		labeld = labeld.substr (pos+1);
	}
	labeld = "<b>" + labeld + "</b>";

	font.set_family ("Liberation Sans");		//THEME - block_label_font

	font.set_size (12 * Pango::SCALE);		//THEME - block_label_font_size
	layoutd->set_font_description (font);

	font.set_size (8 * Pango::SCALE);		//THEME - ratio of block_label_font_size
	layouts->set_font_description (font);

	layoutd->set_markup (labeld);
	layouts->set_markup (labels);

	layoutd->get_pixel_size (wd, hd);
	layouts->get_pixel_size (ws, hs);

	xd = (below.w - wd) / 2; if (xd < 0) xd = 0;
	xs = (below.w - ws) / 2; if (xs < 0) xs = 0;

	yd = below.y + below.h - hd - hs;
	ys = below.y + below.h - hs;

	// Draw block label
	cr->set_source_rgba (1.0, 1.0, 1.0, 1.0);	//THEME - block_label_highlight_colour
	cr->move_to (xd+2, yd+0); layoutd->update_from_cairo_context (cr); layoutd->show_in_cairo_context (cr);
	cr->move_to (xd-2, yd+0); layoutd->update_from_cairo_context (cr); layoutd->show_in_cairo_context (cr);
	cr->move_to (xd+0, yd+2); layoutd->update_from_cairo_context (cr); layoutd->show_in_cairo_context (cr);
	cr->move_to (xd+0, yd-2); layoutd->update_from_cairo_context (cr); layoutd->show_in_cairo_context (cr);

	cr->move_to (xd, yd);
	cr->set_source_rgba (0.0, 0.0, 0.0, 1.0);	//THEME - block_label_text_colour
	layoutd->update_from_cairo_context (cr);
	layoutd->show_in_cairo_context (cr);

	// Draw size label
	cr->move_to (xs, ys);
	cr->set_source_rgba (0.0, 0.0, 0.0, 1.0);	//THEME - block_label_text_colour
	layouts->update_from_cairo_context (cr);
	layouts->show_in_cairo_context (cr);
#endif
}

/**
 * get_protective
 */
TablePtr
DrawingArea::get_protective (GfxContainerPtr& c)
{
	GfxContainerPtr child;

	//std::cout << "1: " << c << "\n";
	if (!c)
		return nullptr;

	if (!c->is_a ("Table"))
		return nullptr;

	if (c->children.size() != 1)
		return nullptr;

	child = c->children[0];
	//std::cout << "2: " << child << "\n";
	if (!child)
		return nullptr;

	if (!child->is_a ("Partition"))
		return nullptr;

	if (child->children.size() != 1)
		return nullptr;

	child = child->children[0];
	//std::cout << "3: " << child << "\n";
	if (!child)
		return nullptr;

	if (child->is_a ("Table"))
		return std::dynamic_pointer_cast<Table> (child);

	return nullptr;
}

#endif

/**
 * set_data
 */
void
DrawingArea::set_data (GfxContainerPtr& c)
{
	// check we've been given a top level object?

	if (!c)
		return;

	// invalidate window
	unsigned int children = c->children.size();
	set_size_request (1000, 70 * children);

	top_level = c;
	//top_level->dump();
}

/**
 * get_focus
 */
GfxContainerPtr
DrawingArea::get_focus (int x, int y)
{
	GfxContainerPtr match;

	for (auto rg : vRange) {
		Rect r = rg.r;
		if ((x >= r.x) && (x < (r.x + r.w)) && (y >= r.y) && (y < (r.y + r.h))) {
			match = rg.p;
			break;
		}
	}

	return match;
}

/**
 * on_textview_query_tooltip
 */
bool
DrawingArea::on_textview_query_tooltip(int x, int y, bool keyboard_tooltip, const Glib::RefPtr<Gtk::Tooltip>& tooltip)
{
	std::stringstream ss;

	GfxContainerPtr c = get_focus (x, y);
	if (c) {
		//ss << "<b>" << c->name << "</b> (" << c->uuid << ")";
		ss << "<b>" << c->get_tooltip() << "</b>";

		tooltip->set_markup(ss.str());
		tooltip->set_icon_from_icon_name("dialog-information", Gtk::ICON_SIZE_MENU);

		return true;
	}

	return false;
}


/**
 * draw_container - recursively draw a set of containers
 */
void
DrawingArea::draw_container (const Cairo::RefPtr<Cairo::Context>& cr, GfxContainerPtr& cont, Rect shape)
{
	if (!cr)
		return;
	if (!cont)
		return;

#if 0
	if (!top_level->update_info())
		return;
#endif

	std::string display = cont->display;
	Gdk::RGBA background = cont->background;
	Gdk::RGBA colour = cont->colour;
	Glib::RefPtr<Gdk::Pixbuf> icon = cont->icon;
	std::string name = cont->name;
	std::string label = cont->label;
	bool usage = cont->usage;

	if (display.empty()) {
		display = "box";
	}

	//XXX need to define these somewhere .h?
	//XXX hard-code into theme.cpp?	A bit limiting
	if (display == "never") {		// Don't display at all
		return;
	}

	std::vector<GfxContainerPtr> children = cont->children;

	if ((display != "box") && (display != "empty") && (display != "icon") && (display != "iconbox") && (display != "tabbox")) {
		display = "box";
	}

	if (display == "empty") {		// Only display if there's no children
		if (children.empty()) {
			display = "box";
		}
	}

	//Rect tab;
	Rect inside;

	//printf ("object = %s (%s) -- %d,%d\n", cont->name.c_str(), cont->uuid.c_str(), shape.w, TAB_WIDTH);
	if (shape.w < TAB_WIDTH) {
#if 0
		printf ("draw_container: too narrow\n");
		std::cout << cont << std::endl;
#endif
		return;
	}

	if (display == "icon") {		// Large icon
		Rect box = shape;
		Rect right = shape;
		int width = 48 + SIDES*2;	//XXX fixed at 48px square (for now)
		box.w = width;
		right.x += (width + GAP);
		right.w -= (width + GAP);

		fill_area (cr, box, background);

		Rect box2 = box;
		box.x += GAP;
		box.y += GAP;
		box.w -= (GAP*2);
		box.h -= (GAP*2);

		Rect below;
		vRange.push_front ({shape, cont});
		//std::cout << "Icon: " << icon << std::endl;
		draw_icon (cr, cont, icon, box, below);
		draw_text (cr, box2, name);

		inside = right;
		/* theme
		 *	icon
		 *	background
		 * get icon
		 * rect space = icon size
		 * rect right = remaining space
		 * draw_icon (clipped to 64x64)
		 * rect right
		 * rect inside (for label)
		 */
	} else if (display == "iconbox") {	// A box containing a small icon
		Rect tab;
		set_colour (cr, colour);
		draw_iconbox (cr, cont, shape, tab, inside);

		Glib::RefPtr<Gdk::Pixbuf> icon;

		icon = gui_app->get_theme()->get_icon ("table");
		draw_icon (cr, cont, icon,  tab, tab);
		icon = gui_app->get_theme()->get_icon ("shield");
		draw_icon (cr, cont, icon, tab, tab);

		/* theme
		 *	icon
		 *	colour
		 *	background
		 * get_icon
		 * rect space = icon size
		 * rect inside = remaining space
		 * draw_box
		 * rect tab (icon space)
		 * draw icon(s)
		 */
	} else if (display == "box") {		// A simple coloured box
		set_colour (cr, colour);
		draw_box (cr, cont, shape, inside);

		fill_area (cr, inside, background);

		if (usage) {
			Rect usage = inside;
			usage.w = usage.w * 2 / 3;
			cr->set_source_rgba (0.97, 0.97, 0.42, 1.0);
			draw_border (cr, usage);
			cr->fill();
		}
		if (!label.empty()) {
			draw_text (cr, shape, label);
		}
		/* theme
		 *	colour
		 *	background
		 * draw_box
		 * rect inside
		 */
	} else if (display == "tabbox") {	// A coloured box with a handy tab
		set_colour (cr, colour);
		Rect tab;
		draw_tabbox (cr, cont, shape, tab, inside);
		/* theme
		 *	colour
		 *	background
		 */
	} else if (display == "empty") {	// Do nothing for now
		//printf ("EMPTY\n");
		inside = shape;
	} else {
		printf ("unknown display type: %s\n", display.c_str());
		return;
	}

	uint64_t total = cont->bytes_size;
	uint64_t bpp   = total / inside.w;	// bytes per pixel

	for (auto c : cont->children) {
		if (c->bytes_size > total) {
			total = c->bytes_size;		//XXX tmp -- need to get intermediate object
			bpp   = total / inside.w;
		}
		long offset = c->parent_offset / bpp;
		long size   = c->bytes_size    / bpp;

		Rect next = inside;
		next.x += offset;
		next.w = size;

		draw_container(cr, c, next);
	}
	//XXX vRange.push_front ({work, cont});			// Associate a region with a container

	if (cont->get_focus() && (has_focus() || menu_active)) {
		draw_focus (cr, shape);
	}
}


/**
 * get_rect
 */
Rect
DrawingArea::get_rect (GfxContainerPtr g)
{
	Rect r = { -1, -1, -1, -1 };
	if (!g)
		return r;

	for (const auto& rg : vRange) {
		if (rg.p == g) {
			return rg.r;
		}
	}

	return r;
}

/**
 * on_keypress
 */
bool
DrawingArea::on_keypress(GdkEventKey* ev)
{
	bool redraw  = false;
	bool handled = false;

	//std::cout << "Key: " << std::dec << ev->keyval << " (0x" << std::hex << ev->keyval << ")" << std::dec << std::endl;

	//Extra keys: Delete, Insert, Space/Enter (select)?

	DParted *dp = reinterpret_cast<DParted*> (get_toplevel());
	if (!dp) {
		std::cout << "No DParted" << std::endl;
		return false;
	}

	GfxContainerPtr c = dp->get_focus();
	if (!c) {
		//std::cout << "No focus" << std::endl;
		return false;
	}

	GfxContainerPtr new_sel;
	Rect r;
	int x = 0;
	int y = 0;
	switch (ev->keyval) {
		case GDK_KEY_Menu:	// 65383 (0xFF67)
			get_coords(x, y);
			popup_menu (x, y);
			handled = true;
			break;
		case GDK_KEY_Left:	// 65361 (0xFF51)
			new_sel = c->get_left();
			redraw = dp->set_focus (new_sel);
			handled = true;
			break;
		case GDK_KEY_Up:	// 65362 (0xFF52)
			r = get_rect(c);
			r.y = ((r.y/70) * 70) - 35;
			new_sel = get_focus (r.x, r.y);
			redraw = dp->set_focus (new_sel);
			handled = true;
			break;
		case GDK_KEY_Right:	// 65363 (0xFF53)
			new_sel = c->get_right();
			redraw = dp->set_focus (new_sel);
			handled = true;
			break;
		case GDK_KEY_Down:	// 65364 (0xFF54)
			r = get_rect(c);
			r.y = ((r.y/70) * 70) + 70 + 35;
			new_sel = get_focus (r.x, r.y);
			redraw = dp->set_focus (new_sel);
			handled = true;
			break;
	}

	if (redraw) {
		get_window()->invalidate (false);
	}

	return handled;
}


/**
 * on_focus_in
 */
bool
DrawingArea::on_focus_in (GdkEventFocus* event)
{
	LOG_TRACE;

	DParted *dp = reinterpret_cast<DParted*> (get_toplevel());
	if (!dp) {
		std::cout << "No DParted" << std::endl;
		return false;
	}
	GfxContainerPtr c = dp->get_focus();
	if (!c) {
		//std::cout << "No focus" << std::endl;
		c = get_focus (0, 0);
		if (c) {
			dp->set_focus (c);
		}
	}

	return true;
}

/**
 * on_focus_out
 */
bool
DrawingArea::on_focus_out (GdkEventFocus* event)
{
	LOG_TRACE;
	return true;
}


/**
 * popup_menu
 */
void
DrawingArea::popup_menu (int x, int y)
{
	if (!m_pMenuPopup) {
		return;
	}

	m_pMenuPopup->popup ([x,y] (int& xc, int& yc, bool& in) { xc = x; yc = y; in = false; }, 0, gtk_get_current_event_time());

	menu_active = true;
}

