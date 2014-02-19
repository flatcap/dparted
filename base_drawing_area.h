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

#ifndef _BASE_DRAWING_AREA_H_
#define _BASE_DRAWING_AREA_H_

#include <gtkmm/drawingarea.h>

#include "gfx_container.h"

typedef struct { int x, y, w, h; } Rect;		// x,y coords, width, height

const double ARC_N = 3*M_PI_2;	// Compass points in radians
const double ARC_E = 0*M_PI_2;
const double ARC_S = 1*M_PI_2;
const double ARC_W = 2*M_PI_2;

const int GAP         =   3;	// Space between partitions
const int RADIUS      =   8;	// Curve radius of corners
const int SIDES       =   2;	// Width of sides and base
const int TAB_WIDTH   =  10;	// Space in left side-bar
const int BLOCK_WIDTH =  24;	// Placeholder for icons

class BaseDrawingArea : public Gtk::DrawingArea
{
public:
	BaseDrawingArea (void);
	~BaseDrawingArea();

	virtual void set_data (GfxContainerPtr& c);

	virtual void set_cont_height (int height);
	virtual int  get_cont_height (void);

protected:
	int cont_height = 70;

	GfxContainerPtr top_level;

	void checker_rect (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape, int check_size);
	void fill_area (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape, const Gdk::RGBA& colour);
	void set_colour (const Cairo::RefPtr<Cairo::Context>& cr, const Gdk::RGBA& rgba);
	void draw_border (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape);
	void escape_text (std::string &text);

	void draw_text (const Cairo::RefPtr<Cairo::Context>& cr, Rect shape, std::string text);
	void draw_icon (const Cairo::RefPtr<Cairo::Context>& cr, Glib::RefPtr<Gdk::Pixbuf> icon, const Rect& shape, Rect& below);
	void draw_iconbox (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape, Rect& tab, Rect& inside);
	void draw_box (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape, Rect& inside);
	void draw_corner (const Cairo::RefPtr<Cairo::Context>& cr, Rect shape, bool north, bool east, bool convex);
	void draw_arc (const Cairo::RefPtr<Cairo::Context>& cr, Rect shape, bool east);
	void draw_tabbox (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape, Rect& tab, Rect& inside);
	virtual bool on_draw (const Cairo::RefPtr<Cairo::Context>& cr);

};

#endif // _BASE_DRAWING_AREA_H_

