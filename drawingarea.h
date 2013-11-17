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

#include <gtkmm/drawingarea.h>

class DPContainer;
class Filesystem;
class Misc;
class Partition;
class Table;
class Theme;

typedef struct { int x, y, w, h; } Rect;		// x,y coords, width, height
typedef struct { Rect r; DPContainer *p; } Range;

/**
 * class DPDrawingArea
 */
class DPDrawingArea : public Gtk::DrawingArea
{
public:
	DPDrawingArea();
	virtual ~DPDrawingArea();

	void set_data (DPContainer *c);

protected:
	virtual bool on_draw (const Cairo::RefPtr<Cairo::Context> &cr);

#if 0
	bool on_mouse_motion (GdkEventMotion *event);
	bool on_mouse_click  (GdkEventButton *event);
	bool on_mouse_leave  (GdkEventCrossing *event);
#endif

#if 0
	bool on_timeout (int timer_number);
	bool get_focus (int &x, int &y, int &w, int &h);
#endif

	bool get_theme (const std::string &object, const std::string &attr);

private:
	DPContainer *m_c;
	Theme *theme;

	void draw_block     (const Cairo::RefPtr<Cairo::Context> &cr, DPContainer *cont, const Rect &shape, Rect *tab, Rect *right);
	void draw_border    (const Cairo::RefPtr<Cairo::Context> &cr, const Rect &shape, Rect *inside = nullptr);
	void draw_box       (const Cairo::RefPtr<Cairo::Context> &cr, DPContainer *cont, const Rect &shape, Rect *inside);
	void draw_container (const Cairo::RefPtr<Cairo::Context> &cr, DPContainer *cont, Rect shape);
	void draw_fill      (const Cairo::RefPtr<Cairo::Context> &cr, const Rect &shape);
	void draw_focus     (const Cairo::RefPtr<Cairo::Context> &cr, const Rect &shape);
	void draw_highlight (const Cairo::RefPtr<Cairo::Context> &cr, const Rect &shape);
	void draw_icon      (const Cairo::RefPtr<Cairo::Context> &cr, const std::string &name, Rect &shape, Rect *below = nullptr);
	void draw_rect      (const Cairo::RefPtr<Cairo::Context> &cr, const std::string &colour, const Rect &shape);
	void draw_tabbox    (const Cairo::RefPtr<Cairo::Context> &cr, DPContainer *cont, const Rect &shape, Rect *tab, Rect *inside);

#if 0
	void draw_grid        (const Cairo::RefPtr<Cairo::Context> &cr);
	void draw_grid_linear (const Cairo::RefPtr<Cairo::Context> &cr, Rect space, long max_size);
	void draw_grid_log    (const Cairo::RefPtr<Cairo::Context> &cr, Rect space, long max_size);
#endif

	Table *       get_table      (DPContainer *c);
	Partition *   get_partition  (DPContainer *c);
	Table *       get_protective (DPContainer *c);
	Filesystem *  get_filesystem (DPContainer *c);
	Misc *        get_misc       (DPContainer *c);

	std::deque<Range> vRange;

	int sel_x;
	int sel_y;

	bool mouse_close;

	std::map<std::string,std::string> newtheme;
};


#endif // _DRAWINGAREA_

