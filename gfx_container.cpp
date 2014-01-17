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

#include "gfx_container.h"
#include "gui_app.h"
#include "log.h"

/**
 * GfxContainer
 */
GfxContainer::GfxContainer (void)
{
}

/**
 * ~GfxContainer
 */
GfxContainer::~GfxContainer()
{
}

/**
 * GfxContainer
 */
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


/**
 * get_container
 */
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

/**
 * sync
 */
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

/**
 * init
 */
bool
GfxContainer::init (ContainerPtr c)
{
	if (!c)
		return false;

	std::string path = c->get_path();
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

		colour     = process_colour (c);
		background = process_colour (bg);
		icon       = process_icon   (i);
		usage      = process_bool   (u);

#if 0
		std::cout << "Config\n";
		std::cout << "\tbackground: " << background     << std::endl;
		std::cout << "\tcolour:     " << colour         << std::endl;
		std::cout << "\tdisplay:    " << display        << std::endl;
		std::cout << "\ticon:       " << icon           << std::endl;
		std::cout << "\tlabel:      " << label_template << std::endl;
		std::cout << "\tusage:      " << usage          << std::endl;
#endif

	} catch (const std::string& e) {
		std::cout << "Exception: " << e << std::endl;
		exit (1);
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
bool find_subst (const std::string& text, std::string& tag, size_t& start, size_t& stop)
{
	//XXX on failure, point start at error
	const char* valid = "abcdefghijklmnopqrstuvwxyz_";
	size_t open  = std::string::npos;
	size_t close = std::string::npos;

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


/**
 * process_label
 */
std::string
GfxContainer::process_label (const std::string& label_template)
{
	std::string l = label_template;

	//std::cout << "Label: " << l << std::endl;
	ContainerPtr c = get_container();
	if (!c)
		return l;

	std::string tag;
	size_t start = std::string::npos;
	size_t stop  = std::string::npos;
	while (find_subst (l, tag, start, stop)) {
		std::string value = c->get_property (tag);
		if (value == tag) {		//XXX tmp
			break;
		}
		l.replace (start, stop-start+1, value);
	}

	//printf ("label = %s\n", l.c_str());

	return l;
}

/**
 * update_info
 */
bool GfxContainer::update_info (void)
{
	ContainerPtr c = get_container();
	if (!c)
		return false;

	// compare wrapper seqnums
	// compare theme seqnums

	return true;
}

/**
 * dump
 */
void
GfxContainer::dump (void)
{
	static int indent = 0;
	std::string tabs;

	if (indent > 0) {
		tabs.resize (indent, '\t');
	}

#if 0
	std::cout << tabs << "----------------------" << std::endl;
	std::cout << tabs << "display        = " << display        << std::endl;
	std::cout << tabs << "colour         = " << colour         << std::endl;
	std::cout << tabs << "background     = " << background     << std::endl;
	std::cout << tabs << "icon           = " << icon           << std::endl;
	std::cout << tabs << "label          = " << label          << std::endl;
	std::cout << tabs << "label_template = " << label_template << std::endl;
	std::cout << tabs << "bytes_size     = " << bytes_size     << std::endl;
	std::cout << tabs << "bytes_used     = " << bytes_used     << std::endl;
	std::cout << tabs << "usage          = " << usage          << std::endl;
	std::cout << tabs << "seqnum         = " << seqnum         << std::endl;
#endif

	indent++;
	for (auto c : children) {
		c->dump();
	}
	indent--;
}


/**
 * set_focus
 */
bool
GfxContainer::set_focus (bool focus)
{
	ContainerPtr c = get_container();
	//std::cout << "Focus: " << c << " = " << focus << std::endl;
	focussed = focus;
	return true;
}

/**
 * get_focus
 */
bool
GfxContainer::get_focus (void)
{
	return focussed;
}

/**
 * add_to_selection
 */
void
GfxContainer::add_to_selection (void)
{
}

/**
 * remove_from_selection
 */
void
GfxContainer::remove_from_selection (void)
{
}


/**
 * process_colour
 */
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

/**
 * process_icon
 */
Glib::RefPtr<Gdk::Pixbuf>
GfxContainer::process_icon (const std::string& str)
{
	Glib::RefPtr<Gdk::Pixbuf> pb;

	if (str.empty())
		return pb;

	pb = theme->get_icon (str);
	//std::cout << "icon: " << str << " " << pb << std::endl;
	//pb = Gdk::Pixbuf::create_from_file (str);

	return pb;
}

/**
 * process_bool
 */
bool
GfxContainer::process_bool (const std::string& str)
{
	if (str == "true")
		return true;

	return false;
}


/**
 * get_tooltip
 */
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

/**
 * mouse_event
 */
bool GfxContainer::mouse_event (void)
{
	//std::cout << __PRETTY_FUNCTION__ << std::endl;
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


/**
 * get_smart
 */
GfxContainerPtr
GfxContainer::get_smart (void)
{
	if (weak.expired()) {
		std::cout << "SMART\n";
		//XXX who created us?
		GfxContainerPtr c (this);
		std::cout << c << std::endl;
		weak = c;
	}
	return weak.lock();
}


/**
 * get_index
 */
int
GfxContainer::get_index (const GfxContainerPtr& me)
{
	if (!me)
		return -1;

	int size = children.size();
	for (int i = 0; i < size; i++) {
		if (children[i] == me) {
			return i;
		}
	}

	return -1;
}

/**
 * get_depth
 */
int
GfxContainer::get_depth (void)
{
	GfxContainerPtr up = parent.lock();
	int depth = 0;

	while (up) {
		up = up->parent.lock();
		depth++;
	}

	return depth;
}

/**
 * get_left
 */
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

/**
 * get_right
 */
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

