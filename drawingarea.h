#ifndef _DRAWINGAREA_
#define _DRAWINGAREA_

#include <gtkmm/drawingarea.h>

class DPContainer;

typedef struct { int x; int y; int w; int h; DPContainer *p; } Range;

class DPDrawingArea : public Gtk::DrawingArea
{
public:
	DPDrawingArea();
	virtual ~DPDrawingArea();

	void set_data (DPContainer *c);

protected:
	virtual bool on_draw (const Cairo::RefPtr<Cairo::Context>& cr);

	void draw_box  (const Cairo::RefPtr<Cairo::Context>& cr, double x, double y, double width, double height, double line_width, double red, double green, double blue, double alpha);
	void draw_rect (const Cairo::RefPtr<Cairo::Context>& cr, double x, double y, double width, double height, double red, double green, double blue, double alpha);

	bool on_mouse_motion (GdkEventMotion *event);
	bool on_mouse_click  (GdkEventButton *event);
	bool on_mouse_leave  (GdkEventCrossing *event);

	bool on_timeout (int timer_number);

private:
	DPContainer *m_c;

	void get_colour (std::string &name, double &red, double &green, double &blue);
	bool get_focus (int &x, int &y, int &w, int &h);

	void draw_border (const Cairo::RefPtr<Cairo::Context>& cr, int x, int y, int w, int h, int r);
	void draw_container (const Cairo::RefPtr<Cairo::Context>& cr, int x, int y, int width, int height, DPContainer *c);
	void draw_frame (const Cairo::RefPtr<Cairo::Context>& cr, int &x, int &y, int &w, int &h, const Gdk::RGBA &colour);
	void draw_focus (const Cairo::RefPtr<Cairo::Context>& cr, int x, int y, int w, int h);
	void draw_tabframe (const Cairo::RefPtr<Cairo::Context>& cr, int &x, int &y, int &w, int &h, const Gdk::RGBA &colour);
	void draw_partition (const Cairo::RefPtr<Cairo::Context>& cr, int &x, int y, int w, int h, int width_fs, int width_usage, const Gdk::RGBA &colour);
	void write_label (const Cairo::RefPtr<Cairo::Context>& cr, const Glib::ustring &text);

	std::deque<Range> vRange;

	int sel_x;
	int sel_y;

	bool mouse_close;
};

#endif // _DRAWINGAREA_

