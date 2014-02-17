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
#include <iostream>
#include <sstream>

#include "drawing_area.h"
#include "log.h"
#include "utils.h"
#include "log_trace.h"
#include "gui_app.h"
#include "window.h"
#include "table.h"

/**
 * DrawingArea
 */
DrawingArea::DrawingArea()
	//Glib::ObjectBase ("MyDrawingArea")
{
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

	//set_tooltip_text ("tooltip number 1");

	set_has_tooltip();	// We'll be handling the tooltips ourself
	signal_query_tooltip().connect (sigc::mem_fun (*this, &DrawingArea::on_textview_query_tooltip));

	m_Menu_Popup.signal_key_press_event().connect (sigc::mem_fun (*this, &DrawingArea::popup_on_keypress));

	// Lambdas to let us know when the popup menu is in use.
	m_Menu_Popup.signal_show().connect ([this] { menu_active = true;  });
	m_Menu_Popup.signal_hide().connect ([this] { menu_active = false; });
}

/**
 * ~DrawingArea
 */
DrawingArea::~DrawingArea()
{
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
	cr->set_line_width(1);
	cr->rectangle (0.5+x, 0.5, w-1, h-1);
	cr->stroke();

	double spacing = pow (2, lower) / bytes_per_pixel;

	int count = (w / spacing) + 1;

	for (int i = 0; i <= count; i++) {
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

/**
 * draw_grid_log
 */
void
draw_grid_log (const Cairo::RefPtr<Cairo::Context>& cr, Rect space, long max_size)
{
}

#endif

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


/**
 * draw_focus - 2px dashed black/white line
 */
void
DrawingArea::draw_focus (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape, bool primary)
{
	static int start = 0;
	std::vector<double> dashes = {5,5};

	cr->save();
	draw_border (cr, shape);				// Set clipping area
	//cr->clip();

	if (primary)
		cr->set_line_width(2);
	else
		cr->set_line_width(1);

	cr->set_dash (dashes, start);
	cr->set_source_rgba (0.0, 0.0, 0.0, 1.0);		//RAR focus colours from theme
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
	start++;
}

/**
 * draw_gradient - apply light shading to an area
 */
void
DrawingArea::draw_gradient (const Cairo::RefPtr<Cairo::Context>& cr, Rect shape)
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

	vRange.push_front ({shape, cont});
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
	shape.h = cont_height;
	if (top_level->name == "dummy") {
		for (auto c : top_level->children) {
			//if (c->type == "Loop") {
				draw_container (cr, c, shape);
				shape.y += cont_height;
			//}
		}
	} else {
		draw_container (cr, top_level, shape);
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

		printf ("\t%d,%d %d,%d %p (%s) - %ld\n", r.x, r.y, r.w, r.h, (void*) p.get(), type.c_str(), p.use_count());
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

	grab_focus();				// Place the windows focus on the DrawingArea

	//std::cout << "top_level: " << get_toplevel()->get_name() << std::endl;
	Window *dp = reinterpret_cast<Window*> (get_toplevel());

	GfxContainerPtr selection;

	for (const auto& rg : vRange) {
		const Rect& r = rg.r;

		if ((event->x >= r.x) && (event->x < (r.x + r.w)) &&
		    (event->y >= r.y) && (event->y < (r.y + r.h))) {
			if (dp->set_focus (rg.p)) {
				get_window()->invalidate (false);
			}
			selection = rg.p;
			break;
		}
	}

	if (event->type == GDK_2BUTTON_PRESS) {
		on_menu_select (selection, Action {"Properties"} );	// Properties
		return true;					// We handled the event
	}

	if (event->type != GDK_BUTTON_PRESS)
		return false;			// We haven't handled the event

	if ((event->button == 3) && (selection)) {		// Right-click
		popup_menu (selection, event->x_root, event->y_root);
	}

	return true;		// We've handled the event
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

	std::string title = "<b>" + cont->get_device_name() + "</b> &#8211; " + get_size (cont->bytes_size) + " <small>(" + std::to_string (100*cont->bytes_used/cont->bytes_size) + "% used)</small>";

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
	BaseDrawingArea::set_data (c);

	// check we've been given a top level object?

	if (!c)
		return;

	// invalidate window
	unsigned int children = c->children.size();
	children = 14;
	set_size_request (500, cont_height * children);

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
 * set_focus
 */
void
DrawingArea::set_focus (GfxContainerPtr& gfx)
{
	if (!gfx)
		return;

	//std::cout << gfx << std::endl;

	ContainerPtr c = gfx->get_container();
	if (!c) {
		std::cout << "No focus" << std::endl;
		return;
	}

	std::vector<Action> actions;

	for (; c; c = c->parent.lock()) {
		std::cout << c->type.back() << std::endl;
		auto tmp = c->get_actions();
		actions.insert (std::end (actions), std::begin (tmp), std::end (tmp));
		for (auto t : tmp) {
			std::cout << '\t' << t.name << std::endl;
		}
	}
	std::cout << std::endl;

	if (actions.empty()) {
		//std::cout << "No actions" << std::endl;
		//return;
	}

	Window *dp = reinterpret_cast<Window*> (get_toplevel());
	if (!dp) {
		std::cout << "No Window" << std::endl;
		return;
	}

	dp->set_actions (actions);

#if 0
	std::cout << "Actions:" << std::endl;
	for (auto a : actions) {
		int colour;
		if (a.enabled)
			colour = 32;
		else
			colour = 31;

		std::cout << "\t" << "\033[01;" << std::to_string (colour) << "m" << a.name << "\033[0m" << std::endl;
	}
	std::cout << std::endl;
#endif

	get_window()->invalidate (false);
}

/**
 * on_textview_query_tooltip
 */
bool
DrawingArea::on_textview_query_tooltip (int x, int y, bool keyboard_tooltip, const Glib::RefPtr<Gtk::Tooltip>& tooltip)
{
	if (menu_active)
		return false;

	GfxContainerPtr c = get_focus (x, y);
	if (c) {
		std::string text = c->get_tooltip();
		escape_text (text);
		text = "<b>" + text + "</b>";
		tooltip->set_markup (text);
		tooltip->set_icon_from_icon_name ("dialog-information", Gtk::ICON_SIZE_MENU);

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
		vRange.push_front ({shape, cont});

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
		vRange.push_front ({shape, cont});

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
		vRange.push_front ({shape, cont});
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

		draw_container (cr, c, next);
	}
	//XXX vRange.push_front ({work, cont});			// Associate a region with a container

	//if (cont->get_focus() && (has_focus() || menu_active)) {
	if (cont->get_focus()) {
		draw_focus (cr, shape, (has_focus() || menu_active));
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
DrawingArea::on_keypress (GdkEventKey* ev)
{
	bool redraw  = false;
	bool handled = false;

	//std::cout << "Key: " << std::dec << ev->keyval << " (0x" << std::hex << ev->keyval << ")" << std::dec << std::endl;

	//Extra keys: Delete, Insert, Space/Enter (select)?

	//std::cout << "top_level: " << get_toplevel()->get_name() << std::endl;
	Window *dp = reinterpret_cast<Window*> (get_toplevel());
	if (!dp) {
		std::cout << "No Window" << std::endl;
		return false;
	}

	GfxContainerPtr c = dp->get_focus();
	if (!c) {
		//std::cout << "No focus" << std::endl;
		return false;
	}

	int x = 0;
	int y = 0;
	switch (ev->keyval) {
		case GDK_KEY_Return:	// 65293 (0xFF0D)
			//std::cout << "state = " << ev->state << std::endl;
			if (ev->state & GDK_MOD1_MASK) {		// Alt-Enter
				on_menu_select (c, Action {"Properties"});	// properties
				handled = true;
			}
			break;
		case GDK_KEY_Menu:	// 65383 (0xFF67)
			get_coords (x, y);
			popup_menu (c, x, y);
			handled = true;
			break;
		case GDK_KEY_Left:	// 65361 (0xFF51)
			redraw = dp->set_focus (left(c));
			handled = true;
			break;
		case GDK_KEY_Right:	// 65363 (0xFF53)
			redraw = dp->set_focus (right(c));
			handled = true;
			break;
		case GDK_KEY_Up:	// 65362 (0xFF52)
			redraw = dp->set_focus (up(c));
			handled = true;
			break;
		case GDK_KEY_Down:	// 65364 (0xFF54)
			redraw = dp->set_focus (down(c));
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
	//LOG_TRACE;

	//std::cout << "top_level: " << get_toplevel()->get_name() << std::endl;
	Window *dp = reinterpret_cast<Window*> (get_toplevel());
	if (!dp) {
		std::cout << "No Window" << std::endl;
		return false;
	}
	GfxContainerPtr c = dp->get_focus();
	if (!c) {
		//std::cout << "No focus" << std::endl;
		c = get_focus (0, 0);
		if (c) {
			dp->set_focus(c);
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
	//LOG_TRACE;
	return true;
}


/**
 * set_cont_height
 */
void
DrawingArea::set_cont_height (int height)
{
	cont_height = height;
	//XXX invalidate
}

/**
 * get_cont_height
 */
int
DrawingArea::get_cont_height (void)
{
	return cont_height;
}

/**
 * set_cont_recurse
 */
void
DrawingArea::set_cont_recurse (int recurse)
{
	cont_recurse = recurse;
	//XXX invalidate
}

/**
 * get_cont_recurse
 */
bool
DrawingArea::get_cont_recurse (void)
{
	return cont_recurse;
}


/**
 * is_visible
 */
bool
DrawingArea::is_visible (const GfxContainerPtr& c)
{
	if (!c)
		return false;

	std::string& display = c->display;

	if (display == "never")
		return false;

	if ((display == "box") || (display == "icon") || (display == "iconbox") || (display == "tabbox"))
		return true;

	if ((display == "empty") && (c->children.empty()))
		return true;

	return false;
}

/**
 * left
 */
GfxContainerPtr
DrawingArea::left (GfxContainerPtr c)
{
	if (!c)
		return nullptr;

	do {
		if (c->get_depth() == 1)	// Already at a top-level object
			return nullptr;

		c = c->get_left();

		if (is_visible(c))
			return c;

	} while (c);

	return c;
}

/**
 * right
 */
GfxContainerPtr
DrawingArea::right (GfxContainerPtr c)
{
	//XXX ugly, clumsy

	if (!c)
		return nullptr;

	int y = get_rect(c).y + 30;	// plus half a row

	do {
		c = c->get_right();
		if (get_rect(c).y > y)	// We've changed rows
			return nullptr;

		if (is_visible(c))
			return c;

	} while (c);

	return nullptr;
}

/**
 * up
 */
GfxContainerPtr
DrawingArea::up (GfxContainerPtr c)
{
	if (!c)
		return nullptr;

	Rect r = get_rect(c);
	r.y = ((r.y/cont_height) * cont_height) - (cont_height/2);
	return get_focus (r.x, r.y);
}

/**
 * down
 */
GfxContainerPtr
DrawingArea::down (GfxContainerPtr c)
{
	if (!c)
		return nullptr;

	Rect r = get_rect(c);
	r.y = ((r.y/cont_height) * cont_height) + cont_height + (cont_height/2);
	return get_focus (r.x, r.y);
}


// POPUP
/**
 * setup_popup
 */
void
DrawingArea::setup_popup (GfxContainerPtr gfx, std::vector<Action>& actions)
{
	std::vector<Widget*> items = m_Menu_Popup.get_children();
	for (auto i : items) {
		m_Menu_Popup.remove(*i);
	}

	std::string section;
	std::string key;
	Gtk::Menu*     index_menu = &m_Menu_Popup;
	Gtk::MenuItem* index_item = nullptr;

	actions.insert (actions.begin(), { "---",        true });
	actions.insert (actions.begin(), { "Properties", true });

#if 0
	actions.push_back ({ "---",           true });
	actions.push_back ({ "Cut",           true });
	actions.push_back ({ "Copy",          true });
	actions.push_back ({ "Paste",         true });
	actions.push_back ({ "Paste Special", true });
#endif

	for (auto a : actions) {
		//std::cout << a.name << std::endl;
		size_t pos = a.name.find_first_of ('/');
		if (pos == std::string::npos) {
			section.clear();
			key = a.name;
			index_menu = &m_Menu_Popup;
			index_item = nullptr;
		} else {
			std::string s = a.name.substr (0, pos);
			key = a.name.substr (pos+1);

			if (section != s) {
				section = s;

				Gtk::Menu*     sub_menu = Gtk::manage (new Gtk::Menu());
				Gtk::MenuItem* sub_item = Gtk::manage (new Gtk::MenuItem (section, true));

				m_Menu_Popup.append (*sub_item);
				sub_item->set_submenu (*sub_menu);
				sub_item->set_sensitive(false);		// default to off

				index_menu = sub_menu;
				index_item = sub_item;
			}
		}

		Gtk::MenuItem* item = nullptr;

		if (a.name == "---") {
			item = Gtk::manage (new Gtk::SeparatorMenuItem());
		} else {
			item = Gtk::manage (new Gtk::MenuItem (key, true));
			item->signal_activate().connect (sigc::bind<GfxContainerPtr,Action> (sigc::mem_fun (*this, &DrawingArea::on_menu_select), gfx, a));
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

	m_Menu_Popup.accelerate (*this);
#if 1
	m_Menu_Popup.show_all();
#endif
}

/**
 * on_menu_select
 */
void
DrawingArea::on_menu_select (GfxContainerPtr gfx, Action action)
{
	//LOG_TRACE;

	ContainerPtr c = gfx->get_container();
	if (!c)
		return;

	if (action.name == "Copy") {
		std::cout << action.name << std::endl;
	} else if (action.name == "Paste") {
		std::cout << action.name << std::endl;
	} else if (action.name == "Paste Special") {
		std::cout << action.name << std::endl;
	} else if (action.name == "Properties") {
		gui_app->properties (gfx);
	} else {
		//XXX needs to be done in a separate thread in case it takes a white
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
	//std::cout << "top_level: " << get_toplevel()->get_name() << std::endl;
	Window *dp = reinterpret_cast<Window*> (get_toplevel());
	if (!dp) {
		std::cout << "No Window" << std::endl;
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
	w->get_origin (ox, oy);		// Coords of Window's main window (inside chrome)

	//std::cout << "top_level: " << get_toplevel()->get_name() << std::endl;
	Gtk::Widget* window = dynamic_cast<Gtk::Widget*> (get_toplevel());
	if (!window) {
		return false;
	}

	int tx = 0;
	int ty = 0;
	if (!translate_coordinates (*window, 0, 0, tx, ty)) {
		return false;		// Coords of DrawingArea within Window's window
	}

	Rect r = get_rect(c);		// Size and shape of selected container
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
 * popup_menu
 */
void
DrawingArea::popup_menu (GfxContainerPtr gfx, int x, int y)
{
	ContainerPtr c = gfx->get_container();
	if (!c) {
		std::cout << "No gfx_container" << std::endl;
		return;
	}

	std::vector<Action> actions = c->get_actions();
	if (actions.empty()) {
		//std::cout << "No actions" << std::endl;
		//return;
	}

#if 0
	std::cout << "Actions:" << std::endl;
	for (auto a : actions) {
		std::cout << "\t" << a << std::endl;
	}
#endif

	setup_popup (gfx, actions);

	// Lamba to position popup menu
	m_Menu_Popup.popup ([x,y] (int& xc, int& yc, bool& in) { xc = x; yc = y; in = false; }, 0, gtk_get_current_event_time());
}

/**
 * popup_on_keypress
 */
bool
DrawingArea::popup_on_keypress (GdkEventKey* ev)
{
	if (ev->keyval == GDK_KEY_Menu) {
		m_Menu_Popup.popdown();
		return true;
	}

	return false;
}


