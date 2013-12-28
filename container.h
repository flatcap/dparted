/* Copyright (c) 2013 Richard Russon (FlatCap)
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Library General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _CONTAINER_H_
#define _CONTAINER_H_

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <set>
#include <tuple>

#include "pointers.h"
#include "variant.h"
#include "mmap.h"

class Visitor;

typedef std::shared_ptr<BaseVariant>          VPtr;

/**
 * class Container
 */
class Container
{
public:
	virtual ~Container();
	static ContainerPtr create (void);
	virtual bool accept (Visitor& v);

	virtual void mouse_event (void)	//RAR
	{
		std::cout << __PRETTY_FUNCTION__ << std::endl;
	}

	virtual void add_child      (ContainerPtr& child);
	virtual void just_add_child (ContainerPtr& child);
	virtual void delete_child   (ContainerPtr& child);
	virtual void move_child     (ContainerPtr& child, long offset, long size);

	template<class T>
	void add_child (std::shared_ptr<T>& child)
	{
		ContainerPtr c (child);
		add_child (c);
	}

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

	std::string	 name;
	std::string	 uuid;

	std::string	 device;
	long		 parent_offset = 0;

	long		 block_size = 0;
	long		 bytes_size = 0;
	long		 bytes_used = 0;

	WholePtr	 whole;

	std::weak_ptr<Container> parent;

	std::vector<std::string> type;	//XXX move to protected

	bool		 missing = false;

	int		 fd = -1;

	virtual std::vector<ContainerPtr>& get_children (void);

	std::string get_path (void);

	std::weak_ptr<Container> weak;	//XXX private?

	ContainerPtr get_smart (void)
	{
		if (weak.expired()) {
			std::cout << "SMART\n";
			//XXX who created us?
			ContainerPtr c (this);
			weak = c;
		}
		return weak.lock();
	}

	template<typename T>
	void declare_prop (const char* owner, const char* name, T& var, const char* desc)
	{
		VPtr vp (new Variant<T>(owner, name, var, desc));
		props[name] = vp;
	}

	VPtr get_prop (const std::string& name);

	std::vector<std::string> get_prop_names (void);

protected:
	Container (void);

	bool visit_children (Visitor& v);

	friend std::ostream& operator<< (std::ostream& stream, const ContainerPtr& c);

	void declare (const char* name);
	void insert (long offset, long size, void* ptr);

	MmapSet	mmaps;

	std::map<std::string,VPtr> props;
	std::vector<ContainerPtr> children;
private:

};


#endif // _CONTAINER_H_

