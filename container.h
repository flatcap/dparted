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

	void dump (int indent = 0);

	std::set<ContainerPtr> children;
	std::weak_ptr<Container> self;

protected:
	Container (void) = default;

	std::mutex children_mutex;
};

#endif // _CONTAINER_H_

