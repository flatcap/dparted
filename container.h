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

	virtual std::set<ContainerPtr>& get_children (void);

	ContainerPtr get_smart    (void);
	ContainerPtr get_parent   (void);
	ContainerPtr get_toplevel (void);

	std::string	name;

	void dump (int indent = 0);

protected:
	Container (void) = default;

	std::weak_ptr<Container> self;
	std::weak_ptr<Container> parent;

	std::set<ContainerPtr> children;
	std::mutex mutex_children;

	void insert (std::uint64_t offset, std::uint64_t size, void* ptr);
};

#endif // _CONTAINER_H_

