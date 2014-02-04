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

/**
 * class DrawingArea
 */
class DrawingArea : public BaseDrawingArea
{
public:
	DrawingArea();
	virtual ~DrawingArea();

	virtual void set_data (GfxContainerPtr& c);

	virtual void set_cont_height (int height);
	virtual int  get_cont_height (void);

	void set_cont_recurse (int recurse);
	bool get_cont_recurse (void);

	bool on_keypress (GdkEventKey* ev);
	void set_focus (GfxContainerPtr& c);

protected:
	bool cont_recurse = true;

	virtual bool on_draw (const Cairo::RefPtr<Cairo::Context>& cr);

	bool on_mouse_motion (GdkEventMotion* event);
	bool on_mouse_click  (GdkEventButton* event);
	bool on_mouse_leave  (GdkEventCrossing* event);

	bool on_focus_in (GdkEventFocus* event);
	bool on_focus_out (GdkEventFocus* event);

#if 0
	bool on_timeout (int timer_number);
#endif
	GfxContainerPtr get_focus (int x, int y);
	bool on_textview_query_tooltip (int x, int y, bool keyboard_tooltip, const Glib::RefPtr<Gtk::Tooltip>& tooltip);

private:
	void draw_container (const Cairo::RefPtr<Cairo::Context>& cr, GfxContainerPtr& cont, Rect shape);

	void draw_block   (const Cairo::RefPtr<Cairo::Context>& cr, GfxContainerPtr& cont, const Rect& shape, Rect& tab, Rect& right);

	void fill_rect (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape, const Gdk::RGBA& colour);
	void draw_focus (const Cairo::RefPtr<Cairo::Context>& cr, const Rect& shape, bool primary);
	void draw_gradient (const Cairo::RefPtr<Cairo::Context>& cr, Rect shape);

	Rect get_rect (GfxContainerPtr g);
	bool is_visible (const GfxContainerPtr& c);

	GfxContainerPtr left  (GfxContainerPtr c);
	GfxContainerPtr right (GfxContainerPtr c);
	GfxContainerPtr up    (GfxContainerPtr c);
	GfxContainerPtr down  (GfxContainerPtr c);

	std::deque<Range> vRange;

	bool mouse_close = false;

	Gtk::Menu m_fake_menu;

	// POPUP
	void setup_popup (std::vector<std::string>& actions);
	void on_menu_select (const std::string& option);
	bool get_coords (int& x, int& y);
	void popup_menu (GfxContainerPtr gfx, int x, int y);
	bool popup_on_keypress (GdkEventKey* ev);
	Gtk::Menu m_Menu_Popup;
	bool menu_active = false;
};


#endif // _DRAWING_AREA_H_

