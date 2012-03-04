#ifndef _DRAWINGAREA_
#define _DRAWINGAREA_

#include <gtkmm/drawingarea.h>

class DPContainer;

class DrawingArea : public Gtk::DrawingArea
{
public:
	DrawingArea();
	virtual ~DrawingArea();

	void set_data (DPContainer *c);

protected:
	virtual bool on_draw (const Cairo::RefPtr<Cairo::Context>& cr);

	void draw_box  (const Cairo::RefPtr<Cairo::Context>& cr, double x, double y, double width, double height, double line_width, double red, double green, double blue, double alpha);
	void draw_rect (const Cairo::RefPtr<Cairo::Context>& cr, double x, double y, double width, double height, double red, double green, double blue, double alpha);

	bool on_mouse_motion (GdkEventMotion *event);
	bool on_mouse_click  (GdkEventButton *event);

private:
	DPContainer *m_c;

	void get_colour (std::string &name, double &red, double &green, double &blue);
	void draw_container (const Cairo::RefPtr<Cairo::Context>& cr, int x, int y, int width, int height, DPContainer *c);

};

#endif // _DRAWINGAREA_

