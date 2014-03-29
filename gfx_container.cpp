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

#include "gfx_container.h"
#include "gui_app.h"
#include "log.h"
#include "log_trace.h"

GfxContainer::GfxContainer (void)
{
}

GfxContainer::~GfxContainer()
{
}

GfxContainerPtr
GfxContainer::create (GfxContainerPtr p, ContainerPtr c)
{
	GfxContainerPtr g (new GfxContainer());
	g->weak = g;

	g->parent = p;
	g->container = c;

	if (!gui_app) {
		throw "no gui_app!";
	}

	g->theme = gui_app->get_theme();

	g->sync();

	return g;
}


ContainerPtr
GfxContainer::get_container (void)
{
	ContainerPtr c = container.lock();
	if (!c) {
		// Object has gone away
		children.clear();
		seqnum = -1;

		display.clear();
		icon.reset();
		label.clear();
		label_template.clear();

		bytes_size = 0;
		bytes_used = 0;

		usage = false;

		focussed = false;
		selected = false;
	}

	return c;
}

bool
GfxContainer::sync (void)
{
	ContainerPtr c = get_container();
	if (!c)
		return false;

	if (seqnum == c->seqnum)
		return true;

	init(c);
	for (auto child : c->get_children()) {
		GfxContainerPtr g = GfxContainer::create (get_smart(), child);
		children.push_back(g);
	}

	return true;
}

bool
GfxContainer::init (ContainerPtr c)
{
	if (!c)
		return false;

	std::string path = c->get_type_long();
	name = c->name;
	type = c->type.back();
	device = c->get_device_name();

	try {
		display        = theme->get_config (path, name, "display");
		treeview       = theme->get_config (path, name, "treeview");
		label_template = theme->get_config (path, name, "label");

		std::string c  = theme->get_config (path, name, "colour");
		std::string bg = theme->get_config (path, name, "background");
		std::string i  = theme->get_config (path, name, "icon");
		std::string u  = theme->get_config (path, name, "usage");

		colour     = process_colour(c);
		background = process_colour (bg);
		icon       = process_icon  (i);
		usage      = process_bool  (u);

#if 0
		log_debug ("Config\n");
		log_debug ("\tbackground: %s\n", background);
		log_debug ("\tcolour:     %s\n", colour);
		log_debug ("\tdisplay:    %s\n", display.c_str());
		log_debug ("\ticon:       %s\n", icon);
		log_debug ("\tlabel:      %s\n", label_template.c_str());
		log_debug ("\tusage:      %d\n", usage);
#endif

	} catch (const std::string& e) {
		log_debug ("Exception: %s\n", e.c_str());
		exit(1);
	}

	label = process_label (label_template);

	bytes_size = c->bytes_size;
	bytes_used = c->bytes_used;
	parent_offset = c->parent_offset;

	return true;
}


/**
 * find_subst - get the position of a {tag}
 */
bool
find_subst (const std::string& text, std::string& tag, std::size_t& start, std::size_t& stop)
{
	//XXX on failure, point start at error
	const char* valid = "abcdefghijklmnopqrstuvwxyz_";
	std::size_t open  = std::string::npos;
	std::size_t close = std::string::npos;

	open = text.find ('{');
	if (open == std::string::npos) {
		//log_debug ("nothing to substitute\n");
		return false;
	}

	close = text.find_first_not_of (valid, open+1);
	if (close == std::string::npos) {
		log_error ("missing close brace\n");
		return false;
	}

	if (text[close] != '}') {
		log_error ("invalid tag name\n");
		return false;
	}

	if (close == (open+1)) {
		log_error ("missing tag\n");
		return false;
	}

	tag   = text.substr (open+1, close-open-1);
	start = open;
	stop  = close;

	return true;
}


std::string
GfxContainer::process_label (const std::string& label_template)
{
	std::string l = label_template;

	//log_debug ("Label: %s\n", l.c_str());
	ContainerPtr c = get_container();
	if (!c)
		return l;

	std::string tag;
	std::size_t start = std::string::npos;
	std::size_t stop  = std::string::npos;
	while (find_subst (l, tag, start, stop)) {
		PPtr prop = c->get_prop (tag);
		std::string value = (std::string) *prop;
		if (value == tag) {		//XXX error tag not translated
			break;
		}
		l.replace (start, stop-start+1, value);
	}

	//log_debug ("label = %s\n", l.c_str());

	return l;
}

bool GfxContainer::update_info (void)
{
	ContainerPtr c = get_container();
	if (!c)
		return false;

	// compare wrapper seqnums
	// compare theme seqnums

	return true;
}

void
GfxContainer::dump2 (void)
{
	static int indent = 0;
	std::string tabs;

	if (indent > 0) {
		tabs.resize (indent, '\t');
	}

#if 0
	log_debug ("%s----------------------", tabs.c_str);
	log_debug ("%sdisplay        = ", tabs.c_str(), display);
	log_debug ("%scolour         = ", tabs.c_str(), colour);
	log_debug ("%sbackground     = ", tabs.c_str(), background);
	log_debug ("%sicon           = ", tabs.c_str(), icon);
	log_debug ("%slabel          = ", tabs.c_str(), label);
	log_debug ("%slabel_template = ", tabs.c_str(), label_template);
	log_debug ("%sbytes_size     = ", tabs.c_str(), bytes_size);
	log_debug ("%sbytes_used     = ", tabs.c_str(), bytes_used);
	log_debug ("%susage          = ", tabs.c_str(), usage);
	log_debug ("%sseqnum         = ", tabs.c_str(), seqnum);
#endif

	++indent;
	for (auto c : children) {
		c->dump();
	}
	indent--;
}


bool
GfxContainer::set_focus (bool focus)
{
	ContainerPtr c = get_container();
	//log_debug ("Focus: %s = %d\n", c->dump(), focus);
	focussed = focus;
	return true;
}

bool
GfxContainer::get_focus (void)
{
	return focussed;
}

void
GfxContainer::add_to_selection (void)
{
}

void
GfxContainer::remove_from_selection (void)
{
}


Gdk::RGBA
GfxContainer::process_colour (const std::string& str)
{
	Gdk::RGBA r;

	if (r.set (str)) {
		return r;
	}

	if (str == "none") {
		r.set ("rbga(0,0,0,0)");
		return r;
	}

	throw "bad colour: " + str;
}

Glib::RefPtr<Gdk::Pixbuf>
GfxContainer::process_icon (const std::string& str)
{
	Glib::RefPtr<Gdk::Pixbuf> pb;

	if (str.empty())
		return pb;

	pb = theme->get_icon (str);
	//log_debug ("icon: %s %p\n", str.c_str(), (void*) pb.operator->());
	//pb = Gdk::Pixbuf::create_from_file (str);

	return pb;
}

bool
GfxContainer::process_bool (const std::string& str)
{
	if (str == "true")
		return true;

	return false;
}


std::string
GfxContainer::get_tooltip (void)
{
	ContainerPtr c = get_container();
	if (!c)
		return "";

	std::string tt;

	tt = c->name + " : " + c->type.back();

	return tt;
}

bool GfxContainer::mouse_event (void)
{
	//LOG_TRACE;
	return false;
}


/**
 * operator<<
 */
std::ostream&
operator<< (std::ostream& stream, const GfxContainerPtr& g)
{
	ContainerPtr c = g->get_container();
	if (c) {
		stream << c;
	} else {
		stream << "[Empty]";
	}

	return stream;
}


GfxContainerPtr
GfxContainer::get_smart (void)
{
	if (weak.expired()) {
		log_debug ("SMART\n");
		//XXX who created us? - code error
		GfxContainerPtr c (this);
		log_debug ("%s\n", c->dump());
		weak = c;
	}
	return weak.lock();
}


int
GfxContainer::get_index (const GfxContainerPtr& me)
{
	if (!me)
		return -1;

	int size = children.size();
	for (int i = 0; i < size; ++i) {
		if (children[i] == me) {
			return i;
		}
	}

	return -1;
}

int
GfxContainer::get_depth (void)
{
	GfxContainerPtr up = parent.lock();
	int depth = 0;

	while (up) {
		up = up->parent.lock();
		++depth;
	}

	return depth;
}

GfxContainerPtr
GfxContainer::get_left (void)
{
	GfxContainerPtr empty;
	GfxContainerPtr me = get_smart();

	GfxContainerPtr gparent = parent.lock();
	if (!gparent)				// No parent -> no siblings
		return empty;

	int index = gparent->get_index (me);
	if (index < 0)				// I'm not my parent's child
		return empty;

	if (index == 0)				// I'm the first sibling
		return gparent;

	GfxContainerPtr prev = gparent->children[index-1];
	if (!prev)				// Empty child!
		return empty;

	int size;
	while ((size = prev->children.size()) > 0) {
		prev = prev->children[size-1];	// Find last (grand)*child
	}

	return prev;
}

GfxContainerPtr
GfxContainer::get_right (void)
{
	GfxContainerPtr empty;
	GfxContainerPtr me = get_smart();

	GfxContainerPtr gparent = parent.lock();
	if (!gparent)				// No parent -> no siblings
		return empty;

	if (me->children.size() > 0)		// Descend to children
		return me->children[0];

	int last_sib;
	do {
		int index = gparent->get_index (me);
		if (index < 0)				// I'm not my parent's child
			return empty;

		last_sib = gparent->children.size()-1;
		if (index < last_sib) {
			return gparent->children[index+1];	// Next sibling
		}
		me = gparent;
		gparent = gparent->parent.lock();
	}
	while (gparent);

	return empty;
}

const char*
GfxContainer::dump (void)
{
	std::stringstream s;
	s << this;
	return s.str().c_str();
}


