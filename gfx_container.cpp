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
GfxContainer::GfxContainer (ContainerPtr c)
{
	if (!gui_app) {
		throw "no gui_app!";
	}

	theme = gui_app->get_theme();

	container = c;
	sync();
}

/**
 * ~GfxContainer
 */
GfxContainer::~GfxContainer()
{
}


/**
 * get_smart
 */
ContainerPtr
GfxContainer::get_smart (void)
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
	ContainerPtr c = get_smart();
	if (!c)
		return false;

	if (seqnum == c->seqnum)
		return true;

	init(c);
	for (auto child : c->get_children()) {
		GfxContainerPtr g = std::make_shared<GfxContainer>(child);
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
	std::string name = c->name;

	try {
		display        = theme->get_config (path, name, "display");
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
	ContainerPtr c = get_smart();
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
	ContainerPtr c = get_smart();
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
	return false;
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

