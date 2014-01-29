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
Properties::Properties (GfxContainerPtr c, Gtk::Window* w) :
	Gtk::Dialog ("dummy", false),
	close ("_Close", true),
	container(c)
{
	set_default_size (400, 400);

	if (w) {
		set_transient_for (*w);

		w->signal_delete_event().connect (sigc::mem_fun (*this, &Properties::on_parent_delete));
	}

	Gtk::Box* ca = get_content_area();

	ca->set_orientation (Gtk::ORIENTATION_VERTICAL);
	ca->pack_start (drawing,  false, false, 6);
	ca->pack_start (scrolled, true,  true);

	scrolled.add (textview);
	scrolled.set_hexpand (true);
	scrolled.set_vexpand (true);
	scrolled.set_policy (Gtk::PolicyType::POLICY_AUTOMATIC, Gtk::PolicyType::POLICY_AUTOMATIC);

	textview.set_editable (false);
	textview.set_cursor_visible (false);

	textbuffer = Gtk::TextBuffer::create();
	textview.set_buffer (textbuffer);

	Glib::RefPtr<Gtk::TextTag> bold = textbuffer->create_tag ("bold");
	bold->property_weight() = Pango::WEIGHT_BOLD;

	Glib::RefPtr<Gtk::TextTag> fixed = textbuffer->create_tag ("fixed");
	fixed->property_family() = "monospace";

	Glib::RefPtr<Gtk::TextTag> heading = textbuffer->create_tag ("heading");
	heading->property_weight() = Pango::WEIGHT_BOLD;
	heading->property_scale() = Pango::SCALE_LARGE;

#if 0
	Glib::RefPtr<Pango::Context> ctx = textview.get_pango_context();
	Glib::RefPtr<Pango::Layout> lo = Pango::Layout::create (ctx);
	lo->set_markup ("<b>mmmmmmmmmmmmm</b>");

	int width = 0;
	int height = 0;
	lo->get_pixel_size (width, height);
	std::cout << "width = " << width << ", height = " << height << std::endl;

	Pango::TabArray tabs (2, true);
	tabs.set_tab (0, Pango::TabAlign::TAB_LEFT, 20);
	tabs.set_tab (1, Pango::TabAlign::TAB_LEFT, width+30);
	textview.set_tabs (tabs);

	tabstop = width + 30;

	Gtk::TextBuffer::iterator iter = textbuffer->get_iter_at_offset(0);
#endif

#if 0
	iter = textbuffer->insert_with_tag (iter, "Properties:\n", "heading");
	iter = textbuffer->insert_with_tag (iter, "\ta", "bold");
	iter = textbuffer->insert (iter, "\tb\n");
	iter = textbuffer->insert_with_tag (iter, "\tiiiiii", "bold");
	iter = textbuffer->insert (iter, "\tmmmmmm\n");
	iter = textbuffer->insert_with_tag (iter, "\tmmmmmmmmmmmmm", "bold");
	iter = textbuffer->insert (iter, "\tiiiiii\n");

	iter = textbuffer->insert_with_tag (iter, "\t000000000000\n", "fixed");
	iter = textbuffer->insert_with_tag (iter, "\t 11111111111\n", "fixed");
	iter = textbuffer->insert_with_tag (iter, "\t  2222222222\n", "fixed");
	iter = textbuffer->insert_with_tag (iter, "\t   333333333\n", "fixed");
	iter = textbuffer->insert_with_tag (iter, "\t    44444444\n", "fixed");
	iter = textbuffer->insert_with_tag (iter, "\t     5555555\n", "fixed");
	iter = textbuffer->insert_with_tag (iter, "\t      666666\n", "fixed");
	iter = textbuffer->insert_with_tag (iter, "\t       77777\n", "fixed");
	iter = textbuffer->insert_with_tag (iter, "\t        8888\n", "fixed");
	iter = textbuffer->insert_with_tag (iter, "\t         999\n", "fixed");
#endif

	Gtk::ButtonBox* bb = get_action_area();
	bb->pack_start (close, Gtk::PACK_SHRINK);
	bb->set_border_width(5);
	bb->set_layout (Gtk::BUTTONBOX_END);

	close.signal_clicked().connect (sigc::mem_fun (*this, &Properties::on_close));

	drawing.set_data(c);

	//XXX "unnamed" if name is empty
	//XXX "DParted" from app
	set_title (c->name + " properties - DParted");

	signal_response().connect (sigc::mem_fun (*this, &Properties::on_dialog_response));

#if 0
	signal_realize().connect (sigc::mem_fun (*this, &Properties::my_realize));
#endif
	signal_show().connect (sigc::mem_fun (*this, &Properties::my_show));

	show_all();
}

/**
 * ~Properties
 */
Properties::~Properties()
{
	//LOG_TRACE;
}


/**
 * on_dialog_response
 */
void
Properties::on_dialog_response (int response_id)
{
	switch (response_id) {
		case Gtk::RESPONSE_DELETE_EVENT:
			//std::cout << "Delete clicked." << std::endl;
			break;
		default:
			std::cout << "Unexpected button clicked, response_id=" << response_id << std::endl;
			break;
	}

	delete this;
}

/**
 * on_parent_delete
 */
bool
Properties::on_parent_delete (GdkEventAny* event)
{
	std::cout << "parent delete\n";
	delete this;
	return false;
}

/**
 * on_close
 */
void
Properties::on_close (void)
{
#if 0
	tabstop += 20;

	Pango::TabArray tabs (2, true);
	tabs.set_tab (0, Pango::TabAlign::TAB_LEFT, 20);
	tabs.set_tab (1, Pango::TabAlign::TAB_LEFT, tabstop);
	textview.set_tabs (tabs);
#endif
	delete this;
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

/**
 * on_event
 */
bool
Properties::on_event (GdkEvent* event)
{
	std::cout << "type: " << event->type << std::endl;
	return false;
}

/**
 * my_realize
 */
void
Properties::my_realize (void)
{
	LOG_TRACE;
}

#endif
/**
 * my_show
 */
void
Properties::my_show (void)
{
	//LOG_TRACE;
	ContainerPtr c = container->get_container();
	if (!c) {
		std::cout << "no container" << std::endl;
		return;
	}

	std::vector<PPtr> props (c->get_all_props());

	Gtk::TextBuffer::iterator iter = textbuffer->get_iter_at_offset(0);
	iter = textbuffer->insert (iter, "Props:\n");
	for (auto p : props) {
		std::string line = p->owner + " : " + p->name + " = " + (std::string) *p + "\n";
		iter = textbuffer->insert (iter, line);
	}
}


