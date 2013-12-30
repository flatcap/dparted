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

#ifndef _DRAWINGAREA_
#define _DRAWINGAREA_

#include <deque>

#include <gtkmm/drawingarea.h>
#include <memory>

#include "container.h"
#include "gfx_container.h"

class Theme;

typedef struct { int x, y, w, h; } Rect;		// x,y coords, width, height
typedef struct { Rect r; ContainerPtr p; } Range;

/**
 * class DPDrawingArea
 */
class DPDrawingArea : public Gtk::DrawingArea
{
public:
	DPDrawingArea();
	virtual ~DPDrawingArea();

	void set_data (ContainerPtr& c);

protected:
	virtual bool on_draw (const Cairo::RefPtr<Cairo::Context>& cr);

	bool on_mouse_motion (GdkEventMotion* event);
	bool on_mouse_click  (GdkEventButton* event);
	bool on_mouse_leave  (GdkEventCrossing* event);

#if 0
	bool on_timeout (int timer_number);
#endif
	ContainerPtr get_focus (int x, int y);
	bool on_textview_query_tooltip(int x, int y, bool keyboard_tooltip, const Glib::RefPtr<Gtk::Tooltip>& tooltip);

private:
	ContainerPtr m_c;
	Theme* theme = nullptr;

	void draw_container (const Cairo::RefPtr<Cairo::Context>& cr, ContainerPtr& cont, Rect shape);
	void draw_icon      (const Cairo::RefPtr<Cairo::Context>& cr, ContainerPtr& cont, const std::string& name, const Rect& shape, Rect& below);

	void draw_block   (const Cairo::RefPtr<Cairo::Context>& cr, ContainerPtr& cont, const Rect& shape, Rect& tab, Rect& right);
	void draw_box     (const Cairo::RefPtr<Cairo::Context>& cr, ContainerPtr& cont, const Rect& shape, Rect& inside);
	void draw_iconbox (const Cairo::RefPtr<Cairo::Context>& cr, ContainerPtr& cont, const Rect& shape, Rect& tab, Rect& inside);
	void draw_tabbox  (const Cairo::RefPtr<Cairo::Context>& cr, ContainerPtr& cont, const Rect& shape, Rect& tab, Rect& inside);

	std::deque<Range> vRange;

	int sel_x = -1;
	int sel_y = -1;

	bool mouse_close = false;

	GfxContainerPtr focus;
	std::vector<GfxContainerPtr> selection;
};


#endif // _DRAWINGAREA_

