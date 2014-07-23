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

	virtual void add_child (ContainerPtr& child);
	int count_children (void);

	std::set<ContainerPtr> children;
	std::weak_ptr<Container> self;
	std::weak_ptr<Container> parent;

	std::string name;

protected:
	Container (void) = default;

	std::mutex mutex_children;
};

#endif // _CONTAINER_H_

