#ifndef _DRAWINGAREA_
#define _DRAWINGAREA_

#include <gtkmm/drawingarea.h>

class DPContainer;

typedef struct { int l; int h; DPContainer *p; } Range;

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

private:
	DPContainer *m_c;

	void get_colour (std::string &name, double &red, double &green, double &blue);
	void draw_container (const Cairo::RefPtr<Cairo::Context>& cr, int x, int y, int width, int height, DPContainer *c);
	void draw_frame (const Cairo::RefPtr<Cairo::Context>& cr, int x, int y, int width, int height, DPContainer *c, std::string label_left, std::string label_right, int width_fs, int width_usage, Glib::RefPtr<Gdk::Pixbuf> icon1, Glib::RefPtr<Gdk::Pixbuf> icon2, Gdk::RGBA &colour);
	void draw_partition (const Cairo::RefPtr<Cairo::Context>& cr, int x, int y, int width, int height, DPContainer *c, std::string label_left, std::string label_right, int width_fs, int width_usage, Glib::RefPtr<Gdk::Pixbuf> icon1, Glib::RefPtr<Gdk::Pixbuf> icon2, Gdk::RGBA &colour);
	void write_label (const Cairo::RefPtr<Cairo::Context>& cr, const Glib::ustring &text);

	std::vector<Range> vRange;

	bool mouse_close;
};

#endif // _DRAWINGAREA_

