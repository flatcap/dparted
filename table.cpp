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

#include <sstream>
#include <string>

#include "table.h"
#include "action.h"
#ifdef DP_GPT
#include "gpt.h"
#endif
#include "log.h"
#include "log_trace.h"
#ifdef DP_LUKS
#include "luks_table.h"
#endif
#ifdef DP_LVM
#include "lvm_table.h"
#endif
#ifdef DP_MD
#include "md_table.h"
#endif
#ifdef DP_MSDOS
#include "msdos.h"
#endif
#include "partition.h"
#include "utils.h"
#include "visitor.h"

Table::Table (void)
{
	const char* me = "Table";

	sub_type (me);
}

Table::~Table()
{
}

TablePtr
Table::create (void)
{
	TablePtr p (new Table());
	p->self = p;

	return p;
}


bool
Table::accept (Visitor& v)
{
	TablePtr t = std::dynamic_pointer_cast<Table> (get_smart());
	if (!v.visit(t))
		return false;

	return visit_children(v);
}


std::vector<Action>
Table::get_actions (void)
{
	// LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.table", true },
	};

	std::vector<Action> parent_actions = Container::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

bool
Table::perform_action (Action action)
{
	if (action.name == "dummy.table") {
		log_debug ("Table perform: %s\n", action.name.c_str());
		return true;
	} else {
		return Container::perform_action (action);
	}
}


bool
Table::probe (ContainerPtr& parent, std::uint8_t* buffer, std::uint64_t bufsize)
{
	//LOG_TRACE;

	return_val_if_fail (parent,  false);
	return_val_if_fail (buffer,  false);
	return_val_if_fail (bufsize, false);

#ifdef DP_GPT
	if (Gpt::probe (parent, buffer, bufsize))
		return true;
#endif
#ifdef DP_LUKS
	if (LuksTable::probe (parent, buffer, bufsize))
		return true;
#endif
#ifdef DP_LVM
	if (LvmTable::probe (parent, buffer, bufsize))
		return true;
#endif
#ifdef DP_MD
	if (MdTable::probe (parent, buffer, bufsize))
		return true;
#endif
#ifdef DP_MSDOS
	if (Msdos::probe (parent, buffer, bufsize))
		return true;
#endif

	return false;
}

std::uint64_t
Table::fill_space (void)
{
	// iterate through the children and add a Misc for all the unallocated regions
	// the size of these fillers must be >= alignment size

	std::vector<ContainerPtr> vm;
#if 0
	std::string s1;
	std::string s2;
	log_debug ("fill space\n");
#endif

	std::uint64_t upto = 0;

	for (auto c : children) {
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
			PartitionPtr p = Partition::create();
			p->sub_type ("Space");
			p->sub_type ("Unallocated");
			p->parent_offset = upto;
			p->bytes_size = (c->parent_offset - upto);
			p->bytes_used = 0;

			vm.push_back(p);

			upto = c->parent_offset + c->bytes_size;
		}
	}

	//log_debug ("upto = %ld, size = %ld\n", upto, bytes_size);
	if (upto < bytes_size) {
		PartitionPtr p = Partition::create();
		p->sub_type ("Space");
		p->sub_type ("Unallocated");
		p->parent_offset = upto;
		p->bytes_size = (bytes_size - upto);
		p->bytes_used = 0;
		vm.push_back(p);
	}

#if 1
	for (auto i : vm) {
		add_child(i);			// add_free()
	}
#endif

#if 0
	log_debug ("\nrecap\n");
	for (auto c : children) {
		std::string s1 = get_size (c->bytes_size);
		log_debug ("\t%-12s %12lld -> %12lld  %9s\n", c->name.c_str(), c->parent_offset, c->parent_offset + c->bytes_size, s1.c_str());
	}
#endif

	return 0;
}


bool
Table::set_alignment (std::uint64_t bytes)
{
	block_size = bytes;
	return true;
}

