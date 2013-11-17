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

#include "block.h"
#include "container.h"
#include "drawingarea.h"
#include "filesystem.h"
#include "log.h"
#include "misc.h"
#include "partition.h"
#include "table.h"
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
	set_size_request (400, 77);
	set_hexpand (true);
	set_vexpand (false);

	add_events (Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::LEAVE_NOTIFY_MASK);

#if 0
	signal_button_press_event() .connect (sigc::mem_fun (*this, &DPDrawingArea::on_mouse_click));
	signal_motion_notify_event().connect (sigc::mem_fun (*this, &DPDrawingArea::on_mouse_motion));
	signal_leave_notify_event() .connect (sigc::mem_fun (*this, &DPDrawingArea::on_mouse_leave));
#endif

#if 0
	sigc::slot<bool> my_slot = sigc::bind (sigc::mem_fun (*this, &DPDrawingArea::on_timeout), 0);
	sigc::connection conn = Glib::signal_timeout().connect (my_slot, 800); // ms
#endif
	//std::cout << "GType name: " << G_OBJECT_TYPE_NAME (gobj()) << "\n";

	theme = new Theme();

	theme->read_config ("themes/default.conf");
}

/**
 * ~DPDrawingArea
 */
DPDrawingArea::~DPDrawingArea()
{
	delete theme;
}


#if 0
/**
 * operator<< - serialise a Rect
 */
std::ostream &
operator<< (std::ostream &stream, const Rect &r)
{
	stream << "[" << r.x << "," << r.y << "," << r.w << "," << r.h << "]";

	return stream;
}

/**
 * rand_colour - select a colour from set
 */
void
rand_colour (const Cairo::RefPtr<Cairo::Context> &cr)
{
	static int colour = 0;
	switch (colour) {
		case 0:  cr->set_source_rgba (0.0, 1.0, 0.0, 0.8); break; // Green
		case 1:  cr->set_source_rgba (0.8, 0.8, 0.0, 0.8); break; // Yellow
		case 2:  cr->set_source_rgba (0.0, 0.0, 1.0, 0.8); break; // Blue
		default: cr->set_source_rgba (1.0, 0.0, 0.0, 0.8); break; // Red
	}
	colour = (colour+1) % 4;
}

/**
 * draw_edge - one pixel red border around area
 */
void
draw_edge (const Cairo::RefPtr<Cairo::Context> &cr, const Rect &shape)
{
	const int &x = shape.x;
	const int &y = shape.y;
	const int &w = shape.w - 1;
	const int &h = shape.h - 1;

	cr->save();
	cr->set_source_rgba (1.0, 0.0, 0.0, 1.0);
	cr->set_line_width (1);

	cr->move_to     (x+0.5, y+0.5);
	cr->rel_line_to ( w, 0);
	cr->rel_line_to ( 0, h);
	cr->rel_line_to (-w, 0);
	cr->rel_line_to ( 0,-h);

	cr->stroke();
	cr->restore();
}

#endif
/**
 * fill_area - fill rect with black
 */
void
fill_area (const Cairo::RefPtr<Cairo::Context> &cr, const Rect &shape)
{
	const int &x = shape.x;
	const int &y = shape.y;
	const int &w = shape.w;
	const int &h = shape.h;

	cr->set_source_rgba (1.0, 1.0, 1.0, 1.0);

	cr->move_to     ( x, y);
	cr->rel_line_to ( w, 0);
	cr->rel_line_to ( 0, h);
	cr->rel_line_to (-w, 0);
	cr->rel_line_to ( 0,-h);

	cr->fill();
}

/**
 * checker_area - checker rect
 */
void
checker_area (const Cairo::RefPtr<Cairo::Context> &cr, const Rect &shape)
{
	const int check_size = 7;
	const int &x = shape.x;
	const int &y = shape.y;
	const int &w = shape.w;
	const int &h = shape.h;

	for (int i = 0; i <= w; i += check_size) {
		for (int j = 0; j <= h; j += check_size) {
			if (((i+j)/check_size)&1)
				cr->set_source_rgba (0.8, 0.8, 0.8, 1.0);
			else
				cr->set_source_rgba (0.0, 0.0, 0.0, 0.0);

			cr->move_to     ( x+i, y+j);
			cr->rel_line_to ( check_size,          0);
			cr->rel_line_to (          0, check_size);
			cr->rel_line_to (-check_size,          0);
			cr->rel_line_to (          0,-check_size);

			cr->fill();
		}
	}
}


/**
 * draw_border
 */
void
DPDrawingArea::draw_border (const Cairo::RefPtr<Cairo::Context> &cr, const Rect &shape, Rect *inside /*=nullptr*/)
{
	const int &r = RADIUS;
	const int &x = shape.x;
	const int &y = shape.y;
	const int &w = shape.w;
	const int &h = shape.h;

	cr->move_to (x, y+r);
	cr->arc (x+  r, y+  r, r, ARC_W, ARC_N);
	cr->arc (x+w-r, y+  r, r, ARC_N, ARC_E);
	cr->arc (x+w-r, y+h-r, r, ARC_E, ARC_S);
	cr->arc (x+  r, y+h-r, r, ARC_S, ARC_W);
	cr->close_path();

	if (inside) {			// Space inside shape
		inside->x = x + 6;
		inside->y = y + 6;
		inside->w = w;
		inside->h = h;
	}
}

/**
 * draw_rect
 */
void
DPDrawingArea::draw_rect (const Cairo::RefPtr<Cairo::Context> &cr, const std::string &colour, const Rect &shape)
{
	Gdk::RGBA c = theme->get_colour (colour);

	cr->set_source_rgba (c.get_red(), c.get_green(), c.get_blue(), c.get_alpha());
	cr->rectangle (shape.x, shape.y, shape.w, shape.h);
	cr->fill();
}

/**
 * draw_focus - 2px dashed black/white line
 */
void
DPDrawingArea::draw_focus (const Cairo::RefPtr<Cairo::Context> &cr, const Rect &shape)
{
	std::vector<double> dashes;
	dashes.push_back (5);
	dashes.push_back (5);

	cr->save();
	draw_border (cr, shape);				// Set clipping area
	cr->clip();

	cr->set_line_width (4);

	cr->set_dash (dashes, 0);
	cr->set_source_rgba (0.0, 0.0, 0.0, 1.0);		//RAR focus colours from theme
	draw_border (cr, shape);
	cr->close_path();
	cr->stroke();

	cr->set_dash (dashes, 5);
	cr->set_source_rgba (1.0, 1.0, 1.0, 1.0);
	draw_border (cr, shape);
	cr->close_path();
	cr->stroke();

	cr->restore();						// End clipping
}

/**
 * draw_highlight - 1 pixel white border
 */
void
DPDrawingArea::draw_highlight (const Cairo::RefPtr<Cairo::Context> &cr, const Rect &shape)
{
	cr->save();
	draw_border (cr, shape);
	cr->clip();
	draw_rect (cr, "rgba(0,128,0,0.2)", shape);
	cr->set_source_rgba (1.0, 1.0, 1.0, 1.0);
	draw_border (cr, shape);
	cr->stroke();
	cr->restore();
}

/**
 * draw_icon
 */
void
DPDrawingArea::draw_icon (const Cairo::RefPtr<Cairo::Context> &cr, const std::string &name, Rect &shape, Rect *below /*=nullptr*/)
{
	Glib::RefPtr<Gdk::Pixbuf> pb;

	if (below)
		*below = shape;

	pb = theme->get_icon (name);
	if (!pb) {
		return;
	}

	cr->save();
	Gdk::Cairo::set_source_pixbuf (cr, pb, shape.x, shape.y);
	shape.w = pb->get_width();
	shape.h = pb->get_height();
	//log_info ("icon %d,%d\n", shape.w, shape.h);

	cr->rectangle (shape.x, shape.y, shape.w, shape.h);
	cr->fill();
	cr->restore();

	if (below) {
		below->x  =  shape.x;
		below->y += (shape.h + GAP);
		below->w  =  shape.w;
		below->h -= (shape.h + GAP);
	}
}


#if 0
/**
 * draw_container - examples
 */
void
DPDrawingArea::draw_container2 (const Cairo::RefPtr<Cairo::Context> &cr, DPContainer *cont, Rect shape, Rect *right)
{
	if (!cont)
		return;

#if 0 //XXX icon + label above
	Rect inside;
	Rect below;

	fill_area (cr, shape);
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

	draw_box (cr, cont, shape, &inside);
	//draw_edge (cr, inside);

	cr->set_source_rgba (1.0, 1.0, 1.0, 1.0);
	draw_fill (cr, inside);

	draw_icon (cr, "disk", inside, &below);
#endif

#if 0 //XXX icon + shaded label below
	Rect inside;
	Rect below;

	shape.w = 2*TAB_WIDTH + 2*GAP;

	draw_box (cr, cont, shape, &inside);
	//draw_edge (cr, inside);

	cr->set_source_rgba (1.0, 1.0, 1.0, 1.0);
	draw_fill (cr, inside);

	draw_icon (cr, "disk", inside, &below);

	std::string labeld = m_c->device;
	std::string labels = get_size (m_c->bytes_size);

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

#if 0 //XXX get_pixel_size
	int tw = 0;
	int th = 0;
	layout->get_pixel_size (tw, th);
	printf ("text width = %d, height = %d\n", tw, th);
#endif
}

/**
 * get_protective
 */
Table *
DPDrawingArea::get_protective (DPContainer *c)
{
	DPContainer *child = nullptr;

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
		return dynamic_cast<Table*> (child);

	return nullptr;
}

#endif

/**
 * draw_label - write some text into an area
 */
void
draw_label (const Cairo::RefPtr<Cairo::Context> &cr, DPContainer *cont, Rect shape)
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
		Filesystem *fs = dynamic_cast<Filesystem*> (cont);
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
 * draw_gradient - apply light shading to an area
 */
void
draw_gradient (const Cairo::RefPtr<Cairo::Context> &cr, Rect shape)
{
	const int &x = shape.x;
	const int &y = shape.y;
	const int &w = shape.w;
	const int &h = shape.h;

	//XXX needs clipping to rounded rectangle

	Cairo::RefPtr<Cairo::LinearGradient> grad;			// Gradient shading
	grad = Cairo::LinearGradient::create (0.0, 0.0, 0.0, h);
	grad->add_color_stop_rgba (0.00, 0.0, 0.0, 0.0, 0.2);
	grad->add_color_stop_rgba (0.50, 0.0, 0.0, 0.0, 0.0);
	grad->add_color_stop_rgba (1.00, 0.0, 0.0, 0.0, 0.1);
	cr->set_source (grad);
	cr->rectangle (x, y, w, h);
	cr->fill();
}

/**
 * draw_corner - draw a corner ne/nw/se/sw convex/concave
 */
void draw_corner (const Cairo::RefPtr<Cairo::Context> &cr, Rect shape, bool north, bool east, bool convex)
{
	const int &r = RADIUS;
	const int &x = shape.x;
	const int &y = shape.y;
	const int &w = shape.w;
	const int &h = shape.h;

	cr->save();
	cr->move_to (x, y);
	cr->rel_line_to (w, 0);
	cr->rel_line_to (0, h);
	cr->rel_line_to (-w, 0);
	cr->rel_line_to (0, -h);
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
 * draw_fill - fill in a rounded rectangle
 */
void
DPDrawingArea::draw_fill (const Cairo::RefPtr<Cairo::Context> &cr, const Rect &shape)
{
	const int &r = RADIUS;
	const int &x = shape.x;
	const int &y = shape.y;
	const int &w = shape.w;
	const int &h = shape.h;

	//std::cout << shape << std::endl;

	draw_corner (cr, shape, true,  true,  true);
	draw_corner (cr, shape, true,  false, true);
	draw_corner (cr, shape, false, true,  true);
	draw_corner (cr, shape, false, false, true);

	cr->move_to     (x, y+r);
	cr->rel_line_to (r, 0);
	cr->rel_line_to (0, -r);
	cr->rel_line_to (w-(2*r), 0);
	cr->rel_line_to (0, r);
	cr->rel_line_to (r, 0);

	cr->rel_line_to (0, h-(2*r));

	cr->rel_line_to (-r, 0);
	cr->rel_line_to (0, r);
	cr->rel_line_to (-w+(2*r), 0);
	cr->rel_line_to (0, -r);
	cr->rel_line_to (-r, 0);
	cr->close_path();

	cr->fill();
}

/**
 * draw_arc - draw a corner line (90 degrees) se/sw
 */
void draw_arc (const Cairo::RefPtr<Cairo::Context> &cr, Rect shape, bool east)
{
	const int &r = RADIUS;
	const int &x = shape.x;
	const int &y = shape.y;
	const int &w = shape.w;
	const int &h = shape.h;

	cr->save();
	cr->set_line_width (r/4+1);

	if (east) {
		// South-East
		cr->move_to (x+w-r, y+h);
		cr->rel_line_to (0, -r);
		cr->rel_line_to (r, 0);
		cr->rel_line_to (0, r);
		cr->close_path();
		cr->clip();

		cr->arc (x+w-r-(r/4), y+h-r-(r/4), r+(r/8)+1, ARC_E, ARC_S);
	} else {
		// South-West
		cr->move_to (x, y+h);
		cr->rel_line_to (0, -r);
		cr->rel_line_to (r, 0);
		cr->rel_line_to (0, r);
		cr->close_path();
		cr->clip();

		cr->arc (x+r+(r/4), y+h-r-(r/4), r+(r/8)+1, ARC_S, ARC_W);
	}

	cr->stroke();
	cr->restore();
}

/**
 * draw_block - draw an icon-width, hollow, rounded rectangle
 */
void
DPDrawingArea::draw_block (const Cairo::RefPtr<Cairo::Context> &cr, DPContainer *cont, const Rect &shape, Rect *tab, Rect *right)
{
	if (!cont)
		return;

	if (shape.h < (RADIUS*2)) {
		log_info ("draw_tab: too short\n");
		return;
	}

	if (shape.w < (BLOCK_WIDTH + (RADIUS*2))) {
		log_info ("draw_tab: too narrow\n");
		return;
	}

	Rect work = shape;

	work.w = BLOCK_WIDTH + (RADIUS/2);

	const int &x = work.x;
	const int &y = work.y;
	const int &w = work.w;
	const int &h = work.h;

	vRange.push_front ({work, cont});			// Associate a region with a container

	draw_corner (cr, work, true,  false, true);		// Top left corner (1)
	draw_corner (cr, work, true,  true,  true);		// Top right corner (3)

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

	draw_arc (cr, work, true);				// Thin bottom right corner (12)
	draw_arc (cr, work, false);				// Thin bottom left corner (13)

	Rect t = { x+SIDES, y+RADIUS, BLOCK_WIDTH, h-RADIUS-(SIDES*1) };

	draw_corner (cr, t, true,  true,  false);		// Tab inside top right corner
	draw_corner (cr, t, true,  false, false);		// Tab inside top left corner


	if (tab)
		*tab = t;

	if (right)
		*right = { shape.x + work.w + GAP, shape.y, shape.w - work.w - GAP, shape.h};
}

/**
 * draw_tabbox - draw a rounded rectangle with a handy tab
 */
void
DPDrawingArea::draw_tabbox (const Cairo::RefPtr<Cairo::Context> &cr, DPContainer *cont, const Rect &shape, Rect *tab, Rect *inside)
{
	if (!cont)
		return;

	if (shape.h < (RADIUS*2)) {
		log_info ("draw_tabbox: too short\n");
		return;
	}

	if (shape.w < (TAB_WIDTH + (RADIUS*2))) {
		log_info ("draw_tabbox: too narrow\n");
		return;
	}

	Rect work = shape;

	const int &x = work.x;
	const int &y = work.y;
	const int &w = work.w;
	const int &h = work.h;

	vRange.push_front ({work, cont});			// Associate a region with a container

	draw_corner (cr, work, true,  false, true);		// Top left corner (1)

	draw_corner (cr, work, true,  true,  true);		// Top right corner (3)

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

	draw_arc (cr, work, true);				// Thin bottom right corner (12)

	draw_corner (cr, shape, false, false, true);		// Bottom left corner (9)

	cr->move_to (x+SIDES, y+RADIUS);			// Tab block (5)
	cr->rel_line_to (TAB_WIDTH+SIDES, 0);
	cr->rel_line_to (0, h-RADIUS-SIDES);
	cr->rel_line_to (-TAB_WIDTH+RADIUS-(SIDES*2), 0);
	cr->rel_line_to (0, -RADIUS+SIDES);
	cr->rel_line_to (-RADIUS+SIDES, 0);
	cr->close_path();
	cr->fill();

	Rect i = { x+TAB_WIDTH+(SIDES*2), y+RADIUS, w-TAB_WIDTH-(SIDES*3), h-RADIUS-SIDES };

	draw_corner (cr, i, false, false, false);		// Bottom left inner corner (11)

	Rect t = { x+SIDES, y+RADIUS, TAB_WIDTH, h-RADIUS-(SIDES*1) };

	draw_corner (cr, i, true, false, false);		// Top left inner corner (6)
	draw_corner (cr, i, true, true,  false);		// Top right inner corner (7)

	if (tab)
		*tab = t;
	if (inside)
		*inside = i;
}

/**
 * draw_box - draw a rounded rectangle
 */
void
DPDrawingArea::draw_box (const Cairo::RefPtr<Cairo::Context> &cr, DPContainer *cont, const Rect &shape, Rect *inside)
{
	if (!cont)
		return;

	if (shape.h < (RADIUS*2)) {
		log_info ("draw_box: too short\n");
		return;
	}

	if (shape.w < (TAB_WIDTH + (RADIUS*2))) {
		log_info ("draw_box: too narrow\n");
		return;
	}

	Rect work = shape;

	const int &x = work.x;
	const int &y = work.y;
	const int &w = work.w;
	const int &h = work.h;

	vRange.push_front ({work, cont});			// Associate a region with a container

	draw_corner (cr, work, true,  false, true);		// Top left corner (1)

	draw_corner (cr, work, true,  true,  true);		// Top right corner (3)

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

	draw_arc (cr, work, true);				// Thin bottom right corner (12)
	draw_arc (cr, work, false);				// Thin bottom left corner (13)

	Rect i = { x+SIDES, y+RADIUS, w-(SIDES*2), h-RADIUS-SIDES };

	draw_corner (cr, i, true, false, false);		// Top left inner corner (6)
	draw_corner (cr, i, true, true,  false);		// Top right inner corner (7)

	if (inside)
		*inside = i;
}

/**
 * draw_container - recursively draw a set of containers
 */
void
DPDrawingArea::draw_container (const Cairo::RefPtr<Cairo::Context> &cr, DPContainer *cont, Rect shape)
{
	Rect tab;
	Rect inside;

	if (!cont)
		return;

	//printf ("object = %s (%s)\n", cont->name.c_str(), cont->uuid.c_str());
	if (shape.w < TAB_WIDTH) {
		printf ("too narrow\n");
		return;
	}

	std::string path = cont->get_path();
	std::string name = cont->name;

	std::string display    = theme->get_config (path, name, "display");
	std::string handle     = theme->get_config (path, name, "handle");
	std::string box        = theme->get_config (path, name, "box");
	std::string colour     = theme->get_config (path, name, "colour");

#if 0
	std::string size       = theme->get_config (path, name, "size");
	std::string background = theme->get_config (path, name, "background");
	std::string label      = theme->get_config (path, name, "label");
	std::string icon       = theme->get_config (path, name, "icon");
#endif

	Gdk::RGBA rgba ("black");
	if (!rgba.set (colour.c_str())) {
		log_error ("don't understand colour: %s\n", colour.c_str());
		//XXX validate (and default) any colours in theme.cpp
	}

	cr->set_source_rgba (rgba.get_red(), rgba.get_green(), rgba.get_blue(), rgba.get_alpha());

	//log_info ("theme: %s\n", path.c_str());
	//std::cout << shape << std::endl;
	//log_info ("%-12s: D: %-5s, H: %-5s, B: %-5s, C: %s\n", cont->name.c_str(), display.c_str(), handle.c_str(), box.c_str(), colour.c_str());

	if (display == "true") {
		shape.x += 1;
		shape.w -= 2;
		shape.y += 1;
		shape.h -= 2;

		if (handle == "true") {
			if (box == "true") {
				draw_tabbox (cr, cont, shape, &tab, &inside);
			} else {
				draw_block (cr, cont, shape, &tab, &inside);
				//draw_edge (cr, inside);
#if 0
				cr->set_source_rgba (1.0, 0.0, 0.0, 1.0);
				draw_icon (cr, "table", tab, &tab);
				draw_icon (cr, "warning", tab, &tab);
#endif
			}
		} else {
			draw_box (cr, cont, shape, &inside);
			if (theme->get_config (path, name, "usage") == "true") {
#if 1
				Rect usage = inside;
				usage.w = usage.w * 2 / 3;
				cr->set_source_rgba (0.97, 0.97, 0.42, 1.0);
				draw_fill (cr, usage);
#endif
			}
			draw_gradient (cr, inside);
			draw_label (cr, cont, inside);
		}
	} else {
		inside = shape;
	}

	//draw_edge (cr, inside);

#if 0
	if (shape.h == 3*77-2) {
		std::string value;
		std::string attr;

		attr = "display";
		value = theme->get_config (path, attr);
		log_info ("%s.%s = %s\n", path.c_str(), attr.c_str(), value.c_str());

		attr = "handle";
		value = theme->get_config (path, attr);
		log_info ("%s.%s = %s\n", path.c_str(), attr.c_str(), value.c_str());

		attr = "box";
		value = theme->get_config (path, attr);
		log_info ("%s.%s = %s\n", path.c_str(), attr.c_str(), value.c_str());

		//box = false;
	}
#endif

#if 0
	//Rect right;
	draw_box (cr, cont, shape, &inside);
	//draw_focus (cr, shape);
	draw_highlight (cr, shape);
#endif
#if 0
	cr->set_source_rgba (1.0, 1.0, 1.0, 0.6);
	draw_fill (cr, tab);
	draw_edge (cr, tab);
#endif
#if 0
	cr->set_source_rgba (1.0, 1.0, 1.0, 0.6);
	draw_fill (cr, inside);
	draw_edge (cr, inside);
	//draw_box (cr, cont, shape, &inside, &tab);
#endif
#if 0
	cr->set_source_rgba (1.0, 0.0, 0.0, 1.0);
	draw_fill (cr, tab);
	Rect below;
	draw_icon (cr, "margin_black", tab, &below);
	tab = below;
	draw_icon (cr, "table", tab, &below);
	//draw_edge (cr, tab);
	cr->set_source_rgba (1.0, 1.0, 1.0, 1.0);
	draw_fill (cr, inside);
	//draw_edge (cr, inside);
#endif

	// width = w
	// children = c
	// tabs = t
	// gaps = c-1
	// big_icon = 0

	//long count = cont->children.size();
	//printf ("children = %ld\n", count);
	//printf ("width    = %d\n",  shape.w);

	//if (shape.h < 65) return;

	long total = cont->bytes_size;
	long bpp   = total / inside.w;	// bytes per pixel

	//printf ("\ttotal = %ld\n", total);
	//printf ("\tbpp   = %ld\n", bpp);

	for (auto c : cont->children) {
		long offset = c->parent_offset / bpp;
		long size   = c->bytes_size    / bpp;

		//printf ("\t\toffset = %ld\n", offset);
		//printf ("\t\tsize   = %ld\n", size);

		Rect next = inside;
		next.x += offset;
		next.w = size;

		draw_container(cr, c, next);
	}
}


/**
 * on_draw
 */
bool
DPDrawingArea::on_draw (const Cairo::RefPtr<Cairo::Context> &cr)
{
	//LOG_TRACE;
	if (!m_c)
		return true;

	vRange.clear();

	Gtk::Allocation allocation = get_allocation();

	Rect shape { 0, 0, allocation.get_width(), allocation.get_height() };

	//checker_area (cr, shape);
	fill_area (cr, shape);
	draw_container (cr, m_c, shape);

	return true;
}


/**
 * set_data
 */
void
DPDrawingArea::set_data (DPContainer *c)
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
 * get_theme
 */
bool
DPDrawingArea::get_theme (const std::string &object, const std::string &attr)
{
	//map of obj.attr => value
	//read from file
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
	return false;
}

/**
 * get_focus
 */
bool
DPDrawingArea::get_focus (int &x, int &y, int &w, int &h)
{
	if ((sel_x < 0) || (sel_y < 0))
		return false;

	bool b = false;
#if 0
	for (auto it = vRange.begin(); it != vRange.end(); it++) {
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

#endif
	return b;
}

#endif

#if 0
/**
 * on_mouse_motion
 */
bool
DPDrawingArea::on_mouse_motion (GdkEventMotion *event)
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
DPDrawingArea::on_mouse_leave (GdkEventCrossing *event)
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
DPDrawingArea::on_mouse_click (GdkEventButton *event)
{
	//std::cout << "mouse click: (" << event->x << "," << event->y << ")\n";

	sel_x = event->x;
	sel_y = event->y;

#if 0
	for (auto it = vRange.begin(); it != vRange.end(); it++) {
		bool b = ((event->x >= (*it).x) && (event->x < ((*it).x + (*it).w)) &&
			  (event->y >= (*it).y) && (event->y < ((*it).y + (*it).h)));
		if (b) log_error ("Range: %d,%d %d,%d %p\n", (*it).x, (*it).y, (*it).w, (*it).h, (*it).p);
		else   log_info  ("Range: %d,%d %d,%d %p\n", (*it).x, (*it).y, (*it).w, (*it).h, (*it).p);
	}
	log_info ("\n");
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

#endif

#if 0
/**
 * draw_grid - fine mesh grid
 */
void
DPDrawingArea::draw_grid (const Cairo::RefPtr<Cairo::Context> &cr)
{
	Gtk::Allocation allocation = get_allocation();
	int width  = allocation.get_width();
	int height = allocation.get_height();

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
DPDrawingArea::draw_grid_linear (const Cairo::RefPtr<Cairo::Context> &cr, Rect space, long max_size)
{
	space.w -= 2;

	const int &x = space.x;
	const int &w = space.w;
	const int &h = space.h;

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
DPDrawingArea::draw_grid_log (const Cairo::RefPtr<Cairo::Context> &cr, Rect space, long max_size)
{
}

#endif
