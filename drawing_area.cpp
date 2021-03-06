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

#include <unistd.h>
#include <pangomm.h>
#include <stdlib.h>
#include <gdk/gdk.h>
#include <gtkmm/separatormenuitem.h>

#include <cmath>
#include <cstdlib>
#include <sstream>

#include "action.h"
#include "drawing_area.h"
#include "gui_app.h"
#include "log.h"
#include "table.h"
#include "utils.h"
#include "window.h"
#ifdef DP_TEST
#include "test.h"
#endif
#include "partition.h"

DrawingArea::DrawingArea (void)
	// Glib::ObjectBase ("MyDrawingArea")
{
	LOG_CTOR;
	set_hexpand (true);
	set_vexpand (false);
	set_can_focus (true);

	add_events (Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::LEAVE_NOTIFY_MASK);

	signal_button_press_event() .connect (sigc::mem_fun (*this, &DrawingArea::on_mouse_click));
	signal_motion_notify_event().connect (sigc::mem_fun (*this, &DrawingArea::on_mouse_motion));
	signal_leave_notify_event() .connect (sigc::mem_fun (*this, &DrawingArea::on_mouse_leave));

	signal_focus_in_event() .connect (sigc::mem_fun (*this, &DrawingArea::on_focus_in));
	signal_focus_out_event().connect (sigc::mem_fun (*this, &DrawingArea::on_focus_out));

#if 0
	sigc::slot<bool> my_slot = sigc::bind (sigc::mem_fun (*this, &DrawingArea::on_timeout), 0);
	sigc::connection conn = Glib::signal_timeout().connect (my_slot, 300); // ms
#endif

	// set_tooltip_text ("tooltip number 1");

	set_has_tooltip();	// We'll be handling the tooltips ourself
	signal_query_tooltip().connect (sigc::mem_fun (*this, &DrawingArea::on_textview_query_tooltip));

	menu_popup.signal_key_press_event().connect (sigc::mem_fun (*this, &DrawingArea::popup_on_keypress));

	// Lambdas to let us know when the popup menu is in use.
	menu_popup.signal_show().connect ([this] { menu_active = true;  });
	menu_popup.signal_hide().connect ([this] { menu_active = false; });
}

DrawingArea::~DrawingArea()
{
	LOG_DTOR;
}


#if 0
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
 * draw_edge - 1px rectangular border
 */
void
draw_edge (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape, const Gdk::RGBA& colour)
{
	cr->save();
	set_colour (cr, colour);
	cr->set_line_width(1);

	cr->rectangle (shape.x+0.5, shape.y+0.5, shape.w-1, shape.h-1);

	cr->stroke();
	cr->restore();
}

#endif

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
	cr->set_line_width(1);
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
draw_grid_linear (const Cairo::RefPtr<Cairo::Context>& cr, Rect space, std::uint64_t max_size)
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
	cr->set_line_width(1);
	cr->rectangle (0.5+x, 0.5, w-1, h-1);
	cr->stroke();

	double spacing = pow (2, lower) / bytes_per_pixel;

	int count = (w / spacing) + 1;

	for (int i = 0; i <= count; ++i) {
		if ((i % major) == 0) {
			cr->set_line_width(3);
			cr->set_source_rgba (0.3, 0.3, 0.8, 0.7);
		} else if ((i % minor) == 0) {
			cr->set_line_width(2);
			cr->set_source_rgba (0.5, 0.5, 0.5, 0.6);	//XXX theme
		} else {
			cr->set_line_width(1);
			cr->set_source_rgba (0.3, 0.3, 0.3, 0.5);
		}

		cr->move_to ((int) (spacing*i) + 0.5 + x, 0);
		cr->rel_line_to (0, h);
		cr->stroke();
	}

	cr->restore();
}

void
draw_grid_log (const Cairo::RefPtr<Cairo::Context>& UNUSED(cr), Rect UNUSED(space), std::uint64_t UNUSED(max_size))
{
}


bool
is_empty (const GfxContainerPtr& gfx)
{
	return_val_if_fail (gfx, true);

#if 1
	int children = gfx->get_container()->get_count_real_children();
	return (children == 0);
#else
	int children = gfx->children.size();
	if (children == 0)
		return true;

	if (children > 1)
		return false;

	GfxContainerPtr child = gfx->children[0];
	if (child->get_container()->is_a ("Misc"))
		return true;

	return false;
#endif
}

/**
 * draw_block - draw an icon-width, hollow, rounded rectangle
 */
void
DrawingArea::draw_block (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape, GfxContainerPtr& gfx, Rect& tab, Rect& right)
{
	if (shape.h < (RADIUS*2)) {
		log_info ("draw_block: too short");
		return;
	}

	if (shape.w < (BLOCK_WIDTH + (RADIUS*2))) {
		log_info ("draw_block: too narrow");
		log_debug (gfx->dump());
		return;
	}

	Rect work = { shape.x, shape.y, BLOCK_WIDTH + (RADIUS/2), shape.h };

	const int& x = work.x;
	const int& y = work.y;
	const int& w = work.w;
	const int& h = work.h;

	draw_corner (cr, work, true,  false, true);		// Top left corner(1)
	draw_corner (cr, work, true,  true,  true);		// Top right corner(3)

	draw_arc (cr, work, true);				// Thin bottom right corner (12)
	draw_arc (cr, work, false);				// Thin bottom left corner (13)

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

	tab = { x+SIDES, y+RADIUS, BLOCK_WIDTH, h-RADIUS-(SIDES*1) };

	draw_corner (cr, tab, true, true,  false);		// Tab inside top right corner
	draw_corner (cr, tab, true, false, false);		// Tab inside top left corner

	right = { shape.x + work.w + GAP, shape.y, shape.w - work.w - GAP, shape.h};

	vRange.push_front ({shape, gfx});
}

/**
 * draw_container - recursively draw a set of containers
 */
void
DrawingArea::draw_container (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape, GfxContainerPtr& gfx)
{
	return_if_fail (cr);
	return_if_fail (gfx);

	std::string display = gfx->display;
	Gdk::RGBA background = gfx->background;
	Gdk::RGBA colour = gfx->colour;
	Glib::RefPtr<Gdk::Pixbuf> icon = gfx->icon;
	std::string name = gfx->name;
	std::string label = gfx->label;
	bool usage = gfx->usage;

	if (display.empty()) {
		display = "box";
	}

	//XXX need to define these somewhere .h?
	//XXX hard-code into theme.cpp?	A bit limiting
	if (display == "never") {		// Don't display at all
		return;
	}

	std::vector<GfxContainerPtr> children = gfx->children;

	if ((display != "box") && (display != "empty") && (display != "icon") && (display != "iconbox") && (display != "tabbox")) {
		display = "box";
	}

	if (display == "empty") {		// Only display if there's no children
		if (is_empty (gfx)) {
			display = "box";
		}
	}

	// Rect tab;
	Rect inside { 0, 0, 0, 0 };

	log_debug ("object = %s -- %d,%d", SP(gfx->name), shape.w, TAB_WIDTH);
	if (shape.w < TAB_WIDTH) {
#if 0
		log_debug ("draw_container: too narrow");
		log_debug (gfx->dump());
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
		vRange.push_front ({shape, gfx});
		log_debug ("Icon: %p", (void*) icon.operator->());
		draw_icon (cr, box, icon, below);
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
		vRange.push_front ({shape, gfx});

		Glib::RefPtr<Gdk::Pixbuf> icon;

		icon = gui_app->get_theme()->get_icon ("table");
		draw_icon (cr, tab, icon,  tab);
		icon = gui_app->get_theme()->get_icon ("shield");
		draw_icon (cr, tab, icon, tab);

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
		vRange.push_front ({shape, gfx});

		fill_area (cr, inside, background);

		if (usage) {
			Rect r_usage = inside;
			double d = (double) gfx->bytes_used / (double) gfx->bytes_size;
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
		vRange.push_front ({shape, gfx});
		/* theme
		 *	colour
		 *	background
		 */
	} else if (display == "empty") {	// Do nothing for now
		log_debug ("EMPTY");
		inside = shape;
	} else {
		log_debug ("unknown display type: %s", SP(display));
		return;
	}

	if (!inside.w) {
		log_error ("NO WIDTH");
		return;
	}

	uint64_t total = gfx->bytes_size;
	uint64_t bpp   = total / inside.w;	// bytes per pixel

	for (auto& c : gfx->children) {
		if (c->bytes_size > total) {
			total = c->bytes_size;		//XXX tmp -- need to get intermediate object
			bpp   = total / inside.w;
		}
		if (bpp == 0) {
			log_error ("no bpp");
			continue;
		}
		std::uint64_t offset = c->parent_offset / bpp;
		std::uint64_t size   = c->bytes_size    / bpp;

		Rect next = inside;
		next.x += offset;
		next.w = size;

		draw_container (cr, next, c);
	}
	//XXX vRange.push_front ({work, gfx});			// Associate a region with a container

	// if (gfx->get_focus() && (has_focus() || menu_active)) {
	if (gfx->get_focus()) {
		draw_focus (cr, shape, (has_focus() || menu_active));
	}
}

/**
 * draw_focus - 2px dashed black/white line
 */
void
DrawingArea::draw_focus (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape, bool primary)
{
	static int start = 0;
	std::vector<double> dashes = { 5, 5 };

	cr->save();
	draw_border (cr, shape);				// Set clipping area
	// cr->clip();

	if (primary) {
		cr->set_line_width(2);
	} else {
		cr->set_line_width(1);
	}

	cr->set_dash (dashes, start);
	cr->set_source_rgba (0.0, 0.0, 0.0, 1.0);		//XXX focus colours from theme
	draw_border (cr, shape);
	cr->close_path();
	cr->stroke();

	if (primary) {
		cr->set_dash (dashes, start+5);
		cr->set_source_rgba (1.0, 1.0, 1.0, 1.0);
		draw_border (cr, shape);
		cr->close_path();
		cr->stroke();
	}

	cr->restore();						// End clipping
	++start;
}

/**
 * draw_gradient - apply light shading to an area
 */
void
DrawingArea::draw_gradient (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape)
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
 * fill_rect - fill rectangle
 */
void
DrawingArea::fill_rect (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape, const Gdk::RGBA& colour)
{
	cr->save();
	set_colour (cr, colour);

	cr->rectangle (shape.x, shape.y, shape.w, shape.h);

	cr->fill();
	cr->restore();
}


bool
DrawingArea::on_draw (const Cairo::RefPtr<Cairo::Context>& cr)
{
	return_val_if_fail (top_level, false);
	LOG_TRACE;

	top_level->update_info();

	unsigned int children = top_level->children.size();
	set_size_request (-1, cont_height * children);

	ContainerPtr tlc = top_level->get_container();

	log_debug ("DRAW:");
	log_debug ("top_level = %p, tlc = %p", VP(top_level), VP(tlc));
	log_debug ("children = %ld", top_level->children.size());

	vRange.clear();

	Gtk::Allocation allocation = get_allocation();

	Rect shape { 0, 0, allocation.get_width(), allocation.get_height() };

#if 1
	checker_rect (cr, shape, 5);
#else
	draw_grid (cr, shape);
	draw_grid_linear (cr, shape, c->bytes_size);
	fill_rect (cr, shape, "white");
#endif
	shape.h = cont_height;
	if (top_level->is_top_level()) {
		for (auto& c : top_level->children) {
			// if (c->type == "Loop") {
				draw_container (cr, shape, c);
				shape.y += cont_height;
			// }
		}
	} else {
		draw_container (cr, shape, top_level);
	}

	return true;
}


#if 0
bool
DrawingArea::on_timeout (int timer_number)
{
	log_debug ("timer");
	redraw_area();
	// return (c->device == "/dev/sdc");
	return true;
}

#endif

#if 0
void
dump_range (const std::deque<Range>& vRange)
{
	log_debug ("Ranges:");
	for (auto& rg : vRange) {
		Rect         r = rg.r;
		GfxContainerPtr p = rg.p;
		std::string type = "unknown";
		if (p && p->type.size() > 0) {
			type = p->type.back();
		}

		log_debug ("\t%d,%d %d,%d %p (%s) - %ld", r.x, r.y, r.w, r.h, VP(p), SP(type), p.use_count());
	}
}

#endif

bool
DrawingArea::on_focus_in (GdkEventFocus* UNUSED(event))
{
	LOG_TRACE;

	Window *win = get_window();
	if (!win) {
		log_debug ("No Window");
		return false;
	}
	log_debug ("top_level: %s", SP(win->get_name()));
	GfxContainerPtr gfx = win->get_focus();
	if (!gfx) {
		log_debug ("No focus");
		gfx = get_focus (0, 0);
		if (gfx) {
			win->set_focus (gfx);
		}
	}

	return true;
}

bool
DrawingArea::on_focus_out (GdkEventFocus* UNUSED(event))
{
	LOG_TRACE;
	return true;
}

bool
DrawingArea::on_keypress (GdkEventKey* event)
{
	return_val_if_fail (event, false);

	bool redraw  = false;
	bool handled = false;

	log_debug ("Key: %d (0x%x)", event->keyval, event->keyval);

	// Extra keys: Delete, Insert, Space/Enter (select)?

	Window *win = get_window();
	if (!win) {
		log_debug ("No Window");
		return false;
	}
	log_debug ("top_level: %s", SP(win->get_name()));

	GfxContainerPtr gfx = win->get_focus();
	if (!gfx) {
		log_debug ("No focus");
		return false;
	}

	int x = 0;
	int y = 0;
	switch (event->keyval) {
		case GDK_KEY_Return:	// 65293 (0xFF0D)
			log_debug ("state = %d", event->state);
			if (event->state & GDK_MOD1_MASK) {		// Alt-Enter
				on_menu_select (gfx, Action { "Properties", "Properties", nullptr, true });	// properties
				handled = true;
			}
			break;
		case GDK_KEY_Menu:	// 65383 (0xFF67)
			get_coords (x, y);
			popup_menu (gfx, x, y);
			handled = true;
			break;
		case GDK_KEY_Left:	// 65361 (0xFF51)
			redraw = win->set_focus (left(gfx));
			handled = true;
			break;
		case GDK_KEY_Right:	// 65363 (0xFF53)
			redraw = win->set_focus (right(gfx));
			handled = true;
			break;
		case GDK_KEY_Up:	// 65362 (0xFF52)
			redraw = win->set_focus (up(gfx));
			handled = true;
			break;
		case GDK_KEY_Down:	// 65364 (0xFF54)
			redraw = win->set_focus (down(gfx));
			handled = true;
			break;
	}

	if (redraw) {
		redraw_area();
	}

	return handled;
}

bool
DrawingArea::on_mouse_click (GdkEventButton* event)
{
	// top_level->dump3();
	log_debug ("mouse click: (%.0f,%.0f)", event->x, event->y);

	grab_focus();				// Place the windows focus on the DrawingArea

	Window *win = get_window();
	if (!win) {
		return false;
	}

	log_debug ("top_level: %s", SP(win->get_name()));

	GfxContainerPtr selection;

	for (const auto& rg : vRange) {
		const Rect& r = rg.r;

		if ((event->x >= r.x) && (event->x < (r.x + r.w)) &&
		    (event->y >= r.y) && (event->y < (r.y + r.h))) {
			if (win->set_focus (rg.p)) {
				redraw_area();
			}
			selection = rg.p;
			break;
		}
	}

	if (event->type == GDK_2BUTTON_PRESS) {
		on_menu_select (selection, Action { "Properties", "Properties", nullptr, true } );	// Properties
		return true;					// We handled the event
	}

	if (event->type != GDK_BUTTON_PRESS)
		return false;			// We haven't handled the event

	if ((event->button == 3) && (selection)) {		// Right-click
		popup_menu (selection, event->x_root, event->y_root);
		return true;
	}

#if 0
	GfxContainerPtr top = selection->get_top_level();
	// top->dump3();

	ContainerPtr c = selection->get_container();
	test_execute (c, top_level->name);
	// top_level->dump3();
#endif

	return true;		// We've handled the event
}

bool
DrawingArea::on_mouse_leave (GdkEventCrossing* UNUSED(event))
{
#if 0
	if (mouse_close) {
		mouse_close = false;
		redraw_area();
	}
#endif
	return true;
}

bool
DrawingArea::on_mouse_motion (GdkEventMotion* UNUSED(event))
{
	// log_debug ("mouse motion: (%.0f,%.0f)", event->x, event->y);

#if 0
	bool old = mouse_close;

	mouse_close = ((event->y > 25) and (event->x < 90));

	if (mouse_close != old) {
		redraw_area();
	}
#endif

	return true;
}


#if 0
void
draw_container_examples (const Cairo::RefPtr<Cairo::Context>& cr, GfxContainerPtr gfx, Rect shape, Rect* right)
{
	return_if_fail (gfx);

#if 0 // icon + label above
	Rect inside;
	Rect below;

	fill_rect (cr, shape, "white");
	cr->set_source_rgba (0.0, 0.0, 0.0, 1.0);

	Pango::FontDescription font;
	font.set_family ("Liberation Sans");

	Glib::RefPtr<Pango::Layout> layout = Pango::Layout::create (cr);
	layout->set_font_description (font);

	std::string title = "<b>" + gfx->get_device_name() + "</b> &#8211; " + get_size (gfx->bytes_size) + " <small>(" + std::to_string (100*gfx->bytes_used/gfx->bytes_size) + "% used)</small>";

	// https://developer.gnome.org/pango/stable/PangoMarkupFormat.html
	layout->set_markup (title);

	int tw = 0;
	int th = 0;
	layout->get_pixel_size (tw, th);
	log_debug ("text width = %d, height = %d", tw, th);

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
	// draw_edge (cr, inside, "red");

	cr->set_source_rgba (1.0, 1.0, 1.0, 1.0);
	draw_border (cr, inside);
	cr->fill();

	draw_icon (cr, "disk", inside, below);
#endif

#if 0 // icon + shaded label below
	Rect inside;
	Rect below;

	shape.w = 2*TAB_WIDTH + 2*GAP;

	draw_box (cr, shape, inside);
	// draw_edge (cr, inside, "red");

	cr->set_source_rgba (1.0, 1.0, 1.0, 1.0);
	draw_border (cr, inside);
	cr->fill();

	draw_icon (cr, "disk", inside, below);

	std::string labeld = gfx->device;
	std::string labels = get_size (gfx->bytes_size);

	Pango::FontDescription font;
	Glib::RefPtr<Pango::Layout> layoutd = Pango::Layout::create (cr);
	Glib::RefPtr<Pango::Layout> layouts = Pango::Layout::create (cr);

	int xd = 0, yd = 0, wd = 0, hd = 0;
	int xs = 0, ys = 0, ws = 0, hs = 0;

	std::size_t pos = labeld.find_last_of ('/');
	if (pos != std::string::npos) {
		labeld = labeld.substr (pos+1);
	}
	labeld = "<b>" + labeld + "</b>";

	font.set_family ("Liberation Sans");		// THEME - block_label_font

	font.set_size (12 * Pango::SCALE);		// THEME - block_label_font_size
	layoutd->set_font_description (font);

	font.set_size (8 * Pango::SCALE);		// THEME - ratio of block_label_font_size
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
	cr->set_source_rgba (1.0, 1.0, 1.0, 1.0);	// THEME - block_label_highlight_colour
	cr->move_to (xd+2, yd+0); layoutd->update_from_cairo_context (cr); layoutd->show_in_cairo_context (cr);
	cr->move_to (xd-2, yd+0); layoutd->update_from_cairo_context (cr); layoutd->show_in_cairo_context (cr);
	cr->move_to (xd+0, yd+2); layoutd->update_from_cairo_context (cr); layoutd->show_in_cairo_context (cr);
	cr->move_to (xd+0, yd-2); layoutd->update_from_cairo_context (cr); layoutd->show_in_cairo_context (cr);

	cr->move_to (xd, yd);
	cr->set_source_rgba (0.0, 0.0, 0.0, 1.0);	// THEME - block_label_text_colour
	layoutd->update_from_cairo_context (cr);
	layoutd->show_in_cairo_context (cr);

	// Draw size label
	cr->move_to (xs, ys);
	cr->set_source_rgba (0.0, 0.0, 0.0, 1.0);	// THEME - block_label_text_colour
	layouts->update_from_cairo_context (cr);
	layouts->show_in_cairo_context (cr);
#endif
}

TablePtr
DrawingArea::get_protective (GfxContainerPtr& gfx)
{
	return_val_if_fail (gfx, nullptr);

	GfxContainerPtr child;

	log_debug ("1: %s", gfx->dump());

	if (!gfx->is_a ("Table"))
		return nullptr;

	if (gfx->children.size() != 1)
		return nullptr;

	child = gfx->children[0];
	log_debug ("2: %s", child->dump());
	if (!child)
		return nullptr;

	if (!child->is_a ("Partition"))
		return nullptr;

	if (child->children.size() != 1)
		return nullptr;

	child = child->children[0];
	log_debug ("3: %s", child->dump());
	if (!child)
		return nullptr;

	if (child->is_a ("Table"))
		return std::dynamic_pointer_cast<Table> (child);

	return nullptr;
}

#endif

void
DrawingArea::set_data (GfxContainerPtr& g)
{
	return_if_fail(g);

	BaseDrawingArea::set_data(g);

	// check we've been given a top level object?

#if 0
	// invalidate window
	unsigned int children = g->children.size();
	// children = 14;
	set_size_request (500, cont_height * children);
#endif
}

GfxContainerPtr
DrawingArea::get_focus (int x, int y)
{
	GfxContainerPtr match;

	for (auto& rg : vRange) {
		Rect r = rg.r;
		if ((x >= r.x) && (x < (r.x + r.w)) && (y >= r.y) && (y < (r.y + r.h))) {
			match = rg.p;
			break;
		}
	}

	return match;
}

void
DrawingArea::set_focus (GfxContainerPtr& gfx)
{
	return_if_fail (gfx);

	log_debug (gfx->dump());

	ContainerPtr c = gfx->get_container();
	if (!c) {
		log_debug ("No focus");
		return;
	}

	std::vector<Action> actions;

#if 0
	for (; c; c = c->parent.lock()) {
		log_debug (c->type.back());
		auto& tmp = c->get_actions();
		actions.insert (std::end (actions), std::begin (tmp), std::end (tmp));
		for (auto& t : tmp) {
			log_debug ("\t%s", t.SP(name));
		}
	}
#endif

	if (actions.empty()) {
		log_debug ("No actions");
		// return;
	}

	Window *win = get_window();
	if (!win) {
		log_debug ("No Window");
		return;
	}

	win->set_actions (actions);

#if 0
	log_debug ("Actions:");
	for (auto& a : actions) {
		log_debug ("\t%s", SP(a.name));
	}
#endif

	redraw_area();
}

bool
DrawingArea::on_textview_query_tooltip (int x, int y, bool UNUSED(keyboard_tooltip), const Glib::RefPtr<Gtk::Tooltip>& tooltip)
{
	if (menu_active)
		return false;

	GfxContainerPtr gfx = get_focus (x, y);
	if (gfx) {
		std::string text = gfx->get_tooltip();
		escape_text (text);
		text = "<b>" + text + "</b>";
		tooltip->set_markup (text);
		tooltip->set_icon_from_icon_name ("dialog-information", Gtk::ICON_SIZE_MENU);

		return true;
	}

	return false;
}


Rect
DrawingArea::get_rect (const GfxContainerPtr& g)
{
	Rect r = { -1, -1, -1, -1 };

	return_val_if_fail (g, r);

	for (const auto& rg : vRange) {
		if (rg.p == g) {
			return rg.r;
		}
	}

	return r;
}


void
DrawingArea::set_cont_height (int height)
{
	cont_height = height;
	//XXX invalidate
}

int
DrawingArea::get_cont_height (void)
{
	return cont_height;
}

void
DrawingArea::set_cont_recurse (int recurse)
{
	cont_recurse = recurse;
	//XXX invalidate
}

bool
DrawingArea::get_cont_recurse (void)
{
	return cont_recurse;
}


bool
DrawingArea::is_visible (const GfxContainerPtr& gfx)
{
	return_val_if_fail (gfx, false);

	std::string& display = gfx->display;

	if (display == "never")
		return false;

	if ((display == "box") || (display == "icon") || (display == "iconbox") || (display == "tabbox"))
		return true;

	if ((display == "empty") && is_empty(gfx))
		return true;

	return false;
}

GfxContainerPtr
DrawingArea::left (GfxContainerPtr g)
{
	return_val_if_fail (g, nullptr);

	do {
		if (g->get_depth() == 1)	// Already at a top-level object
			return nullptr;

		g = g->get_left();

		if (is_visible(g))
			return g;

	} while(g);

	return g;
}

GfxContainerPtr
DrawingArea::right (GfxContainerPtr g)
{
	return_val_if_fail (g, nullptr);

	//XXX ugly, clumsy

	int y = get_rect(g).y + 30;	// plus half a row

	do {
		g = g->get_right();
		if (get_rect(g).y > y)	// We've changed rows
			return nullptr;

		if (is_visible(g))
			return g;

	} while(g);

	return nullptr;
}

GfxContainerPtr
DrawingArea::up (const GfxContainerPtr& g)
{
	return_val_if_fail (g, nullptr);

	Rect r = get_rect(g);
	r.y = ((r.y/cont_height) * cont_height) - (cont_height/2);
	return get_focus (r.x, r.y);
}

GfxContainerPtr
DrawingArea::down (const GfxContainerPtr& g)
{
	return_val_if_fail (g, nullptr);

	Rect r = get_rect(g);
	r.y = ((r.y/cont_height) * cont_height) + cont_height + (cont_height/2);
	return get_focus (r.x, r.y);
}


void
DrawingArea::setup_popup (GfxContainerPtr gfx, std::vector<Action>& actions)
{
	return_if_fail (gfx);

	for (auto& i : menu_popup.get_children()) {	// Clear the current menu
		menu_popup.remove (*i);
	}

	std::string section;
	std::string key;
	Gtk::Menu*     index_menu = &menu_popup;
	Gtk::MenuItem* index_item = nullptr;

	// This is a poor temporary solution
	std::sort (std::begin (actions), std::end (actions), [] (const Action& a, const Action& b) { return a.description < b.description; });

	actions.insert (std::begin (actions), { "---",        "---",        nullptr, true });
	actions.insert (std::begin (actions), { "Properties", "Properties", nullptr, true });

#if 0
	actions.push_back ({ "---",                "---",                nullptr, true });
	actions.push_back ({ "edit.cut",           "Edit/Cut",           nullptr, true });
	actions.push_back ({ "edit.copy",          "Edit/Copy",          nullptr, true });
	actions.push_back ({ "edit.paste",         "Edit/Paste",         nullptr, true });
	actions.push_back ({ "edit.paste_special", "Edit/Paste Special", nullptr, true });
#endif

	for (auto& a : actions) {
		log_debug (a.description);
		std::size_t pos = a.description.find_first_of ('/');
		if (pos == std::string::npos) {
			section.clear();
			key = a.description;
			index_menu = &menu_popup;
			index_item = nullptr;
		} else {
			std::string s = a.description.substr (0, pos);
			key = a.description.substr (pos+1);

			if (section != s) {
				section = s;

				Gtk::Menu*     sub_menu = Gtk::manage (new Gtk::Menu());
				Gtk::MenuItem* sub_item = Gtk::manage (new Gtk::MenuItem (section, true));

				menu_popup.append (*sub_item);
				sub_item->set_submenu (*sub_menu);
				sub_item->set_sensitive (false);		// default to off

				index_menu = sub_menu;
				index_item = sub_item;
			}
		}

		Gtk::MenuItem* item = nullptr;

		if (a.description == "---") {
			item = Gtk::manage (new Gtk::SeparatorMenuItem());
		} else {
			item = Gtk::manage (new Gtk::MenuItem (key, true));
			item->signal_activate().connect (sigc::bind<GfxContainerPtr, Action> (sigc::mem_fun (*this, &DrawingArea::on_menu_select), gfx, a));
			item->set_sensitive (a.enabled);
		}

		index_menu->append (*item);
		if (a.enabled) {
#if 0
			index_item->show();
			item->show();
#endif
			if (index_item) {
				index_item->set_sensitive (true);		// enable the parent, if necessary
			}
		}
	}

	menu_popup.accelerate (*this);
#if 1
	menu_popup.show_all();
#endif
}

void
DrawingArea::on_menu_select (GfxContainerPtr gfx, Action action)
{
	return_if_fail (gfx);
	LOG_TRACE;

	ContainerPtr c = gfx->get_container();
	if (!c)
		return;

	if (action.name == "Copy") {
		log_debug (action.name);
	} else if (action.name == "Paste") {
		log_debug (action.name);
	} else if (action.name == "Paste Special") {
		log_debug (action.name);
	} else if (action.name == "Properties") {
		gui_app->properties (gfx);
	} else {
		//XXX needs to be done in a separate thread in case it takes a while
		c->perform_action (action);
	}
}

/**
 * get_coords
 *
 * Get the absolute screen coordinates of the highlighted container.
 */
bool
DrawingArea::get_coords (int& x, int& y)
{
	Window *win = get_window();
	if (!win) {
		log_debug ("No Window");
		return false;
	}

	log_debug ("top_level: %s", SP(win->get_name()));
	GfxContainerPtr gfx = win->get_focus();
	if (!gfx) {
		log_debug ("No focus");
		return false;
	}

	Glib::RefPtr<Gdk::Window> w = Gtk::Widget::get_window();
	if (!w) {
		return false;
	}

	int ox = 0;
	int oy = 0;
	w->get_origin (ox, oy);		// Coords of Window's main window (inside chrome)

	log_debug ("top_level: %s", SP(win->get_name()));

	int tx = 0;
	int ty = 0;
	if (!translate_coordinates (*win, 0, 0, tx, ty)) {
		return false;		// Coords of DrawingArea within Window's window
	}

	Rect r = get_rect (gfx);		// Size and shape of selected container
	if (r.x < 0) {
		return false;
	}
#if 0
	log_debug ("o %d, %d", ox, oy);
	log_debug ("t %d, %d", tx, ty);
	log_debug ("r %d, %d, %d", r.x, r.y, r.h);
#endif

	x = ox + tx + r.x;
	y = oy + r.y + r.h;
	return true;
}

void
DrawingArea::popup_menu (GfxContainerPtr gfx, int x, int y)
{
	return_if_fail (gfx);

	ContainerPtr c = gfx->get_container();
	if (!c) {
		log_debug ("No gfx_container");
		return;
	}

	std::vector<Action> actions = c->get_actions();
	if (actions.empty()) {
		log_debug ("No actions");
		// return;
	}

#if 0
	log_info ("Actions:");
	for (auto& a : actions) {
		log_info ("\t%s", SP(a.name));
	}
#endif

	setup_popup (gfx, actions);

	// Lamba to position popup menu
	menu_popup.popup ([x, y] (int& xc, int& yc, bool& in) { xc = x; yc = y; in = false; }, 0, gtk_get_current_event_time());
}

bool
DrawingArea::popup_on_keypress (GdkEventKey* event)
{
	return_val_if_fail (event, false);

	if (event->keyval == GDK_KEY_Menu) {
		menu_popup.popdown();
		return true;
	}

	return false;
}


void
DrawingArea::theme_changed (const ThemePtr& new_theme)
{
	LOG_TRACE;
	BaseDrawingArea::theme_changed (new_theme);
}


void
DrawingArea::gfx_container_added (const GfxContainerPtr& UNUSED(child))
{
	log_error ("gfx_container_added");
	redraw_area();
}

void
DrawingArea::gfx_container_changed (const GfxContainerPtr& UNUSED(after))
{
	log_error ("gfx_container_changed");
	redraw_area();
}

void
DrawingArea::gfx_container_deleted (const GfxContainerPtr& UNUSED(child))
{
	log_error ("gfx_container_deleted");
	redraw_area();
}


Window*
DrawingArea::get_window (void)
{
	return dynamic_cast<Window*> (get_toplevel());
}

void
DrawingArea::redraw_area (void)
{
	auto win = Gtk::Widget::get_window();
	if (win)
		win->invalidate (false);
}

