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
#include <functional>
#include <algorithm>

#include "table.h"
#include "action.h"
#ifdef DP_GPT
#include "gpt.h"
#endif
#include "log.h"
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
	LOG_CTOR;
	const char* me = "Table";

	sub_type (me);
}

Table::Table (const Table& c) :
	Container(c)
{
	Table();
	LOG_CTOR;
	// No properties
}

Table::Table (Table&& c)
{
	LOG_CTOR;
	swap(c);
}

Table::~Table()
{
	LOG_DTOR;
}

TablePtr
Table::create (void)
{
	TablePtr p (new Table());
	p->self = p;

	return p;
}


Table&
Table::operator= (const Table& UNUSED(c))
{
	// No properties

	return *this;
}

Table&
Table::operator= (Table&& c)
{
	swap(c);
	return *this;
}


void
Table::swap (Table& UNUSED(c))
{
	// No properties
}

void
swap (Table& lhs, Table& rhs)
{
	lhs.swap (rhs);
}


Table*
Table::clone (void)
{
	LOG_TRACE;
	return new Table (*this);
}


bool
Table::accept (Visitor& v)
{
	TablePtr t = std::dynamic_pointer_cast<Table> (get_smart());
	if (!v.visit(t))
		return false;

	return visit_children(v);
}


ContainerPtr
space_join (ContainerPtr first, ContainerPtr last = {})
{
	return_val_if_fail (first, {});

	ContainerPtr space = Partition::create();
	if (!space) {
		log_error ("failed to create space");
		return {};
	}

	space->sub_type ("Space");
	space->sub_type ("Unallocated");

	if (last) {
		space->bytes_size = (last->parent_offset - first->parent_offset) + last->bytes_size;
	} else {
		space->bytes_size = first->bytes_size;
	}

	space->parent_offset = first->parent_offset;
	space->bytes_used    = space->bytes_size;

	return space;
}

ContainerPtr
space_create (std::uint64_t parent_offset, std::uint64_t bytes_size)
{
	ContainerPtr space = Partition::create();
	if (!space) {
		log_error ("failed to create space");
		return {};
	}

	space->sub_type ("Space");
	space->sub_type ("Unallocated");
	space->parent_offset = parent_offset;
	space->bytes_size    = bytes_size;
	space->bytes_used    = bytes_size;

	return space;
}

void
Table::add_child (ContainerPtr child, bool probe)
{
	ContainerPtr space;
	log_info ("NEW add_child: %ld, %ld", child->parent_offset, child->parent_offset + child->bytes_size - 1);

	for (auto& c : children) {
		// log_info ("\tspace: %10ld - %10ld", c->parent_offset, c->parent_offset + c->bytes_size - 1);
		if ((child->parent_offset >= c->parent_offset) &&
		    (child->parent_offset + child->bytes_size) <= (c->parent_offset + c->bytes_size)) {
			// log_error ("it fits");
			space = c;
			break;
		}
	}

	if (!space) {
		Container::add_child (child, probe);
		return;
	}

	log_info ("space has %ld listeners", space->count_listeners());

	std::uint64_t c_begin = child->parent_offset;
	std::uint64_t c_end   = child->parent_offset + child->bytes_size;
	std::uint64_t s_begin = space->parent_offset;
	std::uint64_t s_end   = space->parent_offset + space->bytes_size;

	auto first = std::begin (children);
	auto end   = std::end   (children);
	auto it    = std::find  (first, end, space);

	ContainerPtr parent = get_smart();
	if (txn) {
		txn->notifications.push_back (std::make_tuple (NotifyType::t_delete, parent, space));
	}
	children.erase (it);

	if (c_begin == s_begin) {
		if (c_end == s_end) {
			log_error ("WHOLE");
			_add_child (children, child);
			if (txn) {
				txn->notifications.push_back (std::make_tuple (NotifyType::t_add, parent, child));
			}
		} else {
			log_error ("START");
			ContainerPtr s = space_create (c_end, space->bytes_size - child->bytes_size);
			_add_child (children, child);
			_add_child (children, s);
			if (txn) {
				txn->notifications.push_back (std::make_tuple (NotifyType::t_add, parent, child));
				txn->notifications.push_back (std::make_tuple (NotifyType::t_add, parent, s));
			}
		}
	} else {
		if (c_end == s_end) {
			log_error ("END");
			ContainerPtr s = space_create (s_begin, space->bytes_size - child->bytes_size);
			_add_child (children, s);
			_add_child (children, child);
			if (txn) {
				txn->notifications.push_back (std::make_tuple (NotifyType::t_add, parent, s));
				txn->notifications.push_back (std::make_tuple (NotifyType::t_add, parent, child));
			}
		} else {
			log_error ("MIDDLE");
			ContainerPtr s1 = space_create (s_begin, child->parent_offset - space->parent_offset);
			ContainerPtr s2 = space_create (c_end, space->bytes_size - (child->parent_offset - space->parent_offset));
			log_info (s1);
			log_info (s2);
			_add_child (children, s1);
			_add_child (children, child);
			_add_child (children, s2);
			if (txn) {
				txn->notifications.push_back (std::make_tuple (NotifyType::t_add, parent, s1));
				txn->notifications.push_back (std::make_tuple (NotifyType::t_add, parent, child));
				txn->notifications.push_back (std::make_tuple (NotifyType::t_add, parent, s2));
			}
		}
	}
}

void
Table::delete_child (ContainerPtr child)
{
	std::lock_guard<std::recursive_mutex> lock (mutex_children);

	auto first = std::begin (children);
	auto end   = std::end   (children);
	auto last  = std::prev  (end);

	log_info ("children:");
	for (auto& c : children) {
		log_info ("\t%s", c->name.c_str());
	}

	if (first != end) {
		log_info ("first = %s", (*first)->name.c_str());
	}

	if (last != end) {
		log_info ("last  = %s", (*last)->name.c_str());
	}

	log_info ("%ld children", children.size());
	auto it = std::find (first, end, child);

	if (it == end) {
		log_error ("delete: child doesn't exist");
		return;
	}

	bool space_before = false;
	bool space_after  = false;

	ContainerPtr prev;
	ContainerPtr next;

	if (it != first) {
		prev = *(std::prev (it));
		space_before = prev->is_a ("Unallocated");
	}

	if (it != last) {
		next = *(std::next (it));
		space_after = next->is_a ("Unallocated");
	}

	log_info ("space before = %s", space_before ? "true" : "false");
	log_info ("space after  = %s", space_after  ? "true" : "false");

	ContainerPtr s;
	ContainerPtr parent = get_smart();
	if (space_before) {
		if (space_after) {
			s = space_join (prev, next);	// join into one big space
			children.erase (std::prev(it), std::next(it));
			if (txn) {
				txn->notifications.push_back (std::make_tuple (NotifyType::t_delete, parent, prev));
				txn->notifications.push_back (std::make_tuple (NotifyType::t_delete, parent, child));
				txn->notifications.push_back (std::make_tuple (NotifyType::t_delete, parent, next));
			}
		} else {
			s = space_join (prev, child);	// extend prev
			children.erase (std::prev(it), it);
			if (txn) {
				txn->notifications.push_back (std::make_tuple (NotifyType::t_delete, parent, prev));
				txn->notifications.push_back (std::make_tuple (NotifyType::t_delete, parent, child));
			}
		}
	} else {
		if (space_after) {
			s = space_join (child, next);	// extend next backwards
			children.erase (it, std::next(it));
			if (txn) {
				txn->notifications.push_back (std::make_tuple (NotifyType::t_delete, parent, child));
				txn->notifications.push_back (std::make_tuple (NotifyType::t_delete, parent, next));
			}
		} else {
			s = space_join (child);		// replace child with space
			children.erase (it);
			if (txn) {
				txn->notifications.push_back (std::make_tuple (NotifyType::t_delete, parent, child));
			}
		}
	}

	add_child (s, false);
	if (txn) {
		txn->notifications.push_back (std::make_tuple (NotifyType::t_add, parent, s));
	}
}

void
Table::move_child (ContainerPtr child, std::uint64_t offset, std::uint64_t size)
{
	ContainerPtr space;
	log_info("");
	log_info ("move_child");
	log_info ("\told: start: %10ld  end: %10ld  size: %10ld", child->parent_offset, child->parent_offset+child->bytes_size, child->bytes_size);
	log_info ("\tnew: start: %10ld  end: %10ld  size: %10ld", offset, offset+size, size);

	bool backwards = (offset < child->parent_offset);
	bool forwards  = ((offset + size) > (child->parent_offset + child->bytes_size));

	log_info ("extend backwards = %s", backwards ? "true" : "false");
	log_info ("extend forwards  = %s", forwards  ? "true" : "false");

	auto first = std::begin (children);
	auto end   = std::end   (children);
	auto last  = std::prev  (end);
	auto it    = std::find  (first, end, child);

	auto it_start = it;
	auto it_end   = it;

	ContainerPtr space_before;
	ContainerPtr space_after;

	if (it != first) {
		auto it_prev = std::prev (it);
		ContainerPtr prev = *it_prev;
		if (prev->is_a ("Unallocated")) {
			space_before = prev;
			it_start = it_prev;
		}
	}

	if (it != last) {
		auto it_next = std::next (it);
		ContainerPtr next = *it_next;
		if (next->is_a ("Unallocated")) {
			space_after = next;
			it_end = it_next;
		}
	}

	log_info ("space before = %s", space_before ? "true" : "false");
	log_info ("space after  = %s", space_after  ? "true" : "false");

	if (backwards) {
		if (!space_before) {
			log_error ("No space to extend backwards");
			return;
		}

		if (offset < space_before->parent_offset) {
			log_error ("space before isn't big enough (%ld)", space_before->parent_offset - offset);
			return;
		}
	}

	if (forwards) {
		if (!space_after) {
			log_error ("No space to extend forwards");
			return;
		}

		if ((offset + size) > (space_after->parent_offset + space_after->bytes_size)) {
			log_error ("space after isn't big enough (%ld)", (offset + size) - (space_after->parent_offset + space_after->bytes_size));
			return;
		}
	}

	ContainerPtr parent = get_smart();
	if (txn) {
		if (backwards) {
			txn->notifications.push_back (std::make_tuple (NotifyType::t_delete, parent, space_before));
		}
		txn->notifications.push_back (std::make_tuple (NotifyType::t_delete, parent, child));
		if (forwards) {
			txn->notifications.push_back (std::make_tuple (NotifyType::t_delete, parent, space_after));
		}
	}
	children.erase (it_start, it_end);

	ContainerPtr new_child = child->backup();
	if (!new_child) {
		log_error ("child backup failed");
		return;
	}

	if (backwards) {
		if (offset == space_before->parent_offset) {
			log_error ("WHOLE before");
			new_child->parent_offset = offset;
			new_child->bytes_size    = size;
			_add_child (children, new_child);
			if (txn) {
				txn->notifications.push_back (std::make_tuple (NotifyType::t_add, parent, new_child));
			}
		} else {
			log_error ("PARTIAL before");
			new_child->parent_offset = offset;
			new_child->bytes_size    = size;
			_add_child (children, new_child);

			std::uint64_t start = space_before->parent_offset;
			std::uint64_t end   = new_child->parent_offset;
			ContainerPtr s = space_create (start, end - start);
			_add_child (children, s);

			if (txn) {
				txn->notifications.push_back (std::make_tuple (NotifyType::t_add, parent, new_child));
				txn->notifications.push_back (std::make_tuple (NotifyType::t_add, parent, s));
			}
		}
	}

	if (forwards) {
		if ((offset + size) == (space_after->parent_offset + space_after->bytes_size)) {
			log_error ("WHOLE after");
			new_child->parent_offset = offset;
			new_child->bytes_size    = size;
			_add_child (children, new_child);
			if (txn) {
				txn->notifications.push_back (std::make_tuple (NotifyType::t_add, parent, new_child));
			}
		} else {
			log_error ("PARTIAL after");
			new_child->parent_offset = offset;
			new_child->bytes_size    = size;
			_add_child (children, new_child);

			std::uint64_t start = new_child->parent_offset + new_child->bytes_size;
			std::uint64_t end   = space_after->parent_offset + space_after->bytes_size;
			ContainerPtr s = space_create (start, end - start);
			_add_child (children, s);

			if (txn) {
				txn->notifications.push_back (std::make_tuple (NotifyType::t_add, parent, new_child));
				txn->notifications.push_back (std::make_tuple (NotifyType::t_add, parent, s));
			}
		}
	}
}


std::vector<Action>
Table::get_actions (void)
{
	LOG_TRACE;
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
		log_debug ("Table perform: %s", action.name.c_str());
		return true;
	} else {
		return Container::perform_action (action);
	}
}


bool
Table::probe (ContainerPtr& parent, std::uint8_t* buffer, std::uint64_t bufsize)
{
	return_val_if_fail (parent,  false);
	return_val_if_fail (buffer,  false);
	return_val_if_fail (bufsize, false);
	LOG_TRACE;

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
	log_debug ("fill space");
#endif

	std::uint64_t upto = 0;

	for (auto& c : children) {
		if (upto == c->parent_offset) {
			upto += c->bytes_size;
#if 0
			std::string s1 = get_size (c->parent_offset);
			std::string s2 = get_size (c->parent_offset + c->bytes_size);
			log_debug ("\tpartition %12ld -> %12ld    %8s -> %8s", c->parent_offset, c->parent_offset + c->bytes_size, s1.c_str(), s2.c_str());
#endif
		} else {
#if 0
			std::string s1 = get_size (upto);
			std::string s2 = get_size (c->parent_offset);
			log_debug ("\tspace     %12ld -> %12ld    %8s -> %8s", upto, c->parent_offset, s1.c_str(), s2.c_str());
			s1 = get_size (c->parent_offset);
			s2 = get_size (c->parent_offset + c->bytes_size);
			log_debug ("\tpartition %12ld -> %12ld    %8s -> %8s", c->parent_offset, c->parent_offset + c->bytes_size, s1.c_str(), s2.c_str());
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

	log_debug ("upto = %ld, size = %ld", upto, bytes_size);
	if (upto < bytes_size) {
		PartitionPtr p = Partition::create();
		p->sub_type ("Space");
		p->sub_type ("Unallocated");
		p->parent_offset = upto;
		p->bytes_size = (bytes_size - upto);
		p->bytes_used = 0;
		vm.push_back(p);
	}

	for (auto& i : vm) {
		add_child (i, false);	// add_free()
	}

#if 0
	log_debug ("recap");
	for (auto& c : children) {
		std::string s1 = get_size (c->bytes_size);
		log_debug ("\t%-12s %12ld -> %12ld  %9s", c->name.c_str(), c->parent_offset, c->parent_offset + c->bytes_size, s1.c_str());
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

