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

#include "properties.h"
#include "log_trace.h"

/**
 * Properties
 */
Properties::Properties (GfxContainerPtr c) :
	container (c)
{
	set_default_size (400, 400);

	Gtk::Box* ca = get_content_area();

	ca->set_orientation (Gtk::ORIENTATION_VERTICAL);
	ca->pack_start (da, false, false);

	da.set_data (c);

	//XXX "unnamed" if name is empty
	//XXX "DParted" from app
	set_title (c->name + " properties - DParted");

#if 0
	Gtk::Label l ("hello");
	Gtk::Expander e ("advanced");
	Gtk::Button g ("C_opy", true);
	Gtk::Button b ("_Close", true);

	Gtk::Box box;
	Gtk::Entry la; la.set_text ("a");
	Gtk::Entry lb; lb.set_text ("b");
	Gtk::Entry lc; lc.set_text ("c");
	Gtk::Entry ld; ld.set_text ("d");
	Gtk::Entry le; le.set_text ("e");

	e.add (box);
	box.pack_start (la);
	box.pack_start (lb);
	box.pack_start (lc);
	box.pack_start (ld);
	box.pack_start (le);

	//Gtk::Widget* w;
	//w = Gtk::manage (new Gtk::Label  ("hello"));
	d.get_content_area()->pack_start (l);
	d.get_content_area()->pack_start (e);
	d.get_action_area()->pack_end (g);
	d.get_action_area()->pack_end (b);

	//add (Gtk::manage (*d));

	d.show_all();
	d.run();
#endif
	show_all();
}

/**
 * ~Properties
 */
Properties::~Properties()
{
}


#if 0
/**
 * on_draw
 */
bool
Properties::on_draw (const Cairo::RefPtr<Cairo::Context>& cr)
{
	LOG_TRACE;
	//XXX check if the GfxContainer has changed
	return Gtk::Dialog::on_draw (cr);
}

#endif
