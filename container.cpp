#include <algorithm>
#include <atomic>
#include <cerrno>
#include <cstring>
#include <iterator>
#include <mutex>
#include <set>
#include <sstream>
#include <string>
#include <typeinfo>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "app.h"
#include "container.h"
#include "visitor.h"

std::atomic_ulong container_id = ATOMIC_VAR_INIT(1);

Container::Container (void)
{
	unique_id = std::atomic_fetch_add (&container_id, (unsigned long)1);
}

Container::~Container()
{
}

ContainerPtr
Container::create (void)
{
	ContainerPtr p (new Container());
	p->self = p;

	return p;
}


bool
Container::visit_children (Visitor& v)
{
	ContainerPtr cont = get_smart();
	if (!v.visit_enter(cont))
		return false;

	for (auto& child : children) {
		if (!child->accept(v))
			return false;
	}

	if (!v.visit_leave())
		return false;

	return true;
}

bool
Container::accept (Visitor& v)
{
	ContainerPtr c = get_smart();
	if (!v.visit(c))
		return false;

	return visit_children(v);
}


void
Container::add_child (ContainerPtr& child)
{
	std::lock_guard<std::mutex> lock (mutex_children);
	++seqnum;
	children.insert (child);

	child->parent = get_smart();
}

void
Container::delete_child (ContainerPtr& child)
{
	for (auto it = children.begin(); it != children.end(); ++it) {
		if (*it == child) {
			children.erase (it);
			break;
		}
	}
}

ContainerPtr
Container::find (const std::string& search)
{
	if (name == search) {
		ContainerPtr c = get_smart();
		return c;
	}

	std::size_t pos = search.find ("(0)");
	if (pos == (search.length() - 3)) {
		std::string search2 = search.substr (0, pos);
		if (name == search2) {
			ContainerPtr c = get_smart();
			return c;
		}
	}

	ContainerPtr item;

	for (auto& i : children) {
		if ((item = i->find (search)))
			break;
	}

	return item;
}


/**
 * operator<<
 */
std::ostream&
operator<< (std::ostream& stream, const ContainerPtr& c)
{
	stream << c->name;

	return stream;
}


bool
Container::is_a (const std::string& t)
{
	// Start with the most derived type
	for (auto it = type.rbegin(); it != type.rend(); ++it) {
		if ((*it) == t) {
			return true;
		}
	}

	return false;
}

void
Container::sub_type (const char* n)
{
	name = n;
	type.push_back (name);
}


std::set<ContainerPtr>&
Container::get_children (void)
{
	return children;
}


ContainerPtr
Container::get_smart (void)
{
	return self.lock();
}

ContainerPtr
Container::get_parent (void)
{
	return parent.lock();
}

ContainerPtr
Container::get_toplevel (void)
{
	ContainerPtr parent = get_smart();
	ContainerPtr p = get_parent();
	while (p) {
		parent = p;
		p = p->get_parent();
	}

	return parent;
}


std::string
Container::dump (void)
{
	std::stringstream s;
	s << this;
	return s.str();
}


