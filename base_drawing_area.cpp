/* Copyright (c) 2014 Richard Russon (FlatCap)
 *
 * This file is part of DParted.
 *
 * DParted is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DParted is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DParted.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <gdkmm.h>

#include "base_drawing_area.h"
#include "log.h"
#include "utils.h"

BaseDrawingArea::BaseDrawingArea (void)
{
	log_ctor ("ctor BaseDrawingArea");
}

BaseDrawingArea::~BaseDrawingArea()
{
	log_dtor ("dtor BaseDrawingArea");
}


void
BaseDrawingArea::set_data (GfxContainerPtr& gfx)
{
	top_level = gfx;
	//top_level->dump();

	gfx->add_listener (this);	//XXX when do listeners get removed?
}

void
BaseDrawingArea::set_cont_height (int height)
{
	cont_height = height;
}

int
BaseDrawingArea::get_cont_height (void)
{
	return cont_height;
}

void
BaseDrawingArea::escape_text (std::string &text)
{
	std::size_t pos = text.find_first_of ("<>");
	while (pos != std::string::npos) {
		if (text[pos] == '<') {
			text.replace (pos, 1, "&#60;");
		} else {
			text.replace (pos, 1, "&#62;");
		}
		pos = text.find_first_of ("<>", pos+2);
	}
}


bool
BaseDrawingArea::on_draw (const Cairo::RefPtr<Cairo::Context>& cr)
{
	return Gtk::DrawingArea::on_draw (cr);
}


/**
 * checker_rect - checker rect
 */
void
BaseDrawingArea::checker_rect (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape, int check_size)
{
	const int& s = check_size;
	const int& x = shape.x;
	const int& y = shape.y;
	const int& w = shape.w;
	const int& h = shape.h;

	for (int i = 0; i <= w; i += s) {
		for (int j = 0; j <= h; j += s) {
			if (((i+j)/s)&1) {
				cr->set_source_rgba (0.8, 0.8, 0.8, 1.0);
			} else {
				cr->set_source_rgba (0.6, 0.6, 0.6, 1.0);
			}
			cr->rectangle (x+i, y+j, s, s);
			cr->fill();
		}
	}
}

/**
 * draw_arc - draw a corner line (90 degrees) se/sw
 */
void
BaseDrawingArea::draw_arc (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape, bool east)
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
 * draw_border - sketch out curved rectangle
 */
void
BaseDrawingArea::draw_border (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape)
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
 * draw_box - draw a rounded rectangle
 */
void
BaseDrawingArea::draw_box (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape, Rect& inside)
{
	if (shape.h < (RADIUS*2)) {
		log_info ("draw_box: too short");
		return;
	}

	if (shape.w < (TAB_WIDTH + (RADIUS*2))) {
		log_info ("draw_box: too narrow");
		return;
	}

	const int& x = shape.x;
	const int& y = shape.y;
	const int& w = shape.w;
	const int& h = shape.h;

	draw_corner (cr, shape, true,  false, true);		// Top left corner(1)
	draw_corner (cr, shape, true,  true,  true);		// Top right corner(3)

	draw_arc (cr, shape, true);				// Thin bottom right corner (12)
	draw_arc (cr, shape, false);				// Thin bottom left corner (13)

	cr->set_line_width (RADIUS);				// Thick top bar(2)
	cr->move_to (x+RADIUS, y+(RADIUS/2));
	cr->rel_line_to (w-(2*RADIUS), 0);
	cr->stroke();

	cr->set_line_width (SIDES);				// Thin left bar(4)
	cr->move_to (x+(SIDES/2), y+RADIUS);
	cr->rel_line_to (0, h-(2*RADIUS));
	cr->stroke();

	cr->set_line_width (SIDES);				// Thin right bar(8)
	cr->move_to (x+w-(SIDES/2), y+RADIUS);
	cr->rel_line_to (0, h-(2*RADIUS));
	cr->stroke();

	cr->set_line_width (SIDES);				// Thin bottom bar (10)
	cr->move_to (x+RADIUS, y+h-(SIDES/2));
	cr->rel_line_to (w-(2*RADIUS), 0);
	cr->stroke();

	inside = { x+SIDES, y+RADIUS, w-(SIDES*2), h-RADIUS-SIDES };

	draw_corner (cr, inside, true, false, false);		// Top left inner corner(6)
	draw_corner (cr, inside, true, true,  false);		// Top right inner corner(7)
}

/**
 * draw_corner - solid ne/nw/se/sw convex/concave corner
 */
void
BaseDrawingArea::draw_corner (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape, bool north, bool east, bool convex)
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

void
BaseDrawingArea::draw_icon (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape, Glib::RefPtr<Gdk::Pixbuf> icon, Rect& below)
{
	return_if_fail (icon);

	Rect work = shape;

	cr->save();

	work.x += ((work.w - icon->get_width()) / 2);	// Centre the icon

	Gdk::Cairo::set_source_pixbuf (cr, icon, work.x, work.y);
	work.w = icon->get_width();
	work.h = icon->get_height();
	log_info ("icon %d,%d", work.w, work.h);

	cr->rectangle (work.x, work.y, work.w, work.h);
	cr->fill();
	cr->restore();
	//draw_edge (cr, work, "red");

	below = { shape.x, shape.y + (work.h + GAP), shape.w, shape.h - (work.h + GAP) };
}

/**
 * draw_iconbox - draw a rounded rectangle with a handy tab
 */
void
BaseDrawingArea::draw_iconbox (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape, Rect& tab, Rect& inside)
{
	if (shape.h < (RADIUS*2)) {
		log_info ("draw_iconbox: too short");
		return;
	}

	if (shape.w < (BLOCK_WIDTH + (RADIUS*2))) {
		log_info ("draw_iconbox: too narrow");
		return;
	}

	const int& x = shape.x;
	const int& y = shape.y;
	const int& w = shape.w;
	const int& h = shape.h;

	draw_corner (cr, shape, true,  false, true);		// Top left corner(1)
	draw_corner (cr, shape, true,  true,  true);		// Top right corner(3)
	draw_corner (cr, shape, false, false, true);		// Bottom left corner(9)

	draw_arc (cr, shape, true);				// Thin bottom right corner (12)

	cr->set_line_width (RADIUS);				// Thick top bar(2)
	cr->move_to (x+RADIUS, y+(RADIUS/2));
	cr->rel_line_to (w-(2*RADIUS), 0);
	cr->stroke();

	cr->set_line_width (SIDES);				// Thin left bar(4)
	cr->move_to (x+(SIDES/2), y+RADIUS);
	cr->rel_line_to (0, h-(2*RADIUS));
	cr->stroke();

	cr->set_line_width (SIDES);				// Thin right bar(8)
	cr->move_to (x+w-(SIDES/2), y+RADIUS);
	cr->rel_line_to (0, h-(2*RADIUS));
	cr->stroke();

	cr->set_line_width (SIDES);				// Thin bottom bar (10)
	cr->move_to (x+RADIUS, y+h-(SIDES/2));
	cr->rel_line_to (w-(2*RADIUS), 0);
	cr->stroke();

	cr->move_to (x+SIDES, y+RADIUS);			// Tab block(5)
	cr->rel_line_to (BLOCK_WIDTH+SIDES, 0);
	cr->rel_line_to (0, h-RADIUS-SIDES);
	cr->rel_line_to (-BLOCK_WIDTH+RADIUS-(SIDES*2), 0);
	cr->rel_line_to (0, -RADIUS+SIDES);
	cr->rel_line_to (-RADIUS+SIDES, 0);
	cr->close_path();
	cr->fill();

	inside = { x+BLOCK_WIDTH+(SIDES*2), y+RADIUS, w-BLOCK_WIDTH-(SIDES*3), h-RADIUS-SIDES };

	draw_corner (cr, inside, false, false, false);		// Bottom left inner corner (11)
	draw_corner (cr, inside, true,  false, false);		// Top left inner corner(6)
	draw_corner (cr, inside, true,  true,  false);		// Top right inner corner(7)

	tab = { x+SIDES, y+RADIUS, BLOCK_WIDTH, h-RADIUS-(SIDES*1) };
}

/**
 * draw_tabbox - draw a rounded rectangle with a handy tab
 */
void
BaseDrawingArea::draw_tabbox (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape, Rect& tab, Rect& inside)
{
	if (shape.h < (RADIUS*2)) {
		log_info ("draw_tabbox: too short");
		return;
	}

	if (shape.w < (TAB_WIDTH + (RADIUS*2))) {
		log_info ("draw_tabbox: too narrow");
		return;
	}

	const int& x = shape.x;
	const int& y = shape.y;
	const int& w = shape.w;
	const int& h = shape.h;

	draw_corner (cr, shape, true,  false, true);		// Top left corner(1)
	draw_corner (cr, shape, true,  true,  true);		// Top right corner(3)
	draw_corner (cr, shape, false, false, true);		// Bottom left corner(9)

	draw_arc (cr, shape, true);				// Thin bottom right corner (12)

	cr->set_line_width (RADIUS);				// Thick top bar(2)
	cr->move_to (x+RADIUS, y+(RADIUS/2));
	cr->rel_line_to (w-(2*RADIUS), 0);
	cr->stroke();

	cr->set_line_width (SIDES);				// Thin left bar(4)
	cr->move_to (x+(SIDES/2), y+RADIUS);
	cr->rel_line_to (0, h-(2*RADIUS));
	cr->stroke();

	cr->set_line_width (SIDES);				// Thin right bar(8)
	cr->move_to (x+w-(SIDES/2), y+RADIUS);
	cr->rel_line_to (0, h-(2*RADIUS));
	cr->stroke();

	cr->set_line_width (SIDES);				// Thin bottom bar (10)
	cr->move_to (x+RADIUS, y+h-(SIDES/2));
	cr->rel_line_to (w-(2*RADIUS), 0);
	cr->stroke();

	cr->move_to (x+SIDES, y+RADIUS);			// Tab block(5)
	cr->rel_line_to (TAB_WIDTH+SIDES, 0);
	cr->rel_line_to (0, h-RADIUS-SIDES);
	cr->rel_line_to (-TAB_WIDTH+RADIUS-(SIDES*2), 0);
	cr->rel_line_to (0, -RADIUS+SIDES);
	cr->rel_line_to (-RADIUS+SIDES, 0);
	cr->close_path();
	cr->fill();

	inside = { x+TAB_WIDTH+(SIDES*2), y+RADIUS, w-TAB_WIDTH-(SIDES*3), h-RADIUS-SIDES };

	draw_corner (cr, inside, false, false, false);		// Bottom left inner corner (11)
	draw_corner (cr, inside, true,  false, false);		// Top left inner corner(6)
	draw_corner (cr, inside, true,  true,  false);		// Top right inner corner(7)

	tab = { x+SIDES, y+RADIUS, TAB_WIDTH, h-RADIUS-(SIDES*1) };
}

/**
 * draw_text - write some text into an area
 */
void
BaseDrawingArea::draw_text (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape, std::string text)
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

	escape_text (text);
	layout->set_markup (text);

	int tw = 0;
	int th = 0;
	layout->get_pixel_size (tw, th);

	cr->set_source_rgba (0.0, 0.0, 0.0, 1.0);	//THEME - icon label colour

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
 * fill_area - fill rounded rectangle
 */
void
BaseDrawingArea::fill_area (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape, const Gdk::RGBA& colour)
{
	cr->save();
	set_colour (cr, colour);

	draw_border (cr, shape);

	cr->fill();
	cr->restore();
}

void
BaseDrawingArea::set_colour (const Cairo::RefPtr<Cairo::Context>& cr, const Gdk::RGBA& rgba)
{
	return_if_fail (cr);

	cr->set_source_rgba (rgba.get_red(), rgba.get_green(), rgba.get_blue(), rgba.get_alpha());
}


void
BaseDrawingArea::gfx_container_added (const GfxContainerPtr& cont, const GfxContainerPtr& parent)
{
	// LOG_TRACE;
	std::string c = "NULL";
	std::string p = "NULL";

	if (cont)   c = cont->name;
	if (parent) p = parent->name;

	log_debug ("AREA gfx_container_added: %s to %s", c.c_str(), p.c_str());
	get_window()->invalidate (false); // everything for now
}

void
BaseDrawingArea::gfx_container_busy (const GfxContainerPtr& UNUSED(cont), int UNUSED(busy))
{
	LOG_TRACE;
}

void
BaseDrawingArea::gfx_container_changed (const GfxContainerPtr& UNUSED(cont))
{
	LOG_TRACE;
}

void
BaseDrawingArea::gfx_container_deleted (const GfxContainerPtr& UNUSED(cont))
{
	LOG_TRACE;
}

void
BaseDrawingArea::gfx_container_resync (const GfxContainerPtr& UNUSED(cont))
{
	LOG_TRACE;
}


