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

#include "gfx_container.h"

typedef struct { int x, y, w, h; } Rect;		// x,y coords, width, height
typedef struct { Rect r; GfxContainerPtr p; } Range;

/**
 * class DrawingArea
 */
class DrawingArea : public Gtk::DrawingArea
{
public:
	DrawingArea();
	virtual ~DrawingArea();

	void set_data (GfxContainerPtr& c);

	bool on_keypress(GdkEventKey* ev);
protected:
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
	bool on_textview_query_tooltip(int x, int y, bool keyboard_tooltip, const Glib::RefPtr<Gtk::Tooltip>& tooltip);

private:
	void draw_container (const Cairo::RefPtr<Cairo::Context>& cr, GfxContainerPtr& cont, Rect shape);
	void draw_icon      (const Cairo::RefPtr<Cairo::Context>& cr, GfxContainerPtr& cont, Glib::RefPtr<Gdk::Pixbuf> icon, const Rect& shape, Rect& below);

	void draw_block   (const Cairo::RefPtr<Cairo::Context>& cr, GfxContainerPtr& cont, const Rect& shape, Rect& tab, Rect& right);
	void draw_box     (const Cairo::RefPtr<Cairo::Context>& cr, GfxContainerPtr& cont, const Rect& shape, Rect& inside);
	void draw_iconbox (const Cairo::RefPtr<Cairo::Context>& cr, GfxContainerPtr& cont, const Rect& shape, Rect& tab, Rect& inside);
	void draw_tabbox  (const Cairo::RefPtr<Cairo::Context>& cr, GfxContainerPtr& cont, const Rect& shape, Rect& tab, Rect& inside);

	void make_menu (void);
	void on_menu_file_popup_generic (void);
	void on_popup_menu_position (int& x, int& y, bool& push_in);
	Gtk::Menu* m_pMenuPopup = nullptr;
	Glib::RefPtr<Gtk::Builder> m_refBuilder;
	int menux = 0;
	int menuy = 0;
	bool get_coords (int& x, int& y);
	void popup_menu (int x, int y);

	Rect get_rect (GfxContainerPtr g);

	std::deque<Range> vRange;

	bool mouse_close = false;

	GfxContainerPtr top_level;
	Gtk::Menu m_fake_menu;
};


#endif // _DRAWING_AREA_H_

