#ifndef _CONTAINER_H_
#define _CONTAINER_H_

#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include <sys/types.h>

class Container;

typedef std::shared_ptr<Container> ContainerPtr;

class Container
{
public:
	static ContainerPtr create (void);
	virtual ~Container() = default;

	virtual void add_child    (ContainerPtr& child);
	virtual void delete_child (ContainerPtr& child);

	void dump (int indent = 0);

	std::set<ContainerPtr> children;
	std::weak_ptr<Container> self;
	std::weak_ptr<Container> parent;

	std::string name;

protected:
	Container (void) = default;

	std::mutex mutex_children;
};

#endif // _CONTAINER_H_

