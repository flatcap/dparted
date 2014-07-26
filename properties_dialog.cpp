/* Copyright (c) 2014 Richard Russon (FlatCap)
 *
 * This file is part of DParted.
 *
 * DParted is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DParted is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DParted.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "properties_dialog.h"
#include "log.h"
#include "utils.h"

PropertiesDialog::PropertiesDialog (GfxContainerPtr c, Gtk::Window* w) :
	Gtk::Dialog ("dummy", false),
	close ("_Close", true),
	container(c)
{
	log_ctor ("ctor PropertiesDialog");
	set_default_size (400, 400);

	if (w) {
		set_transient_for (*w);

		w->signal_delete_event().connect (sigc::mem_fun (*this, &PropertiesDialog::on_parent_delete));
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
	iter = textbuffer->insert_with_tag (iter, "PropertiesDialog:\n", "heading");
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

	close.signal_clicked().connect (sigc::mem_fun (*this, &PropertiesDialog::on_close));

	drawing.set_data(c);

	//XXX "unnamed" if name is empty
	//XXX "DParted" from app
	set_title (c->name + " properties - DParted");

	signal_response().connect (sigc::mem_fun (*this, &PropertiesDialog::on_dialog_response));

#if 0
	signal_realize().connect (sigc::mem_fun (*this, &PropertiesDialog::my_realize));
#endif
	signal_show().connect (sigc::mem_fun (*this, &PropertiesDialog::my_show));

	show_all();
}

PropertiesDialog::~PropertiesDialog()
{
	LOG_TRACE;
	log_dtor ("dtor PropertiesDialog");
}


void
PropertiesDialog::on_dialog_response (int response_id)
{
	switch (response_id) {
		case Gtk::RESPONSE_DELETE_EVENT:
			log_debug ("Delete clicked.");
			break;
		default:
			log_debug ("Unexpected button clicked, response_id=%d", response_id);
			break;
	}

	delete this;
}

bool
PropertiesDialog::on_parent_delete (GdkEventAny* UNUSED(event))
{
	log_debug ("parent delete");
	delete this;
	return false;
}

void
PropertiesDialog::on_close (void)
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
bool
PropertiesDialog::on_draw (const Cairo::RefPtr<Cairo::Context>& cr)
{
	LOG_TRACE;
	// check if the GfxContainer has changed
	return Gtk::Dialog::on_draw (cr);
}

bool
PropertiesDialog::on_event (GdkEvent* event)
{
	log_debug ("type: %d", event->type);
	return false;
}

void
PropertiesDialog::my_realize (void)
{
	LOG_TRACE;
}

#endif

bool
prop_sort (const PPtr& lhs, const PPtr& rhs)
{
	if (lhs->owner != rhs->owner) {
		if (lhs->owner == "Container")		// Force Container to be first
			return true;

		if (rhs->owner == "Container")
			return false;

		return (lhs->owner < rhs->owner);
	}
	return (lhs->name < rhs->name);
}

void
PropertiesDialog::my_show (void)
{
	LOG_TRACE;
	ContainerPtr c = container->get_container();
	if (!c) {
		log_debug ("no container");
		return;
	}

	std::vector<PPtr> props (c->get_all_props());

	std::sort (props.begin(), props.end(), prop_sort);

	int tab1 = 0;
	int tab2 = 0;
	int width = 0;
	int height = 0;

	Glib::RefPtr<Pango::Context> ctx = textview.get_pango_context();
	Glib::RefPtr<Pango::Layout> lo = Pango::Layout::create (ctx);
	lo->set_text ("mmmm");
	lo->get_pixel_size (width, height);
	tab1 = width;		// set indent to 4em

	Gtk::TextBuffer::iterator iter = textbuffer->get_iter_at_offset(0);
	std::string owner;
	for (auto& p : props) {
		if (owner != p->owner) {
			owner = p->owner;
			iter = textbuffer->insert_with_tag (iter, owner + "\n", "heading");
		}

		//XXX tmp
		std::string value = (std::string) *p;
		if ((owner == "Container") && (p->name == "device") && value.empty()) {
			value = c->get_device_name();
		}
		iter = textbuffer->insert_with_tag (iter, "\t" + p->desc + "\t", "bold");
		iter = textbuffer->insert          (iter, value + "\n");

		lo->set_text ("<b>"+p->desc+"</b>");			// Expand the tabs to fit the data
		lo->get_pixel_size (width, height);
		if (width > tab2) {
			tab2 = width;
		}
	}

	Pango::TabArray tabs (2, true);
	tabs.set_tab (0, Pango::TabAlign::TAB_LEFT, tab1);
	tabs.set_tab (1, Pango::TabAlign::TAB_LEFT, tab1+tab2+10);
	textview.set_tabs (tabs);
}


