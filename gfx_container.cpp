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


#include <cstdlib>
#include <ctime>

#include "gfx_container.h"
#include "gui_app.h"
#include "log.h"
#include "utils.h"

GfxContainer::GfxContainer (void)
{
	LOG_CTOR;
}

GfxContainer::~GfxContainer()
{
	LOG_DTOR;
}

GfxContainerPtr
GfxContainer::create (GfxContainerPtr p, ContainerPtr c)
{
	return_val_if_fail (c, nullptr);

	GfxContainerPtr g (new GfxContainer());
	g->self = g;

	g->parent = p;
	g->container = c;

	if (!gui_app) {
		throw "no gui_app!";
	}

	g->theme = gui_app->get_theme();

	c->add_listener(g);		// Container listener
	gui_app->add_listener(g);	// Theme listener

	g->sync();
	c->add_string_prop (std::string("gfx"), std::string("colour"), g->colour2);

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

	if (!c) {
		log_error ("NO CONTAINER");
		//XXX need to delete myself from parent
		//XXX notify delete to OUR listeners
		return false;
	}

	if (seqnum != c->seqnum) {
		init(c);	//XXX rename this
	}

	for (auto& child : c->get_children()) {
		GfxContainerPtr gfx = find_child (child);
		if (gfx) {
			log_code ("EXISTING CHILD");
			gfx->sync();
		} else {
			GfxContainerPtr g = GfxContainer::create (get_smart(), child);
			std::lock_guard<std::mutex> lock (mutex_children);
			insert_child(g);
		}
	}

	return true;
}

bool
GfxContainer::init (ContainerPtr c)
{
	return_val_if_fail (c, false);

	std::string path = c->get_type_long();
	name = c->name;
	type = c->get_type();
	device = c->get_device_name();

	try {
		display        = theme->get_config (path, name, "display");
		treeview       = theme->get_config (path, name, "treeview");
		label_template = theme->get_config (path, name, "label");

		std::string c  = theme->get_config (path, name, "colour");
		std::string bg = theme->get_config (path, name, "background");
		std::string i  = theme->get_config (path, name, "icon");
		std::string u  = theme->get_config (path, name, "usage");

		background = process_colour (bg);
		colour     = process_colour (c);
		colour2    = c;
		icon       = process_icon   (i);
		usage      = process_bool   (u);
	} catch (const std::string& e) {
		log_error ("Exception: %s", SP(e));
	}

	label = process_label (label_template);

	bytes_size = c->bytes_size;
	bytes_used = c->bytes_used;
	parent_offset = c->parent_offset;
	seqnum = c->seqnum;

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
		log_debug ("nothing to substitute");
		return false;
	}

	close = text.find_first_not_of (valid, open+1);
	if (close == std::string::npos) {
		log_error ("missing close brace");
		return false;
	}

	if (text[close] != '}') {
		log_error ("invalid tag name");
		return false;
	}

	if (close == (open+1)) {
		log_error ("missing tag");
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

	log_debug ("Label: %s", SP(l));
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

	log_debug ("label = %s", SP(l));

	return l;
}

bool
GfxContainer::update_info (void)
{
	ContainerPtr c = get_container();
	if (!c)
		return false;

	if (seqnum != c->seqnum) {
		// sync();
		return true;
	}

	for (auto& i : children) {
		i->update_info();
	}

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

	log_info ("%s----------------------", SP(tabs));
	log_info ("%sdisplay        = %s",    SP(tabs), SP(display));
	log_info ("%slabel          = %s",    SP(tabs), SP(label));
	log_info ("%slabel_template = %s",    SP(tabs), SP(label_template));
	log_info ("%sbytes_size     = %ld",   SP(tabs), bytes_size);
	log_info ("%sbytes_used     = %ld",   SP(tabs), bytes_used);
	log_info ("%susage          = %d",    SP(tabs), usage);
	log_info ("%sseqnum         = %d",    SP(tabs), seqnum);

	++indent;
	for (auto& c : children) {
		c->dump2();
	}
	--indent;
}

void
GfxContainer::dump3 (void)
{
	static int indent = 0;
	std::string tabs;

	if (indent > 0) {
		tabs.resize (indent, '\t');
	}

	ContainerPtr c = container.lock();
	GfxContainerPtr g = get_smart();
	log_info ("%s----------------------", SP(tabs));
	log_info ("%sgfx            = %p",    SP(tabs), VP(g));
	log_info ("%sname           = %s",    SP(tabs), SP(name));
	log_info ("%scontainer      = %p",    SP(tabs), VP(c));
	if (c) {
		log_info ("%scont name      = %s",  SP(tabs), SP(c->get_name_default()));
		log_info ("%soffset         = %ld", SP(tabs), c->parent_offset);
		log_info ("%ssize           = %ld", SP(tabs), c->bytes_size);
		log_info ("%slisteners      = %d",  SP(tabs), c->count_listeners());
	}

	++indent;
	for (auto& c : children) {
		c->dump3();
	}
	--indent;
}


bool
GfxContainer::set_focus (bool focus)
{
	ContainerPtr c = get_container();
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
	log_debug ("icon: %s %p", SP(str), (void*) pb.operator->());
	// pb = Gdk::Pixbuf::create_from_file (str);

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

	tt = c->name + " : " + c->get_type();

	return tt;
}

bool
GfxContainer::mouse_event (void)
{
	LOG_TRACE;
	return false;
}


/**
 * operator<<
 */
std::ostream&
operator<< (std::ostream& stream, const GfxContainerPtr& g)
{
	return_val_if_fail (g, stream);

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
	return_val_if_fail (!self.expired(), nullptr);

	return self.lock();
}

ContainerListenerPtr
GfxContainer::get_listener (void)
{
	return_val_if_fail (!self.expired(), nullptr);

	return std::dynamic_pointer_cast<IContainerListener> (self.lock());
}

GfxContainerPtr
GfxContainer::get_parent (void)
{
	return parent.lock();
}

GfxContainerPtr
GfxContainer::get_top_level (void)
{
	GfxContainerPtr parent = get_smart();
	GfxContainerPtr p = get_parent();
	while (p) {
		parent = p;
		p = p->get_parent();
	}

	return parent;
}

bool
GfxContainer::is_top_level (void)
{
	GfxContainerPtr p = parent.lock();
	return (!p);
}


int
GfxContainer::get_index (const GfxContainerPtr& me)
{
	return_val_if_fail (me, -1);

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
	GfxContainerPtr me = get_smart();

	GfxContainerPtr gparent = parent.lock();
	if (!gparent)				// No parent -> no siblings
		return nullptr;

	int index = gparent->get_index (me);
	if (index < 0)				// I'm not my parent's child
		return nullptr;

	if (index == 0)				// I'm the first sibling
		return gparent;

	GfxContainerPtr prev = gparent->children[index-1];
	if (!prev)				// Empty child!
		return nullptr;

	int size;
	while ((size = prev->children.size()) > 0) {
		prev = prev->children[size-1];	// Find last (grand)*child
	}

	return prev;
}

GfxContainerPtr
GfxContainer::get_right (void)
{
	GfxContainerPtr me = get_smart();

	GfxContainerPtr gparent = parent.lock();
	if (!gparent)				// No parent -> no siblings
		return nullptr;

	if (me->children.size() > 0)		// Descend to children
		return me->children[0];

	int last_sib;
	do {
		int index = gparent->get_index (me);
		if (index < 0)				// I'm not my parent's child
			return nullptr;

		last_sib = gparent->children.size()-1;
		if (index < last_sib) {
			return gparent->children[index+1];	// Next sibling
		}
		me = gparent;
		gparent = gparent->parent.lock();
	} while (gparent);

	return nullptr;
}

std::string
GfxContainer::dump (void)
{
	std::stringstream ss;
	ss << this;
	return ss.str();
}


void
GfxContainer::add_listener (GfxContainerListenerPtr& gcl)
{
	return_if_fail (gcl);

	log_listener ("GfxContainer %p add listener: %p", (void*) this, VP(gcl));
	gfx_container_listeners.push_back(gcl);
}

GfxContainerPtr
GfxContainer::find (const ContainerPtr& cont)
{
	if (!cont)
		return {};

	ContainerPtr c = container.lock();
	if (c == cont)
		return get_smart();

	for (auto& i : children) {
		GfxContainerPtr g = i->find (cont);
		if (g) {
			return g;
		}
	}

	return {};
}

GfxContainerPtr
GfxContainer::find_child (ContainerPtr search)
{
	if (!search)
		return {};

	for (auto& gfx : children) {
		ContainerPtr child = gfx->container.lock();
		if (child == search) {
			return gfx;
		}
	}

	return {};
}

void
GfxContainer::container_added (const ContainerPtr& cont)
{
	return_if_fail (cont);

	// get_top_level()->dump3();
	// LOG_TRACE;

	GfxContainerPtr gparent = get_smart();
	if (!gparent) {
		log_error ("add: no parent");
		return;
	}

	log_code ("GFX container_added: %s(%p) to %s(%p)", SP(cont->name), VP(cont), SP(name), VP(gparent));

	GfxContainerPtr existing = find_child (cont);
	if (existing) {
		log_code ("Gfx: child already exists : %s(%p) : %s(%p)", SP(existing->name), VP(existing), SP(cont->get_name_default()), VP(cont));
		//XXX do we need to sync?  might be a duplicate notification (recursive?)
		return;
	}

	GfxContainerPtr gchild = GfxContainer::create (get_smart(), cont);
	if (!gchild) {
		log_error ("couldn't create child");
		return;
	}

	{
	std::lock_guard<std::mutex> lock (mutex_children);
	gparent->insert_child (gchild);
	}

	for (auto& i : gfx_container_listeners) {
		GfxContainerListenerPtr p = i.lock();
		if (p) {
			log_listener ("Added child %p to GfxContainer %p", VP(gchild), VP(gparent));
			p->gfx_container_added (gchild);
		} else {
			log_code ("remove gfx listener from the collection");	//XXX remove it from the collection
		}
	}
}

void
GfxContainer::container_changed (const ContainerPtr& after)
{
	return_if_fail (after);

	// get_top_level()->dump3();
	std::string name;
	ContainerPtr c = container.lock();
	if (!c) {
		log_error ("Container went away");
		return;
	}

	GfxContainerPtr gchild  = get_smart();

	c->remove_listener (gchild);	// Transfer our listener to the new container
	after->add_listener (gchild);

	name = c->get_name_default();

	log_info ("container_changed %s(%p) : %s(%p) -> %s(%p)", SP(name), VP(c), SP(c->get_name_default()), VP(gchild), SP(after->get_name_default()), VP(after));

	container = after;

	parent_offset = after->parent_offset;	//XXX need to re-sync the entire object
	bytes_size    = after->bytes_size;

	// Now notify all of OUR listeners
	for (auto& i : gfx_container_listeners) {
		GfxContainerListenerPtr p = i.lock();
		if (p) {
			log_listener ("Change child %p to %p", VP(gchild), VP(after));
			p->gfx_container_changed (gchild);
		} else {
			log_code ("remove gfx listener from the collection");	//XXX remove it from the collection
		}
	}

	// GfxContainerPtr top = get_top_level();
	// top->dump3();
}

void
GfxContainer::container_deleted (const ContainerPtr& child)
{
	return_if_fail (child);

	// get_top_level()->dump3();
	// LOG_TRACE;

	GfxContainerPtr gparent = get_smart();
	if (!gparent) {
		log_error ("delete: no parent");
		return;
	}

	log_code ("GFX container_deleted: %s(%p) from %s(%p)", SP(child->name), VP(child), SP(name), VP(gparent));

	GfxContainerPtr gchild = find_child (child);
	if (!gchild) {
		log_error ("delete: no child");
		return;
	}

	child->remove_listener (gparent);

	for (auto& i : gfx_container_listeners) {
		GfxContainerListenerPtr p = i.lock();
		if (p) {
			log_listener ("Deleted child %p from GfxContainer %p", VP(gchild), VP(gparent));
			p->gfx_container_deleted (gchild);
		} else {
			log_code ("remove gfx listener from the collection");	//XXX remove it from the collection
		}
	}

	std::lock_guard<std::mutex> lock (gparent->mutex_children);
	children.erase (
		std::remove (
			std::begin (children),
			std::end   (children),
			gchild),
		std::end (children));
}

bool compare (const GfxContainerPtr& a, const GfxContainerPtr& b)
{
	return_val_if_fail (a, true);
	return_val_if_fail (b, true);

	if (a->parent_offset != b->parent_offset)
		return (a->parent_offset < b->parent_offset);

	ContainerPtr ca = a->get_container();
	ContainerPtr cb = b->get_container();
	if (ca && cb) {
		std::uint64_t da = (ca->device_major << 10) + ca->device_minor;
		std::uint64_t db = (cb->device_major << 10) + cb->device_minor;
		if (da != db) {
			return (da < db);
		}
	}

	int x = a->name.compare (b->name);
	if (x != 0)
		return (x < 0);

	return ((void*) a.get() > (void*) b.get());
}

void
GfxContainer::insert_child (GfxContainerPtr& child)
{
	auto end = std::end (children);
	for (auto it = std::begin (children); it != end; ++it) {
		if (compare (child, *it)) {
			children.insert (it, child);
			return;
		}
	}

	children.push_back (child);
}

void
GfxContainer::theme_changed (const ThemePtr& new_theme)
{
	LOG_TRACE;
	theme = new_theme;	//XXX force dropping of cached values
}


