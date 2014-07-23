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

