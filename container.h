#ifndef _CONTAINER_H_
#define _CONTAINER_H_

#include <memory>
#include <mutex>
#include <set>

class Container;

typedef std::shared_ptr<Container> ContainerPtr;

class Container
{
public:
	static ContainerPtr create (void);
	virtual ~Container() = default;

	virtual void add_child (ContainerPtr child);
	virtual void delete_child (ContainerPtr child);

	ContainerPtr pick_container (void);
	ContainerPtr get_parent (void);
	ContainerPtr get_smart (void);

	void dump (int indent = 0);

protected:
	Container (void) = default;

	std::set<ContainerPtr> children;
	std::recursive_mutex children_mutex;

	std::weak_ptr<Container> self;
	std::weak_ptr<Container> parent;
};

#endif // _CONTAINER_H_

