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


#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <typeinfo>

#include "container.h"
#include "log.h"
#include "utils.h"

/**
 * DPContainer
 */
DPContainer::DPContainer (void) :
	parent_offset (0),
	block_size (0),
	bytes_size (0),
	bytes_used (0),
	whole (NULL),
	parent (NULL),
	fd (NULL)
{
	type.push_back ("container");
}

/**
 * ~DPContainer
 */
DPContainer::~DPContainer()
{
	for (std::vector<DPContainer*>::iterator i = children.begin(); i != children.end(); i++) {
		delete *i;
	}

	if (fd) {
		fclose (fd);
	}
}


/**
 * dump_dot
 */
std::string DPContainer::dump_dot (void)
{
	std::ostringstream output;
	std::string uuid_short = uuid;
	//unsigned int count = children.size();

	if (!uuid_short.empty()) {
		uuid_short = uuid.substr (0, 6) + "...";
	}

#if 0
	log_debug ("container: %lu types\n", type.size());
	for (unsigned int i = 0; i < type.size(); i++) {
		log_debug ("\t%s\n", type[i].c_str());
	}
#endif

	//output << "<tr><td align=\"left\" bgcolor=\"#88cccc\" colspan=\"3\"><font color=\"#000000\"><b>DPContainer</b></font></td></tr>\n";

	//output << dump_row ("type",          type);
	output << dump_row ("name",          name);
	output << dump_row ("uuid",          uuid_short); //RAR temp
	output << dump_row ("device",        device);
	output << dump_row ("file desc",     fd);
	output << dump_row ("parent_offset", parent_offset);
	//output << dump_row ("block_size",    block_size);
	output << dump_row ("bytes_size",    bytes_size);
	output << dump_row ("bytes_used",    bytes_used);
	output << dump_row ("bytes_free",    bytes_size - bytes_used);
	//output << dump_row ("parent",        parent);

#if 0
	if (count > 0) {
		output << dump_row ("children",      count);
		for (std::vector<DPContainer*>::iterator i = children.begin(); i != children.end(); i++) {
			output << dump_row ("", (*i));
		}
	}
#endif

	return output.str();
}

/**
 * dump_dot_children
 */
std::string DPContainer::dump_dot_children (void)
{
	std::ostringstream s;

	// now iterate through all the children
	for (std::vector<DPContainer*>::iterator i = children.begin(); i != children.end(); i++) {
		//std::cout << (*i) << "\n";
		s << "\n";
		s << (*i)->dump_dot();
		s << "obj_" << (void*) this << " -> obj_" << (void*) (*i) << ";\n";
		s << "\n";
	}

	return s.str();
}


/**
 * dump_row
 */
std::string DPContainer::dump_row (const char *name, long long value)
{
	std::ostringstream s;
	std::string str = get_size (value);

	s << "<tr>";
	s << "<td align=\"left\">" << name << "</td>";
	s << "<td>=</td>";
#if 1
	s << "<td align=\"left\">" << str << "</td>";
#else
	s << "<td align=\"left\">" << value << " (" << str << ")</td>";
#endif
	s << "</tr>\n";

	return s.str();
}

/**
 * dump_row
 */
std::string DPContainer::dump_row (const char *name, long value)
{
	std::ostringstream s;

	s << "<tr>";
	s << "<td align=\"left\">" << name << "</td>";
	s << "<td>=</td>";
	s << "<td align=\"left\">" << value << "</td>";
	s << "</tr>\n";

	return s.str();
}

/**
 * dump_row
 */
std::string DPContainer::dump_row (const char *name, int value)
{
	std::ostringstream s;

	s << "<tr>";
	s << "<td align=\"left\">" << name << "</td>";
	s << "<td>=</td>";
	s << "<td align=\"left\">" << value << "</td>";
	s << "</tr>\n";

	return s.str();
}

/**
 * dump_row
 */
std::string DPContainer::dump_row (const char *name, unsigned int value)
{
	std::ostringstream s;

	s << "<tr>";
	s << "<td align=\"left\">" << name << "</td>";
	s << "<td>=</td>";
	s << "<td align=\"left\">" << value << "</td>";
	s << "</tr>\n";

	return s.str();
}

/**
 * dump_row
 */
std::string DPContainer::dump_row (const char *name, bool value)
{
	std::ostringstream s;

	s << "<tr>";
	s << "<td align=\"left\">" << name << "</td>";
	s << "<td>=</td>";
	s << "<td align=\"left\">" << (value ? "true" : "false") << "</td>";
	s << "</tr>\n";

	return s.str();
}

/**
 * dump_row
 */
std::string DPContainer::dump_row (const char *name, std::string &value)
{
	std::ostringstream s;

	s << "<tr>";
	s << "<td align=\"left\">" << name << "</td>";
	s << "<td>=</td>";
	s << "<td align=\"left\">" << value << "</td>";
	s << "</tr>\n";

	return s.str();
}

/**
 * dump_row
 */
std::string DPContainer::dump_row (const char *name, void *value)
{
	std::ostringstream s;

	s << "<tr>";
	s << "<td align=\"left\">" << name << "</td>";
	s << "<td>=</td>";
	s << "<td align=\"left\">" << value << "</td>";
	s << "</tr>\n";

	return s.str();
}


/**
 * dump_table_header
 */
std::string DPContainer::dump_table_header (const char *name, const char *colour)
{
	std::ostringstream output;

	output << "obj_" << (void*) this <<" [label=<<table cellspacing=\"0\" border=\"0\">\n";
	output << "<tr><td align=\"left\" bgcolor=\"" << colour << "\" colspan=\"3\"><font color=\"#000000\"><b>" << name << "</b></font> (" << (void*) this << ")</td></tr>\n";

	return output.str();
}

/**
 * dump_table_footer
 */
std::string DPContainer::dump_table_footer (void)
{
	std::ostringstream output;

	output << "</table>>];\n";

	return output.str();
}


/**
 * add_child
 */
void DPContainer::add_child (DPContainer *child)
{
	/* Check:
	 *	available space
	 *	alignment type
	 *	size (restrictions)
	 *	valid type within this parent
	 */
#if 0
	if (children.size() > 0) {
		DPContainer *last = children.back();

		last->next = child;
		child->prev = last;
	}
#endif
	bytes_used += child->bytes_size;

	bool inserted = false;

	for (std::vector<DPContainer*>::iterator i = children.begin(); i != children.end(); i++) {
		if ((*i)->parent_offset > child->parent_offset) {
			children.insert (i, child);
			inserted = true;
			break;
		}
	}

	if (!inserted) {
		children.push_back (child);
	}

	//log_debug ("insert: %s (%s)\n", this->name.c_str(), child->name.c_str());

	child->parent = this;
}

/**
 * delete_child
 */
void DPContainer::delete_child (DPContainer *child)
{
}

/**
 * move_child
 */
void DPContainer::move_child (DPContainer *child, long long offset, long long size)
{
}


/**
 * get_block_size
 */
long DPContainer::get_block_size (void)
{
	if (block_size > 0)
		return block_size;

	if (parent)
		return parent->get_block_size();

	return -1;
}

/**
 * get_device_name
 */
std::string DPContainer::get_device_name (void)
{
	//log_debug ("i am %s\n", typeid (*this).name());
	if (device.length() > 0)
		return device;
	else if (parent)
		return parent->get_device_name();
	else
		return "UNKNOWN";
}

/**
 * get_parent_offset
 */
long long DPContainer::get_parent_offset (void)
{
	return parent_offset;
}

/**
 * get_device_space
 */
unsigned int DPContainer::get_device_space (std::map<long long, long long> &spaces)
{
	spaces.clear();

	//spaces[offset] = size
	//spaces[0] = 123;

	return spaces.size();;
}


/**
 * get_size_total
 */
long long DPContainer::get_size_total (void)
{
	return bytes_size;
}

/**
 * get_size_used
 */
long long DPContainer::get_size_used (void)
{
	return bytes_used;
}

/**
 * get_size_free
 */
long long DPContainer::get_size_free (void)
{
	return bytes_size - bytes_used;
}


/**
 * find_device
 */
DPContainer * DPContainer::find_device (const std::string &dev)
{
	DPContainer *match = NULL;

	// am *I* the device?
	//log_debug ("Me? %s %s\n", device.c_str(), dev.c_str());
	if (dev == device)
		return this;

	for (std::vector<DPContainer*>::iterator i = children.begin(); i != children.end(); i++) {
		//log_debug ("child %p (%s)\n", (*i), (*i)->device.c_str());
		match = (*i)->find_device (dev);
		if (match) {
			//log_debug ("MATCHES! %s (%s)\n", match->type.c_str(), match->name.c_str());
			break;
		}
	}

	return match;
}

/**
 * find_uuid
 */
DPContainer * DPContainer::find_uuid (const std::string &uuid)
{
	std::vector<DPContainer*>::iterator i;

	for (i = children.begin(); i != children.end(); i++) {
		if ((*i)->uuid == uuid) {
			return (*i);
		}
	}

	return NULL;
}

/**
 * find_name
 */
DPContainer * DPContainer::find_name (const std::string &name)
{
	std::vector<DPContainer*>::iterator i;

	for (i = children.begin(); i != children.end(); i++) {
		if ((*i)->name == name) {
			return (*i);
		}
	}

	return NULL;
}


/**
 * open_device
 */
FILE * DPContainer::open_device (void)
{
	// flags? ro, rw
	if (fd)
		return fd;

	if (device.empty()) {
		if (parent) {
			fd = parent->open_device();
		}
		return fd;
	}

	fd = fopen (device.c_str(), "re");	// read, close on exec
	if (fd == NULL) {
		log_error ("failed to open device %s\n", device.c_str());
	}

	//log_info ("OPEN %s = %p\n", device.c_str(), fd);
	return fd;
}

/**
 * read_data
 */
int DPContainer::read_data (long long offset, long long size, unsigned char *buffer)
{
	// if offset >= 0, seek there
	long long current = -1;
	long long bytes = 0;

	if (!buffer)
		return -1;

	if (fd == NULL) {
		if (parent) {
			return parent->read_data (offset + parent_offset, size, buffer);
		} else {
			return -1;
		}
	}

	if (offset >= 0) {
		current = fseek (fd, offset, SEEK_SET);
	} else {
		current = fseek (fd, 0, SEEK_CUR);
	}

	if (current < 0) {
		log_error ("seek to %lld failed on %s (%p)\n", offset, device.c_str(), fd);
		perror ("seek");
		return 1;
	}

	if ((current + size) > bytes_size)
		return -1;

	bytes = fread (buffer, size, 1, fd);
	std::string s = get_size (current);
	//log_info ("READ: device %s (%p), offset %lld (%s), size %lld = %lld\n", device.c_str(), fd, current, s.c_str(), size, bytes);

	return bytes;
}


/**
 * operator<<
 */
std::ostream& operator<< (std::ostream &stream, const DPContainer &c)
{
	stream << c.name << ", " << c.type.back() << ", " << c.device << ", " << c.children.size();
	return stream;
}

/**
 * operator<<
 */
std::ostream& operator<< (std::ostream &stream, const DPContainer *c)
{
	if (c)
		return operator<< (stream, *c);
	else
		return stream;
}


/**
 * is_a
 */
bool DPContainer::is_a (const std::string &t)
{
	//std::cout << "my type = " << type.back() << ", compare to " << t << "\n";

	// Start with the most derived type
	std::vector<std::string>::reverse_iterator it;

	for (it = type.rbegin(); it != type.rend(); it++) {
		if ((*it) == t) {
			return true;
		}
	}

	return false;
}

