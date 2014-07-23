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

#include "container.h"
#include "visitor.h"

Container::Container (void)
{
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
	children.insert (child);

	child->parent = get_smart();
}

void
Container::delete_child (ContainerPtr& child)
{
	std::lock_guard<std::mutex> lock (mutex_children);
	for (auto it = children.begin(); it != children.end(); ++it) {
		if (*it == child) {
			children.erase (it);
			break;
		}
	}
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


