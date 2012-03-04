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
	//Override default signal handler:
	virtual bool on_draw (const Cairo::RefPtr<Cairo::Context>& cr);

	void draw_box  (const Cairo::RefPtr<Cairo::Context>& cr, double x, double y, double width, double height, double line_width, double red, double green, double blue, double alpha);
	void draw_rect (const Cairo::RefPtr<Cairo::Context>& cr, double x, double y, double width, double height, double red, double green, double blue, double alpha);

	void on_drag (const Glib::RefPtr<Gdk::DragContext>& context, Gtk::SelectionData& selection_data, guint info, guint time);
	void on_drop (const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, const Gtk::SelectionData& selection_data, guint info, guint time);

	bool on_mouse_motion (GdkEventMotion *event);
	bool on_mouse_click  (GdkEventButton *event);

	Glib::RefPtr<Gdk::Cursor> cursor;

private:
	DPContainer *m_c;

};

#endif // _DRAWINGAREA_

