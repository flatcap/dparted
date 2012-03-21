#ifndef _DRAWINGAREA_
#define _DRAWINGAREA_

#include <gtkmm/drawingarea.h>

class DPContainer;
class Filesystem;
class Theme;

typedef struct { int x, y, w, h; } Rect;		// x,y coords, width, height
typedef struct { Rect r; DPContainer *p; } Range;

class DPDrawingArea : public Gtk::DrawingArea
{
public:
	DPDrawingArea();
	virtual ~DPDrawingArea();

	void set_data (DPContainer *c);

protected:
	virtual bool on_draw (const Cairo::RefPtr<Cairo::Context> &cr);

	bool on_mouse_motion (GdkEventMotion *event);
	bool on_mouse_click  (GdkEventButton *event);
	bool on_mouse_leave  (GdkEventCrossing *event);

	bool on_timeout (int timer_number);

private:
	DPContainer *m_c;
	Theme *theme;

	bool get_focus (int &x, int &y, int &w, int &h);

	void draw_icon      (const Cairo::RefPtr<Cairo::Context> &cr, const std::string &name, Rect &shape, Rect *below = NULL);
	void write_label    (const Cairo::RefPtr<Cairo::Context> &cr, const Glib::ustring &text, long size);
	void draw_frame     (const Cairo::RefPtr<Cairo::Context> &cr, const Gdk::RGBA &colour, const Rect &shape, Rect *inside = NULL, Rect *right = NULL);
	void draw_border    (const Cairo::RefPtr<Cairo::Context> &cr, const Rect &shape, Rect *inside = NULL, Rect *right = NULL);
	void draw_focus     (const Cairo::RefPtr<Cairo::Context> &cr, const Rect &shape);
	void draw_tabframe  (const Cairo::RefPtr<Cairo::Context> &cr, const std::string &colour, const Rect &shape, Rect *tab = NULL, Rect *inside = NULL, Rect *right = NULL);
	void draw_rect      (const Cairo::RefPtr<Cairo::Context> &cr, const std::string &colour, const Rect &shape);
	void draw_container (const Cairo::RefPtr<Cairo::Context> &cr, DPContainer *c, Rect shape, Rect *right = NULL);
	void draw_partition (const Cairo::RefPtr<Cairo::Context> &cr, const std::string &colour, int width_part, int width_fs, int width_usage, Rect shape, Rect *inside = NULL, Rect *right = NULL);
	void draw_block     (const Cairo::RefPtr<Cairo::Context> &cr, DPContainer *c, Rect &space, Rect *right = NULL);

	void draw_grid      (const Cairo::RefPtr<Cairo::Context> &cr);
	void draw_highlight (const Cairo::RefPtr<Cairo::Context> &cr, const Rect &shape);

	DPContainer * get_table      (DPContainer *c);
	DPContainer * get_protective (DPContainer *c);
	Filesystem *  get_filesystem (DPContainer *c);

	std::deque<Range> vRange;

	int sel_x;
	int sel_y;

	bool mouse_close;
};

#endif // _DRAWINGAREA_

