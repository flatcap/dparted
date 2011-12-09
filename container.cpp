/* Copyright (c) 2011 Richard Russon (FlatCap)
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
#include <string>
#include <typeinfo>
#include <sstream>

#include "container.h"
#include "utils.h"

/**
 * Container
 */
Container::Container (void) :
	device_offset (0),
	block_size (0),
	bytes_size (0),
	bytes_used (0),
	parent (NULL)
{
}

/**
 * ~Container
 */
Container::~Container()
{
	for (std::vector<Container*>::iterator i = children.begin(); i != children.end(); i++) {
		delete *i;
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
		o = get_size ((*i)->device_offset);
		d = (*i)->get_device_name();
		t = (*i)->type;
		if (t.empty()) {
			t = "\e[37m\e[0m";
		}
#if 0
		if (d == "UNKNOWN") {
			continue;
		}
#endif
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
		printf ("%-10s %s%-20s%s  %-22s %13lld %13lld  %8s %8s %8s\n",
			d.c_str(),
			indent,
			t.c_str(),
			undent,
			(*i)->name.c_str(),
			(*i)->device_offset,
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

#if 0
/**
 * dump_dot
 */
std::string Container::dump_dot (void)
{
	std::ostringstream output;

	output << "|container|{{";
	output << "name\\l";
	output << "device\\l";
	output << "}|{";
	output << name << "\\l";
	output << device << "\\l";
	output << "}}";

	return output.str();
}

#else
/**
 * dump_dot
 */
std::string Container::dump_dot (void)
{
	std::ostringstream output;
	unsigned int count = children.size();

	//output << "<tr><td align=\"left\" bgcolor=\"#88cccc\" colspan=\"3\"><font color=\"#000000\"><b>Container</b></font></td></tr>\n";

	//output << dump_row ("type",          type);
	output << dump_row ("name",          name);
	output << dump_row ("label",         label);
	output << dump_row ("uuid",          uuid);
	output << dump_row ("device",        device);
	output << dump_row ("device_offset", device_offset);
	output << dump_row ("block_size",    block_size);
	output << dump_row ("bytes_size",    bytes_size);
	output << dump_row ("bytes_used",    bytes_used);
	output << dump_row ("parent",        parent);
	output << dump_row ("children",      count);	// vector

	return output.str();
}

#endif
/**
 * dump_row
 */
std::string Container::dump_row (const char *name, long long value)
{
	std::ostringstream s;

	s << "<tr>\n";
	s << "<td align=\"left\">" << name << "</td>\n";
	s << "<td>=</td>\n";
	s << "<td align=\"left\">" << value << "</td>\n";
	s << "</tr>\n";

	return s.str();
}

/**
 * dump_row
 */
std::string Container::dump_row (const char *name, long value)
{
	std::ostringstream s;

	s << "<tr>\n";
	s << "<td align=\"left\">" << name << "</td>\n";
	s << "<td>=</td>\n";
	s << "<td align=\"left\">" << value << "</td>\n";
	s << "</tr>\n";

	return s.str();
}

/**
 * dump_row
 */
std::string Container::dump_row (const char *name, int value)
{
	std::ostringstream s;

	s << "<tr>\n";
	s << "<td align=\"left\">" << name << "</td>\n";
	s << "<td>=</td>\n";
	s << "<td align=\"left\">" << value << "</td>\n";
	s << "</tr>\n";

	return s.str();
}

/**
 * dump_row
 */
std::string Container::dump_row (const char *name, unsigned int value)
{
	std::ostringstream s;

	s << "<tr>\n";
	s << "<td align=\"left\">" << name << "</td>\n";
	s << "<td>=</td>\n";
	s << "<td align=\"left\">" << value << "</td>\n";
	s << "</tr>\n";

	return s.str();
}

/**
 * dump_row
 */
std::string Container::dump_row (const char *name, bool value)
{
	std::ostringstream s;

	s << "<tr>\n";
	s << "<td align=\"left\">" << name << "</td>\n";
	s << "<td>=</td>\n";
	s << "<td align=\"left\">" << (value ? "true" : "false") << "</td>\n";
	s << "</tr>\n";

	return s.str();
}

/**
 * dump_row
 */
std::string Container::dump_row (const char *name, std::string &value)
{
	std::ostringstream s;

	s << "<tr>\n";
	s << "<td align=\"left\">" << name << "</td>\n";
	s << "<td>=</td>\n";
	s << "<td align=\"left\">" << value << "</td>\n";
	s << "</tr>\n";

	return s.str();
}

/**
 * dump_row
 */
std::string Container::dump_row (const char *name, void *value)
{
	std::ostringstream s;

	s << "<tr>\n";
	s << "<td align=\"left\">" << name << "</td>\n";
	s << "<td>=</td>\n";
	s << "<td align=\"left\">" << value << "</td>\n";
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
		if ((*i)->device_offset > child->device_offset) {
			children.insert (i, child);
			inserted = true;
			break;
		}
	}

	if (!inserted) {
		children.push_back (child);
	}

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
	//printf ("i am %s\n", typeid(*this).name());
	if (device.length() > 0)
		return device;
	else if (parent)
		return parent->get_device_name();
	else
		return "UNKNOWN";
}

/**
 * get_device_offset
 */
long long Container::get_device_offset (void)
{
	return device_offset;
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
	//printf ("Me? %s %s\n", device.c_str(), dev.c_str());
	if (dev == device)
		return this;

	for (std::vector<Container*>::iterator i = children.begin(); i != children.end(); i++) {
		//printf ("child %p (%s)\n", (*i), (*i)->device.c_str());
		match = (*i)->find_device (dev);
		if (match) {
			//printf ("MATCHES! %s (%s)\n", match->type.c_str(), match->name.c_str());
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
	return NULL;
}

