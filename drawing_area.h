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

#ifndef _DRAWING_AREA_H_
#define _DRAWING_AREA_H_

#include <deque>
#include <memory>
#include <map>

#include <gtkmm/drawingarea.h>
#include <gtkmm/builder.h>
#include <gtkmm/menu.h>

#include "gfx_container.h"
#include "base_drawing_area.h"

typedef struct { Rect r; GfxContainerPtr p; } Range;

class DrawingArea : public BaseDrawingArea
{
public:
	DrawingArea();
	virtual ~DrawingArea();

	virtual void set_data (GfxContainerPtr& g);

	virtual void set_cont_height (int height);
	virtual int  get_cont_height (void);

	void set_cont_recurse (int recurse);
	bool get_cont_recurse (void);

	bool on_keypress (GdkEventKey* event);
	void set_focus (GfxContainerPtr& gfx);

protected:
	bool cont_recurse = true;

	virtual bool on_draw (const Cairo::RefPtr<Cairo::Context>& cr);

	bool on_focus_in     (GdkEventFocus*    event);
	bool on_focus_out    (GdkEventFocus*    event);
	bool on_mouse_click  (GdkEventButton*   event);
	bool on_mouse_leave  (GdkEventCrossing* event);
	bool on_mouse_motion (GdkEventMotion*   event);

#if 0
	bool on_timeout (int timer_number);
#endif
	GfxContainerPtr get_focus (int x, int y);
	bool on_textview_query_tooltip (int x, int y, bool keyboard_tooltip, const Glib::RefPtr<Gtk::Tooltip>& tooltip);

private:
	void draw_block     (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape, GfxContainerPtr& gfx, Rect& tab, Rect& right);
	void draw_container (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape, GfxContainerPtr& gfx);
	void draw_focus     (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape, bool primary);
	void draw_gradient  (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape);
	void fill_rect      (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape, const Gdk::RGBA& colour);

	Rect get_rect   (const GfxContainerPtr& g);
	bool is_visible (const GfxContainerPtr& g);

	GfxContainerPtr left  (      GfxContainerPtr  g);
	GfxContainerPtr right (      GfxContainerPtr  g);
	GfxContainerPtr up    (const GfxContainerPtr& g);
	GfxContainerPtr down  (const GfxContainerPtr& g);

	std::deque<Range> vRange;

	//bool mouse_close = false;

	// POPUP
	void setup_popup (GfxContainerPtr gfx, std::vector<Action>& actions);
	void on_menu_select (GfxContainerPtr gfx, Action action);
	bool get_coords (int& x, int& y);
	void popup_menu (GfxContainerPtr gfx, int x, int y);
	bool popup_on_keypress (GdkEventKey* ev);
	Gtk::Menu menu_popup;
	bool menu_active = false;
};


#endif // _DRAWING_AREA_H_

