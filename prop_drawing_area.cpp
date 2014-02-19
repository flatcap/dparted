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

#include "prop_drawing_area.h"
#include "gui_app.h"

PropDrawingArea::PropDrawingArea (void)
{
}

PropDrawingArea::~PropDrawingArea()
{
}


void
PropDrawingArea::set_data (GfxContainerPtr& c)
{
	BaseDrawingArea::set_data (c);
	set_size_request (500, cont_height/2);
}

bool
PropDrawingArea::on_draw (const Cairo::RefPtr<Cairo::Context>& cr)
{
	//return Gtk::DrawingArea::on_draw (cr);
	if (!top_level)
		return true;

	Gtk::Allocation allocation = get_allocation();

	Rect shape { 0, 0, allocation.get_width(), allocation.get_height() };

#if 1
	checker_rect (cr, shape, 5);
#else
	draw_grid (cr, shape);
	draw_grid_linear (cr, shape, m_c->bytes_size);
	fill_rect (cr, shape, "white");
#endif
	draw_container (cr, top_level, shape);

	return true;
}


void
PropDrawingArea::draw_container (const Cairo::RefPtr<Cairo::Context>& cr, GfxContainerPtr& cont, Rect shape)
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
	bool usage = true;//cont->usage;

	//XXX need to define these somewhere .h?
	//XXX hard-code into theme.cpp?	A bit limiting

	std::vector<GfxContainerPtr> children = cont->children;

	if ((display != "box") && (display != "icon") && (display != "iconbox") && (display != "tabbox")) {
		display = "box";
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
		//std::cout << "Icon: " << icon << std::endl;
		draw_icon (cr, icon, box, below);
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
		draw_iconbox (cr, shape, tab, inside);

		Glib::RefPtr<Gdk::Pixbuf> icon;

		icon = gui_app->get_theme()->get_icon ("table");
		draw_icon (cr, icon,  tab, tab);
		icon = gui_app->get_theme()->get_icon ("shield");
		draw_icon (cr, icon, tab, tab);

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
		draw_box (cr, shape, inside);

		fill_area (cr, inside, background);

		if (usage) {
			Rect r_usage = inside;
			double d = (double) cont->bytes_used / (double) cont->bytes_size;
			r_usage.w = r_usage.w * d;
			if (r_usage.w > 1) {
				cr->set_source_rgba (0.97, 0.97, 0.42, 1.0);
				draw_border (cr, r_usage);
				cr->fill();
			}
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
		draw_tabbox (cr, shape, tab, inside);
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
}


