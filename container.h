/* Copyright (c) 2014 Richard Russon (FlatCap)
 *
 * This file is part of DParted.
 *
 * DParted is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DParted is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DParted.  If not, see <http://www.gnu.org/licenses/>.
 */

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

/**
 * class Container - Base class for all containers
 */
class Container
{
public:
	static ContainerPtr create (void);
	virtual ~Container();
	virtual bool accept (Visitor& v);

	virtual void add_child    (ContainerPtr& child);
	virtual void delete_child (ContainerPtr& child);

	virtual ContainerPtr find (const std::string& uuid);
	//XXX virtual std::vector<ContainerPtr> find_incomplete (void);

	virtual bool is_a (const std::string& type);

	virtual std::set<ContainerPtr>& get_children (void);

	ContainerPtr get_smart    (void);
	ContainerPtr get_parent   (void);
	ContainerPtr get_toplevel (void);

	template<class T>
	void add_child (std::shared_ptr<T>& child)
	{
		ContainerPtr c (child);
		add_child (c);
	}

	void sub_type (const char* name);
	std::string dump (void);

public:
	// properties
	std::string	name;

	int seqnum = 0;

protected:
	Container (void);

	std::weak_ptr<Container> self;
	std::weak_ptr<Container> parent;

	std::vector<std::string> type;

	bool visit_children (Visitor& v);

	friend std::ostream& operator<< (std::ostream& stream, const ContainerPtr& c);
	friend bool operator== (const ContainerPtr& lhs, const ContainerPtr& rhs);

	std::set<ContainerPtr> children;
	std::mutex mutex_children;

	std::vector<std::string> more_props;

private:
	void insert (std::uint64_t offset, std::uint64_t size, void* ptr);

	std::uint64_t unique_id = 0;
};

inline bool
operator== (const ContainerPtr& lhs, const ContainerPtr& rhs)
{
	return (lhs->unique_id == rhs->unique_id);
}

#endif // _CONTAINER_H_

