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

	child->parent = self;
}

void
Container::delete_child (ContainerPtr& child)
{
	std::lock_guard<std::mutex> lock (mutex_children);
	for (auto it = children.begin(); it != children.end(); ++it) {
		if (*it == child) {
			// printf ("delete: %p/%p\n", (void*) (*it).get(), (void*) child.get());
			children.erase (it);
			break;
		}
	}
}


void
Container::dump (int indent)
{
	printf ("%*s%s\n", 8*indent, "", name.c_str());

	for (const auto& c : children) {
		c->dump (indent+1);
	}
}
