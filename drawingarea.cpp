/* Copyright (c) 2013 Richard Russon (FlatCap)
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

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include "container.h"
#include "drawingarea.h"
#include "filesystem.h"
#include "log.h"
#include "theme.h"
#include "utils.h"
#include "whole.h"
#include "log_trace.h"

const double ARC_N = 3*M_PI_2;		// Compass points in radians
const double ARC_E = 0*M_PI_2;
const double ARC_S = 1*M_PI_2;
const double ARC_W = 2*M_PI_2;

const int GAP         =  3;	// Space between partitions
const int RADIUS      =  8;	// Curve radius of corners
const int SIDES       =  2;	// Width of sides and base
const int TAB_WIDTH   = 10;	// Space in left side-bar
const int BLOCK_WIDTH = 24;	// Placeholder for icons

/**
 * DPDrawingArea
 */
DPDrawingArea::DPDrawingArea() :
	//Glib::ObjectBase ("MyDrawingArea"),
	m_c (nullptr),
	theme (nullptr),
	sel_x (-1),
	sel_y (-1),
	mouse_close (false)
{
	//set_size_request (800, 77);
	set_size_request (400, 70);
	set_hexpand (true);
	set_vexpand (false);

	add_events (Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::LEAVE_NOTIFY_MASK);

	signal_button_press_event() .connect (sigc::mem_fun (*this, &DPDrawingArea::on_mouse_click));
	signal_motion_notify_event().connect (sigc::mem_fun (*this, &DPDrawingArea::on_mouse_motion));
	signal_leave_notify_event() .connect (sigc::mem_fun (*this, &DPDrawingArea::on_mouse_leave));

#if 0
	sigc::slot<bool> my_slot = sigc::bind (sigc::mem_fun (*this, &DPDrawingArea::on_timeout), 0);
	sigc::connection conn = Glib::signal_timeout().connect (my_slot, 300); // ms
#endif

	theme = new Theme();

	theme->read_config ("themes/default.conf");

	//set_tooltip_text("tooltip number 1");

	set_has_tooltip();	// We'll be handling the tooltips ourself
	signal_query_tooltip().connect(sigc::mem_fun(*this, &DPDrawingArea::on_textview_query_tooltip));
}

/**
 * ~DPDrawingArea
 */
DPDrawingArea::~DPDrawingArea()
{
	delete theme;
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
void set_colour (const Cairo::RefPtr<Cairo::Context>& cr, const std::string& colour)
{
	Gdk::RGBA rgba ("rgba(0,0,0,0)");		// Transparent
	if (!rgba.set (colour)) {
		log_error ("don't understand colour: %s\n", colour.c_str());
	}
	cr->set_source_rgba (rgba.get_red(), rgba.get_green(), rgba.get_blue(), rgba.get_alpha());
}

/**
 * draw_edge - 1px rectangular border
 */
void
draw_edge (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape, const std::string& colour)
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
fill_area (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape, const std::string& colour)
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
fill_rect (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape, const std::string& colour)
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
				cr->set_source_rgba (0.0, 0.0, 0.0, 0.0);

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

	int left = std::max (GAP, (shape.w - tw) / 2);
	//printf ("left = %d\n", left);
	cr->move_to (shape.x + left, shape.y + shape.h - th);

#if 0
	layout->set_width (Pango::SCALE * (shape.w - 4));
	layout->set_ellipsize (Pango::ELLIPSIZE_END);
#endif
	layout->update_from_cairo_context (cr);
	layout->show_in_cairo_context (cr);
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
DPDrawingArea::draw_block (const Cairo::RefPtr<Cairo::Context>& cr, ContainerPtr& cont, const Rect& shape, Rect& tab, Rect& right)
{
	if (shape.h < (RADIUS*2)) {
		log_info ("draw_tab: too short\n");
		return;
	}

	if (shape.w < (BLOCK_WIDTH + (RADIUS*2))) {
		log_info ("draw_tab: too narrow\n");
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
DPDrawingArea::draw_box (const Cairo::RefPtr<Cairo::Context>& cr, ContainerPtr& cont, const Rect& shape, Rect& inside)
{
	if (shape.h < (RADIUS*2)) {
		log_info ("draw_box: too short\n");
		return;
	}

	if (shape.w < (TAB_WIDTH + (RADIUS*2))) {
		log_info ("draw_box: too narrow\n");
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
DPDrawingArea::draw_iconbox (const Cairo::RefPtr<Cairo::Context>& cr, ContainerPtr& cont, const Rect& shape, Rect& tab, Rect& inside)
{
	if (shape.h < (RADIUS*2)) {
		log_info ("draw_iconbox: too short\n");
		return;
	}

	if (shape.w < (BLOCK_WIDTH + (RADIUS*2))) {
		log_info ("draw_iconbox: too narrow\n");
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
DPDrawingArea::draw_tabbox (const Cairo::RefPtr<Cairo::Context>& cr, ContainerPtr& cont, const Rect& shape, Rect& tab, Rect& inside)
{
	if (shape.h < (RADIUS*2)) {
		log_info ("draw_tabbox: too short\n");
		return;
	}

	if (shape.w < (TAB_WIDTH + (RADIUS*2))) {
		log_info ("draw_tabbox: too narrow\n");
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
DPDrawingArea::draw_icon (const Cairo::RefPtr<Cairo::Context>& cr, ContainerPtr& cont, const std::string& name, const Rect& shape, Rect& below /*=nullptr*/)
{
	Glib::RefPtr<Gdk::Pixbuf> pb;

	Rect work = shape;

	pb = theme->get_icon (name);
	if (!pb) {
		std::cout << "no icon!\n";
		return;
	}

	cr->save();

	work.x += ((work.w - pb->get_width()) / 2);	// Centre the icon

	Gdk::Cairo::set_source_pixbuf (cr, pb, work.x, work.y);
	work.w = pb->get_width();
	work.h = pb->get_height();
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
DPDrawingArea::on_draw (const Cairo::RefPtr<Cairo::Context>& cr)
{
	//LOG_TRACE;
	if (!m_c)
		return true;

	vRange.clear();

	Gtk::Allocation allocation = get_allocation();

	Rect shape { 0, 0, allocation.get_width(), allocation.get_height() };

#if 1
	checker_rect (cr, shape, 7);
#else
	draw_grid (cr, shape);
	draw_grid_linear (cr, shape, m_c->bytes_size);
	fill_rect (cr, shape, "white");
#endif
	draw_container (cr, m_c, shape);

	return true;
}


#if 0
/**
 * on_timeout
 */
bool
DPDrawingArea::on_timeout (int timer_number)
{
	std::cout << "timer" << "\n";
	get_window()->invalidate (false); //RAR everything for now
	//return (m_c->device == "/dev/sdc");
	return true;
}

#endif

/**
 * on_mouse_motion
 */
bool
DPDrawingArea::on_mouse_motion (GdkEventMotion* event)
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
DPDrawingArea::on_mouse_leave (GdkEventCrossing* event)
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
DPDrawingArea::on_mouse_click (GdkEventButton* event)
{
	std::cout << "mouse click: (" << event->x << "," << event->y << ")\n";

	sel_x = event->x;
	sel_y = event->y;

	std::cout << "Range contains " << vRange.size() << " items\n";

	for (auto rg : vRange) {
		Rect r = rg.r;
		bool b = ((event->x >= r.x) && (event->x < (r.x + r.w)) &&
			  (event->y >= r.y) && (event->y < (r.y + r.h)));

		if (b) {
#if 0
			rg.p->mouse_event();
#else
			Gtk::MessageDialog dialog ("are you sure?", false, Gtk::MessageType::MESSAGE_QUESTION, Gtk::ButtonsType::BUTTONS_OK, true);
			dialog.set_title ("delete something");
			dialog.run();
			break;
#endif
		}

		if (b) log_error ("\033[01;31mRange: %d,%d %d,%d %p (%s)\033[0m\n", r.x, r.y, r.w, r.h, (void*) rg.p.get(), rg.p->type.back().c_str());
		else   log_info  ("\033[01;32mRange: %d,%d %d,%d %p (%s)\033[0m\n", r.x, r.y, r.w, r.h, (void*) rg.p.get(), rg.p->type.back().c_str());
	}
	log_info ("\n");

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
	GdkWindow* window;
	gint8 send_event;
	guint32 time;
	gdouble x;
	gdouble y;
	gdouble* axes;
	guint state;
	guint button;
	GdkDevice* device;
	gdouble x_root, y_root;
#endif

	return true;
}


/**
 * find_subst - get the position of a {tag}
 */
bool find_subst (const std::string& text, std::string& tag, size_t& start, size_t& stop)
{
	//XXX on failure, point start at error
	const char* valid = "abcdefghijklmnopqrstuvwxyz_";
	size_t open  = std::string::npos;
	size_t close = std::string::npos;

	open = text.find ('{');
	if (open == std::string::npos) {
		log_debug ("nothing to substitute\n");
		return false;
	}

	close = text.find_first_not_of (valid, open+1);
	if (close == std::string::npos) {
		log_error ("missing close brace\n");
		return false;
	}

	if (text[close] != '}') {
		log_error ("invalid tag name\n");
		return false;
	}

	if (close == (open+1)) {
		log_error ("missing tag\n");
		return false;
	}

	tag   = text.substr (open+1, close-open-1);
	start = open;
	stop  = close;

	return true;
}

#if 0
/**
 * draw_container_examples
 */
void
draw_container_examples (const Cairo::RefPtr<Cairo::Context>& cr, ContainerPtr cont, Rect shape, Rect* right)
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
DPDrawingArea::get_protective (ContainerPtr& c)
{
	ContainerPtr child;

	//std::cout << "1: " << c << "\n";
	if (!c)
		return nullptr;

	if (!c->is_a ("table"))
		return nullptr;

	if (c->children.size() != 1)
		return nullptr;

	child = c->children[0];
	//std::cout << "2: " << child << "\n";
	if (!child)
		return nullptr;

	if (!child->is_a ("partition"))
		return nullptr;

	if (child->children.size() != 1)
		return nullptr;

	child = child->children[0];
	//std::cout << "3: " << child << "\n";
	if (!child)
		return nullptr;

	if (child->is_a ("table"))
		return std::dynamic_pointer_cast<Table> (child);

	return nullptr;
}

#endif

/**
 * set_data
 */
void
DPDrawingArea::set_data (ContainerPtr& c)
{
	// check we've been given a block device

	m_c = c;
	if (!m_c)
		return;

	// invalidate window
	//unsigned int children = c->children.size();
	//set_size_request (400, 50 * children);
}

/**
 * get_focus
 */
ContainerPtr
DPDrawingArea::get_focus (int x, int y)
{
	ContainerPtr match;

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
DPDrawingArea::on_textview_query_tooltip(int x, int y, bool keyboard_tooltip, const Glib::RefPtr<Gtk::Tooltip>& tooltip)
{
	std::stringstream ss;

	ContainerPtr c = get_focus (x, y);
	if (c) {
		ss << "<b>" << c->name << "</b> (" << c->uuid << ")";

		tooltip->set_markup(ss.str());
		tooltip->set_icon_from_icon_name("dialog-information", Gtk::ICON_SIZE_MENU);

		return true;
	}

	return false;
}


/**
 * draw_label - write some text into an area
 */
void
draw_label (const Cairo::RefPtr<Cairo::Context>& cr, ContainerPtr cont, Rect shape)
{
	std::string label;

	std::string device = cont->get_device_name();
	size_t pos = device.find_last_of ('/');
	if (pos == std::string::npos) {
		label = cont->device;
		if (label.empty()) {
			label = "<none>";	//RAR tmp
		}
	} else {
		label = device.substr (pos+1);
	}

	if (cont->is_a ("filesystem")) {
		FilesystemPtr fs = std::dynamic_pointer_cast<Filesystem> (cont);
		if (fs) {
			if (!fs->label.empty()) {
				label += " \"" + fs->label + "\"";
			}
		}
	}

	label += "\n";
	label += get_size (cont->bytes_size);

	Pango::FontDescription font;
	font.set_family ("Liberation Sans");

	Glib::RefPtr<Pango::Layout> layout = Pango::Layout::create (cr);
	layout->set_font_description (font);

	font.set_size (10 * Pango::SCALE);		//THEME - block_label_font_size
	layout->set_font_description (font);

	layout->set_text (label);

	cr->set_source_rgba (0.0, 0.0, 0.0, 1.0);
	cr->move_to (shape.x + 2, shape.y + 2);

	layout->set_width (Pango::SCALE * (shape.w - 4));
	layout->set_ellipsize (Pango::ELLIPSIZE_END);
	layout->update_from_cairo_context (cr);
	layout->show_in_cairo_context (cr);
}

/**
 * draw_container - recursively draw a set of containers
 */
void
DPDrawingArea::draw_container (const Cairo::RefPtr<Cairo::Context>& cr, ContainerPtr& cont, Rect shape)
{
	//Rect tab;
	Rect inside;

	if (!cont)
		return;

	printf ("object = %s (%s) -- %d,%d\n", cont->name.c_str(), cont->uuid.c_str(), shape.w, TAB_WIDTH);
	if (shape.w < TAB_WIDTH) {
		printf ("too narrow\n");
		return;
	}

	std::string path = cont->get_path();
	std::string name = cont->name;

	std::string display = theme->get_config (path, name, "display");
	if (display.empty()) {
		display = "box";
	}

	//XXX need to define these somewhere .h?
	//XXX hard-code into theme.cpp?	A bit limiting
	if (display == "never") {		// Don't display at all
		return;
	}

	std::vector<ContainerPtr> children = cont->get_children();

	if ((display != "box") && (display != "empty") && (display != "icon") && (display != "iconbox") && (display != "tabbox")) {
		display = "box";
	}

	if (display == "empty") {		// Only display if there's no children
		if (children.empty()) {
			display = "box";
		}
	}

	std::string colour     = theme->get_config (path, name, "colour");
	std::string background = theme->get_config (path, name, "background");
	std::string label      = theme->get_config (path, name, "label");
	std::string icon       = theme->get_config (path, name, "icon");

	//XXX vRange.push_front ({work, cont});			// Associate a region with a container

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

#if 0
		draw_edge (cr, box, "red");
		draw_edge (cr, right, "red");
		return;
#endif

		Rect below;
		vRange.push_front ({shape, cont});
		draw_icon (cr, cont, icon, box, below);
		draw_text (cr, box2, cont->name);

		//printf ("height = %d\n", shape.h);
		//std::cout << "icon = " << icon << std::endl;
		//draw_icon (cr, icon, background, shape, below, right);

		inside = right;
#ifdef RAR
		theme
			icon
			background
		get icon
		rect space = icon size
		rect right = remaining space
		draw_icon (clipped to 64x64)
		rect right
		rect inside (for label)
#endif
	} else if (display == "iconbox") {	// A box containing a small icon
		//Rect box = shape;

		Rect tab;
		set_colour (cr, colour);
		draw_iconbox (cr, cont, shape, tab, inside);

		draw_icon (cr, cont, "table", tab, tab);
		draw_icon (cr, cont, "shield", tab, tab);
#if 0
		background = "rgba(255,255,0,0.3)";
		fill_area (cr, inside, background);
#endif

#ifdef RAR
		theme
			icon
			colour
			background
		get_icon
		rect space = icon size
		rect inside = remaining space
		draw_box
		rect tab (icon space)
		draw icon(s)
#endif
	} else if (display == "box") {		// A simple coloured box
		set_colour (cr, colour);
		draw_box (cr, cont, shape, inside);

		fill_area (cr, inside, background);

		//XXX if usage
		if (theme->get_config (path, name, "usage") == "true") {
#if 1
			Rect usage = inside;
			usage.w = usage.w * 2 / 3;
			cr->set_source_rgba (0.97, 0.97, 0.42, 1.0);
			draw_border (cr, usage);
			cr->fill();
#endif
		}
		if (!label.empty()) {
			std::string tag;
			size_t start = std::string::npos;
			size_t stop  = std::string::npos;
			while (find_subst (label, tag, start, stop)) {
				std::string value = cont->get_property (tag);
				if (value == tag) {		//XXX tmp
					break;
				}
				label.replace (start, stop-start+1, value);
			}

			printf ("label = %s\n", label.c_str());

			draw_text (cr, shape, label);
		}

		if (cont->name == "ext4") {
			draw_focus (cr, shape);
			//draw_edge (cr, shape, "red");
		}


#ifdef RAR
		theme
			colour
			background
		draw_box
		rect inside
#endif
	} else if (display == "tabbox") {	// A coloured box with a handy tab
		set_colour (cr, colour);
		Rect tab;
		draw_tabbox (cr, cont, shape, tab, inside);

#ifdef RAR
		theme
			colour
			background
#endif
	} else if (display == "empty") {	// Do nothing for now
		//printf ("EMPTY\n");
		inside = shape;
	} else {
		printf ("unknown display type: %s\n", display.c_str());
		return;
	}

	long total = cont->bytes_size;
	long bpp   = total / inside.w;	// bytes per pixel

	//printf ("\ttotal = %ld\n", total);
	//printf ("\tbpp   = %ld\n", bpp);

	for (auto c : cont->get_children()) {
		if (c->bytes_size > total) {
			total = c->bytes_size;		//XXX tmp -- need to get intermediate object
			bpp   = total / inside.w;
		}
		long offset = c->parent_offset / bpp;
		long size   = c->bytes_size    / bpp;

#if 0
		printf ("size = %ld, %ld\n", cont->bytes_size, c->bytes_size);
		printf ("width = %d, %d\n", inside.w, size);
#endif

		//printf ("\t\toffset = %ld\n", offset);
		//printf ("\t\tsize   = %ld\n", size);

		Rect next = inside;
		next.x += offset;
		next.w = size;

		draw_container(cr, c, next);
	}
}


