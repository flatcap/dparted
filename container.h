/* Copyright (c) 2012 Richard Russon (FlatCap)
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

class Whole;

class DPContainer
{
public:
	DPContainer (void);
	virtual ~DPContainer();

	virtual std::string dump_dot (void);
	virtual std::string dump_dot_children (void);

	virtual void add_child    (DPContainer *child);
	virtual void delete_child (DPContainer *child);
	virtual void move_child   (DPContainer *child, long long offset, long long size);

	virtual long          get_block_size (void);
	virtual std::string   get_device_name (void);
	virtual long long     get_parent_offset (void);
	virtual unsigned int  get_device_space (std::map<long long, long long> &spaces);

	virtual long long get_size_total (void);
	virtual long long get_size_used (void);
	virtual long long get_size_free (void);

	virtual DPContainer * find_device (const std::string &dev);
	virtual DPContainer * find_uuid   (const std::string &uuid);
	virtual DPContainer * find_name   (const std::string &name);

	virtual FILE * open_device (void);
	virtual int read_data (long long offset, long long size, unsigned char *buffer);

	virtual bool is_a (const std::string &type);

	std::string	 name;
	std::string	 uuid;

	std::string	 device;
	long long	 parent_offset;

	long		 block_size;
	long long	 bytes_size;
	long long	 bytes_used;

	Whole		*whole;

	DPContainer	*parent;
	std::vector<DPContainer*> children;

	friend std::ostream & operator<< (std::ostream &stream, const DPContainer &c);
	friend std::ostream & operator<< (std::ostream &stream, const DPContainer *c);

protected:
	std::string dump_row (const char *name, long long value);
	std::string dump_row (const char *name, long value);
	std::string dump_row (const char *name, int value);
	std::string dump_row (const char *name, unsigned int value);
	std::string dump_row (const char *name, bool value);
	std::string dump_row (const char *name, std::string &value);
	std::string dump_row (const char *name, void *value);

	std::string dump_table_header (const char *name, const char *colour);
	std::string dump_table_footer (void);

	FILE		*fd;

	std::vector<std::string> type;

private:

};

#endif // _CONTAINER_H_

