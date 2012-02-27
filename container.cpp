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


#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <string>
#include <typeinfo>
#include <sstream>

#include "container.h"
#include "utils.h"
#include "log.h"

/**
 * Container
 */
Container::Container (void) :
	cont_type (CONT_NORMAL),
	parent_offset (0),
	block_size (0),
	bytes_size (0),
	bytes_used (0),
	parent (NULL),
	fd (NULL)
{
	type = "container";
}

/**
 * ~Container
 */
Container::~Container()
{
	for (std::vector<Container*>::iterator i = children.begin(); i != children.end(); i++) {
		delete *i;
	}

	if (fd) {
		fclose (fd);
	}
}


/**
 * dump
 */
void Container::dump (int indent /* = 0 */)
{
	for (std::vector<Container*>::iterator i = children.begin(); i != children.end(); i++) {
		(*i)->dump (indent + 8);
	}
}

/**
 * dump2
 */
void Container::dump2 (void)
{
	std::string d;
	std::string f;
	std::string o;
	std::string s;
	std::string t;
	std::string u;

	for (std::vector<Container*>::iterator i = children.begin(); i != children.end(); i++) {
		s = get_size ((*i)->bytes_size);
		u = get_size ((*i)->bytes_used);
		f = get_size ((*i)->bytes_size - (*i)->bytes_used);
		o = get_size ((*i)->parent_offset);
		d = (*i)->get_device_name();
		t = (*i)->type;
		const char *indent = "";
		const char *undent = "                        ";
		if (parent == NULL) {
			indent = "";
			undent = "                        ";
		} else if (parent->parent == NULL) {
			indent = "    ";
			undent = "                    ";
		} else if (parent->parent->parent == NULL) {
			indent = "        ";
			undent = "                ";
		} else if (parent->parent->parent->parent == NULL) {
			indent = "            ";
			undent = "            ";
		} else if (parent->parent->parent->parent->parent == NULL) {
			indent = "                ";
			undent = "        ";
		} else if (parent->parent->parent->parent->parent->parent == NULL) {
			indent = "                    ";
			undent = "    ";
		} else if (parent->parent->parent->parent->parent->parent->parent == NULL) {
			indent = "                        ";
			undent = "";
		}
		log_debug ("%-10s %s%-20s%s  %-22s %13lld %13lld  %8s %8s %8s\n",
			d.c_str(),
			indent,
			t.c_str(),
			undent,
			(*i)->name.c_str(),
			(*i)->parent_offset,
			(*i)->bytes_size,
			s.c_str(),
			u.c_str(),
			f.c_str());
		(*i)->dump2();
	}
}

/**
 * dump_csv
 */
void Container::dump_csv (void)
{
	for (std::vector<Container*>::iterator i = children.begin(); i != children.end(); i++) {
		(*i)->dump_csv();
	}
}

/**
 * dump_dot
 */
std::string Container::dump_dot (void)
{
	std::ostringstream output;
	std::string uuid_short = uuid;
	//unsigned int count = children.size();

	if (!uuid_short.empty()) {
		uuid_short = uuid.substr (0, 6) + "...";
	}

	//output << "<tr><td align=\"left\" bgcolor=\"#88cccc\" colspan=\"3\"><font color=\"#000000\"><b>Container</b></font></td></tr>\n";

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
		for (std::vector<Container*>::iterator i = children.begin(); i != children.end(); i++) {
			output << dump_row ("", (*i));
		}
	}
#endif

	return output.str();
}

/**
 * dump_dot_children
 */
std::string Container::dump_dot_children (void)
{
	std::ostringstream s;

	// now iterate through all the children
	for (std::vector<Container*>::iterator i = children.begin(); i != children.end(); i++) {
		s << "\n";
		s << (*i)->dump_dot();
		s << "obj_" << this << " -> obj_" << (*i) << ";\n";
		s << "\n";
	}

	return s.str();
}


/**
 * dump_row
 */
std::string Container::dump_row (const char *name, long long value)
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
std::string Container::dump_row (const char *name, long value)
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
std::string Container::dump_row (const char *name, int value)
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
std::string Container::dump_row (const char *name, unsigned int value)
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
std::string Container::dump_row (const char *name, bool value)
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
std::string Container::dump_row (const char *name, std::string &value)
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
std::string Container::dump_row (const char *name, void *value)
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
std::string Container::dump_table_header (const char *name, const char *colour)
{
	std::ostringstream output;

	output << "obj_" << this <<" [label=<<table cellspacing=\"0\" border=\"0\">\n";
	output << "<tr><td align=\"left\" bgcolor=\"" << colour << "\" colspan=\"3\"><font color=\"#000000\"><b>" << name << "</b></font> (" << this << ")</td></tr>\n";

	return output.str();
}

/**
 * dump_table_footer
 */
std::string Container::dump_table_footer (void)
{
	std::ostringstream output;

	output << "</table>>];\n";

	return output.str();
}


/**
 * add_child
 */
void Container::add_child (Container *child)
{
	/* Check:
	 *	available space
	 *	alignment type
	 *	size (restrictions)
	 *	valid type within this parent
	 */
#if 0
	if (children.size() > 0) {
		Container *last = children.back();

		last->next = child;
		child->prev = last;
	}
#endif
	bytes_used += child->bytes_size;

	bool inserted = false;

	for (std::vector<Container*>::iterator i = children.begin(); i != children.end(); i++) {
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
void Container::delete_child (Container *child)
{
}

/**
 * move_child
 */
void Container::move_child (Container *child, long long offset, long long size)
{
}


/**
 * get_block_size
 */
long Container::get_block_size (void)
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
std::string Container::get_device_name (void)
{
	//log_debug ("i am %s\n", typeid(*this).name());
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
long long Container::get_parent_offset (void)
{
	return parent_offset;
}

/**
 * get_device_space
 */
unsigned int Container::get_device_space (std::map<long long, long long> &spaces)
{
	spaces.clear();

	//spaces[offset] = size
	//spaces[0] = 123;

	return spaces.size();;
}


/**
 * get_size_total
 */
long long Container::get_size_total (void)
{
	return bytes_size;
}

/**
 * get_size_used
 */
long long Container::get_size_used (void)
{
	return bytes_used;
}

/**
 * get_size_free
 */
long long Container::get_size_free (void)
{
	return bytes_size - bytes_used;
}


/**
 * find_device
 */
Container * Container::find_device (const std::string &dev)
{
	Container *match = NULL;

	// am *I* the device?
	//log_debug ("Me? %s %s\n", device.c_str(), dev.c_str());
	if (dev == device)
		return this;

	for (std::vector<Container*>::iterator i = children.begin(); i != children.end(); i++) {
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
Container * Container::find_uuid (const std::string &uuid)
{
	std::vector<Container*>::iterator i;

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
Container * Container::find_name (const std::string &name)
{
	std::vector<Container*>::iterator i;

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
FILE * Container::open_device (void)
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
int Container::read_data (long long offset, long long size, unsigned char *buffer)
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


