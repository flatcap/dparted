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
struct Action;
class Visitor;

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
	virtual void move_child     (ContainerPtr& child, long offset, long size);

	virtual int           get_fd (void);
	virtual long          get_block_size (void);
	virtual std::string   get_device_name (void);
	virtual long          get_parent_offset (void);
	virtual unsigned int  get_device_space (std::map<long, long>& spaces);

	virtual long get_size_total (void);
	virtual long get_size_used (void);
	virtual long get_size_free (void);

	virtual ContainerPtr find_device (const std::string& dev);
	virtual ContainerPtr find_name   (const std::string& name);
	virtual ContainerPtr find_uuid   (const std::string& uuid);
	virtual ContainerPtr find        (const std::string& uuid);

	virtual void find_type (const std::string& type, std::vector<ContainerPtr>& results);
	//XXX virtual std::vector<ContainerPtr> find_incomplete (void);

	virtual unsigned char* get_buffer (long offset, long size);
	virtual void close_buffer (unsigned char* buffer, long size);

	virtual bool is_a (const std::string& type);

	virtual std::string get_property (const std::string& propname);

	virtual std::vector<ContainerPtr>& get_children (void);

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
	void declare_prop (const char* owner, const char* name, T& var, const char* desc)
	{
		PPtr pp (new Property<T> (owner, name, var, desc));
		props[name] = pp;
	}

	void sub_type (const char* name);

public:
	//properties
	std::string	name;
	std::string	uuid;

	std::string	device;	// These don't belong here, but the alternative is multiple inheritance
	dev_t		device_major;
	dev_t		device_minor;

	long		parent_offset = 0;

	long		block_size = 0;
	long		bytes_size = 0;
	long		bytes_used = 0;

	ContainerPtr	 whole;

	std::weak_ptr<Container> parent;

	std::vector<std::string> type;	//XXX move to protected

	bool		 missing = false;

	int		 fd = -1;

	int seqnum = 123;

protected:
	Container (void);

	std::weak_ptr<Container> weak;	//XXX private?

	bool visit_children (Visitor& v);

	friend std::ostream& operator<< (std::ostream& stream, const ContainerPtr& c);

	void insert (long offset, long size, void* ptr);

	MmapSet	mmaps;

	std::map<std::string,PPtr> props;
	std::vector<ContainerPtr> children;

private:

};

#endif // _CONTAINER_H_

