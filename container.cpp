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

ContainerPtr
Container::create (void)
{
	ContainerPtr p (new Container());
	p->self = p;

	return p;
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


void
Container::dump (int indent)
{
	printf ("%*s%s\n", 8*indent, "", name.c_str());

	for (const auto& c : children) {
		c->dump (indent+1);
	}
}
