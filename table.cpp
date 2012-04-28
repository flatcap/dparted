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


#include <sstream>
#include <string>

#include "gpt.h"
#include "identify.h"
#include "log.h"
#include "misc.h"
#include "msdos.h"
#include "table.h"
#include "lvm_table.h"

/**
 * Table
 */
Table::Table (void)
{
	declare ("table", "#ffbbbb");
}

/**
 * ~Table
 */
Table::~Table()
{
}


/**
 * probe
 */
DPContainer * Table::probe (DPContainer *parent, unsigned char *buffer, int bufsize)
{
	DPContainer *c = NULL;

	if ((c = Gpt::probe (parent, buffer, bufsize)))
		return c;

	if ((c = Msdos::probe (parent, buffer, bufsize)))
		return c;

	if ((c = LVMTable::probe (parent, buffer, bufsize)))
		return c;

	return NULL;
}

/**
 * fill_space
 */
long long Table::fill_space (void)
{
	// iterate through the children and add a Misc for all the unallocated regions
	// the size of these fillers must be >= alignment size

	std::vector<DPContainer*> vm;
	std::vector<DPContainer*>::iterator i;
#if 0
	std::string s1;
	std::string s2;
	log_debug ("fill space\n");
#endif

	long long upto = 0;

	for (i = children.begin(); i != children.end(); i++) {
		DPContainer *c = (*i);

		if (upto == c->parent_offset) {
			upto += c->bytes_size;
#if 0
			s1 = get_size (c->parent_offset);
			s2 = get_size (c->parent_offset + c->bytes_size);
			log_debug ("\tpartition %12lld -> %12lld    %8s -> %8s\n", c->parent_offset, c->parent_offset + c->bytes_size, s1.c_str(), s2.c_str());
#endif
		} else {
#if 0
			s1 = get_size (upto);
			s2 = get_size (c->parent_offset);
			log_debug ("\tspace     %12lld -> %12lld    %8s -> %8s\n", upto, c->parent_offset, s1.c_str(), s2.c_str());
			s1 = get_size (c->parent_offset);
			s2 = get_size (c->parent_offset + c->bytes_size);
			log_debug ("\tpartition %12lld -> %12lld    %8s -> %8s\n", c->parent_offset, c->parent_offset + c->bytes_size, s1.c_str(), s2.c_str());
#endif
			Misc *m = new Misc();
			m->name = "unallocated";
			m->parent_offset = upto;
			m->bytes_size = (c->parent_offset - upto);
			m->bytes_used = 0;

			vm.push_back (m);

			upto = c->parent_offset + c->bytes_size;
		}
	}

	//log_debug ("upto = %lld, size = %lld\n", upto, bytes_size);
	if (upto < bytes_size) {
		Misc *m = new Misc();
		m->name = "unallocated";
		m->parent_offset = upto;
		m->bytes_size = (bytes_size - upto);
		m->bytes_used = 0;
		vm.push_back (m);
	}

#if 1
	for (i = vm.begin(); i != vm.end(); i++) {
		add_child (*i);			// add_free()
	}
#endif

#if 0
	log_debug ("\nrecap\n");
	for (i = children.begin(); i != children.end(); i++) {
		DPContainer *c = (*i);
		s1 = get_size (c->bytes_size);
		log_debug ("\t%-12s %12lld -> %12lld  %9s\n", c->name.c_str(), c->parent_offset, c->parent_offset + c->bytes_size, s1.c_str());
	}
#endif

	return 0;
}



/**
 * dump_dot
 */
std::string Table::dump_dot (void)
{
	std::ostringstream output;

	output << DPContainer::dump_dot();

	// no specifics for now

	return output.str();
}


