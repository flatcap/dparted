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

#ifndef _PROPERTIES_H_
#define _PROPERTIES_H_

#include <gtkmm/dialog.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/textview.h>

#include "gfx_container.h"
#include "prop_drawing_area.h"

/**
 * class Properties
 */
class Properties : public Gtk::Dialog
{
public:
	Properties (GfxContainerPtr c, Gtk::Window* w);
	virtual ~Properties();

protected:
#if 0
	virtual bool on_draw (const Cairo::RefPtr<Cairo::Context>& cr);
	virtual bool on_event (GdkEvent* event);
	void my_realize (void);
#endif
	void my_show (void);

	void on_dialog_response (int response_id);
	bool on_parent_delete (GdkEventAny* event);
	void on_close (void);

	PropDrawingArea drawing;
	Gtk::ScrolledWindow scrolled;
	Gtk::TextView textview;
	Glib::RefPtr<Gtk::TextBuffer> textbuffer;
	Gtk::Button close;

	GfxContainerPtr container;
};


#endif // _PROPERTIES_H_

