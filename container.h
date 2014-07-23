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
class Visitor;
struct Action;

typedef std::shared_ptr<Container> ContainerPtr;

class Container
{
public:
	static ContainerPtr create (void);
	virtual ~Container();
	virtual bool accept (Visitor& v);

	virtual void add_child    (ContainerPtr& child);
	virtual void delete_child (ContainerPtr& child);

	virtual std::set<ContainerPtr>& get_children (void);

	ContainerPtr get_smart    (void);
	ContainerPtr get_parent   (void);
	ContainerPtr get_toplevel (void);

	std::string	name;

protected:
	Container (void);

	std::weak_ptr<Container> self;
	std::weak_ptr<Container> parent;

	bool visit_children (Visitor& v);

	std::set<ContainerPtr> children;
	std::mutex mutex_children;

	void insert (std::uint64_t offset, std::uint64_t size, void* ptr);
};

#endif // _CONTAINER_H_

