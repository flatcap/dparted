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

const double ARC_N = 3*M_PI_2;		// Compass points in radians
const double ARC_E = 0;
const double ARC_S = M_PI_2;
const double ARC_W = M_PI;

const int GAP       = 3;		// Space between partitions
const int RADIUS    = 8;		// Curve radius in disk gui
const int TAB_WIDTH = 26;		// Left side-bar in disk gui

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
	sigc::connection conn = Glib::signal_timeout().connect (my_slot, 800); // ms
#endif
	//std::cout << "GType name: " << G_OBJECT_TYPE_NAME (gobj()) << "\n";

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
bool DPDrawingArea::on_timeout (int timer_number)
{
	std::cout << "timer" << "\n";
	get_window()->invalidate (false); //RAR everything for now
	//return (m_c->device == "/dev/sdc");
	return false;
}

/**
 * draw_rect
 */
void DPDrawingArea::draw_rect (const Cairo::RefPtr<Cairo::Context> &cr, const std::string &colour, const Rect &shape)
{
	Gdk::RGBA c = theme->get_colour (colour);

	cr->set_source_rgba (c.get_red(), c.get_green(), c.get_blue(), c.get_alpha());
	cr->rectangle (shape.x, shape.y, shape.w, shape.h);
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


//XXX move these to respective classes?
/**
 * get_table
 * Am I a table?
 * Am I a wrapper for a table?
 */
Table * DPDrawingArea::get_table (DPContainer *c)
{
	DPContainer *child = NULL;

	if (!c)
		return NULL;

	if (c->is_a ("table"))
		return dynamic_cast<Table*> (c);

	if (c->children.size() != 1)
		return NULL;

	child = c->children[0];
	if (!child)
		return NULL;

	if (child->is_a ("table"))
		return dynamic_cast<Table*> (child);

	return NULL;
}

/**
 * get_part
 * Am I a partition?
 */
Partition * DPDrawingArea::get_partition (DPContainer *c)
{
	if (!c)
		return NULL;

	if (c->is_a ("partition"))
		return dynamic_cast<Partition*> (c);

	return NULL;
}

/**
 * get_protective
 */
Table * DPDrawingArea::get_protective (DPContainer *c)
{
	DPContainer *child = NULL;

	//std::cout << "1: " << c << "\n";
	if (!c)
		return NULL;

	if (!c->is_a ("table"))
		return NULL;

	if (c->children.size() != 1)
		return NULL;

	child = c->children[0];
	//std::cout << "2: " << child << "\n";
	if (!child)
		return NULL;

	if (!child->is_a ("partition"))
		return NULL;

	if (child->children.size() != 1)
		return NULL;

	child = child->children[0];
	//std::cout << "3: " << child << "\n";
	if (!child)
		return NULL;

	if (child->is_a ("table"))
		return dynamic_cast<Table*> (child);

	return NULL;
}

/**
 * get_filesystem
 * Am I a filesystem?
 * Am I a wrapper for a filesystem?
 */
Filesystem * DPDrawingArea::get_filesystem (DPContainer *c)
{
	DPContainer *child = NULL;

	if (!c)
		return NULL;

	if (c->is_a ("filesystem"))
		return dynamic_cast<Filesystem*> (c);

	if (c->children.size() != 1)
		return NULL;

	child = c->children[0];
	if (!child)
		return NULL;

	if (child->is_a ("filesystem"))
		return dynamic_cast<Filesystem*> (child);

	return NULL;
}

/**
 * get_misc
 */
Misc * DPDrawingArea::get_misc (DPContainer *c)
{
	DPContainer *child = NULL;

	if (!c)
		return NULL;

	if (c->children.size() != 1)
		return NULL;

	child = c->children[0];
	if (!child)
		return NULL;

	if (child->is_a ("misc"))
		return dynamic_cast<Misc*> (child);

	return NULL;
}


/**
 * draw_icon
 */
void DPDrawingArea::draw_icon (const Cairo::RefPtr<Cairo::Context> &cr, const std::string &name, Rect &shape, Rect *below /*=NULL*/)
{
	Glib::RefPtr<Gdk::Pixbuf> pb;

	if (below)
		*below = shape;

	pb = theme->get_icon (name);
	if (!pb) {
		return;
	}

	Gdk::Cairo::set_source_pixbuf (cr, pb, shape.x, shape.y);
	shape.w = pb->get_width();
	shape.h = pb->get_height();
	//log_info ("icon %d,%d\n", shape.w, shape.h);

	cr->rectangle (shape.x, shape.y, shape.w, shape.h);
	cr->fill();

	if (below) {
		below->x  =  shape.x;
		below->y += (shape.h + GAP);
		below->w  =  shape.w;
		below->h -= (shape.h + GAP);
	}
}

/**
 * draw_block
 */
void DPDrawingArea::draw_block (const Cairo::RefPtr<Cairo::Context> &cr, DPContainer *c, Rect &space, Rect *right /*=NULL*/)
{
	// adjust the rect to match
	std::string name;
	if      (m_c->is_a ("loop")) name = "loop";
	else if (m_c->is_a ("disk")) name = "disk";
	else if (m_c->is_a ("file")) name = "file";
	else                         name = "network";

	Rect below = space;
	draw_icon (cr, name, space, &below);

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

	if (right) {
		right->x += (space.w + GAP);
		right->w -= (space.w + GAP);
	}
}

/**
 * draw_border
 */
void DPDrawingArea::draw_border (const Cairo::RefPtr<Cairo::Context> &cr, const Rect &shape, Rect *inside /*=NULL*/, Rect *right /*=NULL*/)
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

	if (right) {			// Region after shape
		right->x += w;
		right->w -= w;
	}
}

/**
 * draw_focus
 */
void DPDrawingArea::draw_focus (const Cairo::RefPtr<Cairo::Context> &cr, const Rect &shape)
{
	std::vector<double> dashes;
	dashes.push_back (5);
	dashes.push_back (5);

	cr->save();
	draw_border (cr, shape);				// Set clipping area
	cr->clip();

	cr->set_line_width (4);

	cr->set_dash (dashes, 0);
	cr->set_source_rgba (0, 0, 0, 1.0);			//RAR focus colours from theme
	draw_border (cr, shape);
	cr->close_path();
	cr->stroke();

	cr->set_dash (dashes, 5);
	cr->set_source_rgba (1, 1, 1, 1.0);
	draw_border (cr, shape);
	cr->close_path();
	cr->stroke();

	cr->restore();						// End clipping
}

/**
 * draw_frame
 */
void DPDrawingArea::draw_frame (const Cairo::RefPtr<Cairo::Context> &cr, const Gdk::RGBA &colour, const Rect &shape, Rect *inside /*=NULL*/, Rect *right /*=NULL*/)
{
	const int r  = 8;					// Radius
	const int &x = shape.x;
	const int &y = shape.y;
	const int &w = shape.w;
	const int &h = shape.h;

	cr->save();
	draw_border (cr, shape);				// Set clipping area
	cr->clip();

	cr->set_source_rgba (colour.get_red(), colour.get_green(), colour.get_blue(), colour.get_alpha());

	cr->set_line_width (r);					// Thick top bar
	cr->move_to (x, y+ (r/2));
	cr->rel_line_to (w, 0);
	cr->stroke();

	cr->set_line_width (2);					// Thin side bars
	cr->move_to (x+1, y+r);
	cr->rel_line_to (0, h - (2*r));
	cr->move_to (x+w-1, y+r);
	cr->rel_line_to (0, h - (2*r));
	cr->stroke();

	cr->set_line_width (4);					// Thin bottom bar
	cr->arc (x+w-r, y+h-r, r, ARC_E, ARC_S);
	cr->arc (x+  r, y+h-r, r, ARC_S, ARC_W);
	cr->stroke();

	cr->restore();						// End clipping

	Rect rect = { x, y, w, h };
	Range rg = { rect, NULL };
	vRange.push_front (rg);

	if (inside) {						// The space remaining inside
		inside->x = x + 2;
		inside->y = y + r;
		inside->w = w - 4;
		inside->h = h - r - 2;
	}

	if (right) {						// The space after the shape
		right->x += w;
		right->w -= w;
	}
}

/**
 * draw_tabframe
 */
void DPDrawingArea::draw_tabframe (const Cairo::RefPtr<Cairo::Context> &cr, const std::string &colour, const Rect &shape, Rect *tab /*=NULL*/, Rect *inside /*=NULL*/, Rect *right /*=NULL*/)
{
	const int &r = RADIUS;
	const int &t = TAB_WIDTH;
	const int &x = shape.x;
	const int &y = shape.y;
	const int &w = shape.w;
	const int &h = shape.h;

	cr->save();
	draw_border (cr, shape);				// Set clipping area
	cr->clip();

	Gdk::RGBA c= theme->get_colour ("extended");
	cr->set_source_rgba (c.get_red(), c.get_green(), c.get_blue(), c.get_alpha());

	cr->set_line_width (r);					// Thick top bar
	cr->move_to (x, y+4);
	cr->rel_line_to (w, 0);
	cr->stroke();

	cr->set_line_width (t);					// Thick side bar
	cr->move_to (x + (t/2), y+r);
	cr->rel_line_to (0, h-r);
	cr->stroke();

	cr->move_to (x+t, y+r);					// Curvy inner corners
	cr->arc (x+t+r, y + (2*r), r, ARC_W, ARC_N);
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

	Rect rect = { x, y, w, h };
	Range rg = { rect, NULL };
	vRange.push_front (rg);

	if (tab) {
		tab->x = shape.x + 2;
		tab->y = shape.y + 4;
		tab->w = t;
		tab->h = h - r - 2;
	}

	if (inside) {						// The space remaining inside
		inside->x = x + t;
		inside->y = y + r;
		inside->w = w - t - 2;
		inside->h = h - r - 2;
	}

	if (right) {
		right->x += w;
		right->w -= w;
	}
}

/**
 * draw_partition
 */
void DPDrawingArea::draw_partition (const Cairo::RefPtr<Cairo::Context> &cr,
				    const std::string &colour,
				    int width_part, int width_fs, int width_usage,
				    Rect shape, Rect *inside /*=NULL*/, Rect *right /*=NULL*/)
{
	const int r = RADIUS;
	const int &x = shape.x;
	const int &y = shape.y;
	const int &w = shape.w;
	const int &h = shape.h;

	if (right)
		*right = shape;

	shape.w = width_part;

	cr->save();
	draw_border (cr, shape);					// Set clipping area
	cr->clip();

	Rect r_yellow = { x, y+r, width_usage, h - r };
	Rect r_white  = { x + width_usage, y + r, width_fs - width_usage, h - r };

	draw_rect (cr, "used",   r_yellow);
	draw_rect (cr, "unused", r_white);

	if ((width_fs - width_usage) > 1) {
		cr->set_source_rgba (1, 1, 1, 1);
		cr->move_to (x+width_usage, y+r);			// Curvy corners
		cr->arc (x+width_usage-r, y + (2*r), r, ARC_N, ARC_E);
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

	Gdk::RGBA c= theme->get_colour (colour);
	cr->set_source_rgba (c.get_red(), c.get_green(), c.get_blue(), c.get_alpha());

	cr->set_line_width (r);						// Thick top bar
	cr->move_to (x, y+4);
	cr->rel_line_to (w, 0);
	cr->stroke();

	cr->set_line_width (4);
	draw_border (cr, shape);
	cr->stroke();

	cr->restore();							// End clipping

	Rect rect = { x, y, w, h };
	Range rg = { rect, NULL };
	vRange.push_front (rg);

	if (inside) {							// The area remaining inside
		inside->x = shape.x + 2;
		inside->y = shape.y + r + 2;
		inside->w = w - 4;
		inside->h = shape.h - r - 4;
	}

	if (right) {							// The area to the right
		right->x += (w + GAP);
		right->y  = y;
		right->w -= (w + GAP);
		right->h  = h;
	}
}

/**
 * draw_container
 */
void DPDrawingArea::draw_container (const Cairo::RefPtr<Cairo::Context> &cr, DPContainer *c, Rect shape, Rect *right /*=NULL*/)
{
	const int &x = shape.x;
	const int &y = shape.y;
	const int &w = shape.w;
	const int &h = shape.h;
	Rect inside;

	if (!c)
		return;

	//log_debug ("%s\n", __PRETTY_FUNCTION__);
	if (right)
		*right = shape;

	shape.y += 1;
	shape.w -= GAP;
	shape.h -= GAP;

	//XXX assert (w > 0)

	long bytes_per_pixel = c->bytes_size / w;

	std::cout << "draw_container: " << c << "\n";
	for (auto child : c->children) {
		std::cout << "child: " << child << "\n";

		int child_width = (child->bytes_size / bytes_per_pixel);
		int child_usage = (child->bytes_used / bytes_per_pixel);

		int offset = x + (child->parent_offset / bytes_per_pixel);

		Filesystem *fs = get_filesystem (child);
		if (fs) {
			std::cout << "fs: " << fs << "\n";

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

			if (fs) {
				child_usage = fs->bytes_used / bytes_per_pixel;

				if (!fs->label.empty()) {
					label1 += " \"" + fs->label + "\"";
				}
				label2 = get_size (fs->bytes_size);
			}
			Rect rect = { offset, y, child_width-GAP, h };

			//log_debug ("rect: %d, %d, %d, %d\n", rect.x, rect.y, rect.w, rect.h);
			//log_debug ("%s: %d, %d, %d\n", fs->name.c_str(), child_width, child_width-GAP, child_usage);
			draw_partition (cr, fs->name, child_width-GAP, child_width-GAP, child_usage, rect, &inside);

			Pango::FontDescription font;
			font.set_family ("Liberation Sans");

			Glib::RefPtr<Pango::Layout> layout = Pango::Layout::create (cr);
			layout->set_font_description (font);

			layout->set_text (label1 + "\n" + label2);
			cr->set_source_rgba (0.0, 0.0, 0.0, 1.0);
			cr->move_to (inside.x + 2, inside.y + 2);
			layout->set_width (Pango::SCALE * (inside.w - 4));
			layout->set_ellipsize (Pango::ELLIPSIZE_END);
			layout->update_from_cairo_context (cr);
			layout->show_in_cairo_context (cr);
			continue;
		}

		Table *table = get_table (child);
		if (table) {
			//log_debug ("TABLE\n");
			//std::cout << table << "\n";
			DPContainer *protect = get_protective (table);
			std::string name;
			//log_info ("protect = %p\n", protect);
			if (protect) {
				name = protect->name;
			} else {
				name = table->name;
			}

			Rect tab;
			Rect inside;
			Rect right_t;
			Rect below;

			Rect rect = { offset, y, child_width-GAP, h };
			draw_tabframe (cr, "extended", rect, &tab, &inside, &right_t);

			if (protect) {
				draw_icon (cr, "shield", tab, &below);
				tab = below;
				child = protect;
			} else {
				child = table;
			}
			draw_icon (cr, "table", tab);

			draw_container (cr, child, inside);
			continue;
		}

		Partition *part = get_partition (child);
		if (part) {
			//log_debug ("PARTITION\n");
			//std::cout << part << "\n";

			Rect tab;
			Rect inside;
			Rect right_t;
			//Rect below;

			Rect rect = { offset, y, child_width-GAP, h };
			draw_tabframe (cr, "extended", rect, &tab, &inside, &right_t);

			draw_icon (cr, "table", tab);

			draw_container (cr, part, inside);
			continue;
		}

		Misc *m = get_misc (child);
		if (!m && (child->is_a ("misc"))) {
			m = dynamic_cast<Misc*> (child);	// don't need this, it's in get_misc
		}

		if (m) {
			//log_debug ("Other: %s (%s)\n", m->name.c_str(), __PRETTY_FUNCTION__);
			if ((child_width-GAP) < 10)
				continue;
			Rect rect = { offset, y, child_width-GAP, h };
			if (m->name == "unallocated") {
				draw_partition (cr, "unallocated", child_width-GAP, child_width-GAP, 0, rect, &inside);
			} else if (m->name == "empty") {
				draw_border (cr, rect, &inside);
			} else {
				draw_partition (cr, "unknown", child_width-GAP, child_width-GAP, 0, rect, &inside);
			}

			Pango::FontDescription font;
			font.set_family ("Liberation Sans");

			Glib::RefPtr<Pango::Layout> layout = Pango::Layout::create (cr);
			layout->set_font_description (font);

			layout->set_text (m->name + "\n" + get_size (child->bytes_size));
			cr->set_source_rgba (0.0, 0.0, 0.0, 1.0);
			cr->move_to (inside.x + 2, inside.y + 2);
			layout->set_width (Pango::SCALE * (child_width - GAP - 8));
			layout->set_ellipsize (Pango::ELLIPSIZE_END);
			layout->update_from_cairo_context (cr);
			layout->show_in_cairo_context (cr);
			continue;
		}

		std::string s = get_size (child->bytes_size);
		//log_error ("unknown type %s, %s, %s\n", child->name.c_str(), child->device.c_str(), s.c_str());
	}

	if (right) {
		right->x += (w + GAP);
		right->w -= (w + GAP);
	}
}

/**
 * on_draw
 */
bool DPDrawingArea::on_draw (const Cairo::RefPtr<Cairo::Context> &cr)
{
	//log_debug ("%s\n", __PRETTY_FUNCTION__);
	if (!m_c)
		return true;

	DPContainer *item = m_c;

	//if (item->device != "/dev/loop3") return true;
	//std::cout << std::endl;

	vRange.clear();

	Gtk::Allocation allocation = get_allocation();

	Rect space = { 0, 0, allocation.get_width(), allocation.get_height() };
	Rect right = space;
	Rect inside;
	Rect below;

	draw_block (cr, item, space, &right);
	space = right;

	Table *table = get_table (item);
	DPContainer *protect = get_protective (table);
	DPContainer *partition = protect ? protect->parent : NULL;

#if 0
	std::cout << "item: "      << item      << "\n";
	std::cout << "table: "     << table     << "\n";
	std::cout << "protect: "   << protect   << "\n";
	std::cout << "partition: " << partition << "\n";
#endif

	// block (table)
	// block (table, shield)
	if (table) {
		draw_partition (cr, "red", 27, 27, 0, space, &inside, &right); // 27 -> width of icon + GAP, 0  -> usage (yellow)
		draw_icon (cr, "table", inside, &below);
		if (protect) {
			draw_icon (cr, "shield", below);
			inside = below;
			item = protect;
		}
		space = right;
	}

	draw_grid_linear (cr, space, m_c->bytes_size);

	if (partition) {
		//reduce space and delegate
		long bytes_per_pixel = table->bytes_size / space.w;
		space.x = partition->parent_offset / bytes_per_pixel;
		space.w = partition->bytes_size    / bytes_per_pixel;
		item = partition;
	} else if (table) {
		item = table;
	}

	draw_container (cr, item, space);

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


/**
 * set_data
 */
void DPDrawingArea::set_data (DPContainer *c)
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
 * draw_grid
 */
void DPDrawingArea::draw_grid (const Cairo::RefPtr<Cairo::Context> &cr)
{
	Gtk::Allocation allocation = get_allocation();
	int width  = allocation.get_width();
	int height = allocation.get_height();

	cr->save();

	cr->set_antialias (Cairo::ANTIALIAS_NONE);
	cr->set_source_rgba (1, 0, 0, 0.2);
	cr->set_line_width (1);
	cr->rectangle (0.5, 0.5, width-1, height-1);
	cr->stroke();

	for (int i = 10; i < width; i += 10) {
		if ((i % 100) == 0) {
			cr->set_source_rgba (0, 0, 1, 0.2);
		} else {
			cr->set_source_rgba (0, 1, 0, 0.3);
		}
		cr->move_to (i + 0.5, 0);
		cr->rel_line_to (0, height);
		cr->stroke();
	}

	for (int i = 10; i < width; i += 10) {
		if ((i % 100) == 0) {
			cr->set_source_rgba (0, 0, 1, 0.2);
		} else {
			cr->set_source_rgba (0, 1, 0, 0.3);
		}
		cr->move_to (0, i + 0.5);
		cr->rel_line_to (width, 0);
		cr->stroke();
	}

	cr->restore();
}

/**
 * draw_grid_linear
 */
void DPDrawingArea::draw_grid_linear (const Cairo::RefPtr<Cairo::Context> &cr, Rect space, long long max_size)
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
	cr->set_source_rgba (1, 0, 0, 0.2);
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
void DPDrawingArea::draw_grid_log (const Cairo::RefPtr<Cairo::Context> &cr, Rect space, long long max_size)
{
}


/**
 * draw_highlight
 */
void DPDrawingArea::draw_highlight (const Cairo::RefPtr<Cairo::Context> &cr, const Rect &shape)
{
	cr->save();
	draw_border (cr, shape);
	cr->clip();
	draw_rect (cr, "rgba(0,128,0,0.2)", shape);
	cr->set_source_rgba (1, 0, 0, 1);
	draw_border (cr, shape);
	cr->stroke();
	cr->restore();
}

