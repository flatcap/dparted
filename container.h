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
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include <sys/types.h>

#include "property.h"
#include "mmap.h"

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

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

	virtual void add_child      (ContainerPtr& child);
	virtual void just_add_child (ContainerPtr& child);
	virtual void delete_child   (ContainerPtr& child);
	virtual void move_child     (ContainerPtr& child, std::uint64_t offset, std::uint64_t size);

	virtual int      get_fd (void);
	virtual std::uint64_t get_block_size (void);
	virtual std::string   get_device_name (void);
	virtual std::uint64_t get_parent_offset (void);
	virtual std::uint64_t get_device_space (std::map<std::uint64_t, std::uint64_t>& spaces);

	virtual std::uint64_t get_size_total (void);
	virtual std::uint64_t get_size_used (void);
	virtual std::uint64_t get_size_free (void);

	virtual ContainerPtr find (const std::string& uuid);
	//XXX virtual std::vector<ContainerPtr> find_incomplete (void);

	virtual std::uint8_t* get_buffer (std::uint64_t offset, std::uint64_t size);
	virtual void close_buffer (std::uint8_t* buffer, std::uint64_t size);

	virtual bool is_a (const std::string& type);

	struct compare
	{
		bool operator() (const ContainerPtr a, const ContainerPtr b)
		{
			std::uint64_t da = (a->device_major << 10) + a->device_minor;
			std::uint64_t db = (b->device_major << 10) + b->device_minor;
			if (da != db)
				return (da < db);

			int x = a->name.compare (b->name);
			if (x != 0)
				return (x < 0);

			if (a->parent_offset != b->parent_offset)
				return (a->parent_offset < b->parent_offset);

			return ((void*) a.get() < (void*) b.get());
		}
	};

	virtual std::set<ContainerPtr, compare>& get_children (void);

	std::string get_path (void);

	ContainerPtr get_smart (void);

	std::vector<std::string> get_prop_names (void);
	PPtr get_prop (const std::string& name);
	std::vector<PPtr> get_all_props (void);

	template<class T>
	void add_child (std::shared_ptr<T>& child)
	{
		ContainerPtr c (child);
		add_child(c);
	}

	template<typename T>
	void
	declare_prop (const char* owner, const char* name, T& var, const char* desc, int flags = 0)
	{
		if (flags & BaseProperty::Flags::Size) {
			// Create a fake property
			std::string human (name);
			human += "_human";
			PPtr pv (new PropVar<T> (owner, human.c_str(), var, desc, flags));
			props[human] = pv;
			flags &= ~BaseProperty::Flags::Size;	// Turn off the size flag
		}

		PPtr pv (new PropVar<T> (owner, name, var, desc, flags));
		props[name] = pv;
	}

	template<typename T>
	void
	declare_prop (const char* owner, const char* name, std::function<T(void)> fn, const char* desc, int flags = 0)
	{
		if (flags & BaseProperty::Flags::Size) {
			// Create a fake property
			std::string human (name);
			human += "_human";
			PPtr pp (new PropFn<T> (owner, human.c_str(), fn, desc, flags));
			props[human] = pp;
			flags &= ~BaseProperty::Flags::Size;	// Turn off the size flag
		}

		PPtr pp (new PropFn<T> (owner, name, fn, desc, flags));
		props[name] = pp;
	}

	void sub_type (const char* name);

public:
	//properties
	std::string	name;
	std::string	uuid;

	std::string	device;	// These don't belong here, but the alternative is multiple inheritance
	dev_t		device_major = 0;
	dev_t		device_minor = 0;
	int		fd = -1;

	std::uint64_t	parent_offset = 0;

	std::uint64_t	block_size = 0;
	std::uint64_t	bytes_size = 0;
	std::uint64_t	bytes_used = 0;

	ContainerPtr	whole;

	std::weak_ptr<Container> parent;

	std::vector<std::string> type;	//XXX move to protected

	bool		 missing = false;

	int seqnum = 123;

protected:
	Container (void);

	std::weak_ptr<Container> weak;	//XXX private?

	bool visit_children (Visitor& v);

	friend std::ostream& operator<< (std::ostream& stream, const ContainerPtr& c);

	//MmapSet	mmaps;
	MmapPtr	device_mmap;

	std::map<std::string,PPtr> props;
	std::set<ContainerPtr, compare> children;

	// Helper functions
	long        get_bytes_free (void);
	std::string get_device_major_minor (void);
	std::string get_device_short (void);
	std::string get_name_default (void);
	std::string get_type (void);
	std::string get_type_long (void);
	std::string get_uuid_short (void);

private:
	void insert (long offset, long size, void* ptr);

};

#endif // _CONTAINER_H_

