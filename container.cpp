#include "container.h"

ContainerPtr
Container::create (void)
{
	ContainerPtr p (new Container());
	p->self = p;
	return p;
}

void
Container::add_child (ContainerPtr child)
{
	std::lock_guard<std::recursive_mutex> lock (children_mutex);
	children.insert (child);

	child->parent = get_smart();
}

void
Container::delete_child (ContainerPtr child)
{
	std::lock_guard<std::recursive_mutex> lock (children_mutex);
	children.erase (child);
#if 0
	for (auto it = children.begin(); it != children.end(); ++it) {
		if (*it == child) {
			// printf ("delete: %p/%p\n", (void*) (*it).get(), (void*) child.get());
			children.erase (it);
			break;
		}
	}
#endif
}

void
Container::dump (int indent)
{
	std::lock_guard<std::recursive_mutex> lock (children_mutex);
	printf ("%*sContainer\n", 8*indent, "");

	for (const auto& c : children) {
		c->dump (indent+1);
	}
}


ContainerPtr
Container::pick_container (void)
{
	std::lock_guard<std::recursive_mutex> lock (children_mutex);
	int count = children.size();

	int pick = rand() % (count+1);
	if (pick == count) {
		return get_smart();
	}

	auto it = std::begin (children);
	std::advance (it, pick);
	return (*it)->pick_container();
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

