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

/* GPT -- GUID Partition Table
 *
 * https://en.wikipedia.org/wiki/GUID_Partition_Table
 *
 * Read from disk:
 *	A2 A0 D0 EB E5 B9 33 44 87 C0 68 B6 B7 26 99 C7
 *
 * From docs:
 *	EBD0A0A2-B9E5-4433-87C0-68B6B72699C7
 *
 * std::uint64_t LE
 *   dash
 * std::uint16_t LE
 *   dash
 * std::uint16_t LE
 *   dash
 * std::uint16_t LE
 *   dash
 * std::uint8_t
 * std::uint8_t
 * std::uint8_t
 * std::uint8_t
 * std::uint8_t
 * std::uint8_t
 */

#include <cstring>
#include <iomanip>
#include <iterator>
#include <sstream>
#include <utility>
#include <vector>

#include "gpt.h"
#include "action.h"
#include "app.h"
#include "endian.h"
#include "filesystem.h"
#include "log.h"
#ifdef DP_MSDOS
#include "msdos_partition.h"
#endif
#include "utils.h"
#include "visitor.h"

Gpt::Gpt (void)
{
	LOG_CTOR;
	const char* me = "Gpt";

	sub_type (me);
}

Gpt::Gpt (const Gpt& c) :
	Table(c)
{
	Gpt();
	LOG_CTOR;
	// No properties
}

Gpt::Gpt (Gpt&& c)
{
	LOG_CTOR;
	swap(c);
}

Gpt::~Gpt()
{
	LOG_DTOR;
}

GptPtr
Gpt::create (void)
{
	GptPtr p (new Gpt());
	p->self = p;

	return p;
}


Gpt&
Gpt::operator= (const Gpt& UNUSED(c))
{
	// No properties

	return *this;
}

Gpt&
Gpt::operator= (Gpt&& c)
{
	swap(c);
	return *this;
}


void
Gpt::swap (Gpt& UNUSED(c))
{
	// No properties
}

void
swap (Gpt& lhs, Gpt& rhs)
{
	lhs.swap (rhs);
}


Gpt*
Gpt::clone (void)
{
	LOG_TRACE;
	return new Gpt (*this);
}


bool
Gpt::accept (Visitor& v)
{
	GptPtr g = std::dynamic_pointer_cast<Gpt> (get_smart());
	if (!v.visit(g))
		return false;

	return visit_children(v);
}


std::vector<Action>
Gpt::get_actions (void)
{
	LOG_TRACE;

	ContainerPtr me = get_smart();
	std::vector<Action> actions = {
		{ "tidy.gpt",   "Tidy Gpt",   me, true },
		{ "delete.gpt", "Delete/Gpt", me, true },
		{ "resize.gpt", "Resize/Gpt", me, true },	//XXX dependent on parent being resizeable (e.g. loop)
	};

	std::vector<Action> base_actions = Table::get_actions();

	actions.insert (std::end (actions), std::begin (base_actions), std::end (base_actions));

	return actions;
}

bool
Gpt::perform_action (Action action)
{
	if (action.name == "tidy.gpt") {
		log_error ("Gpt perform: %s", SP(action.name));
		return true;
	} else if (action.name == "delete.gpt") {
		log_error ("Gpt perform: %s", SP(action.name));
		return true;
	} else if (action.name == "resize.gpt") {
		log_error ("Gpt perform: %s", SP(action.name));
		return true;
	} else {
		return Table::perform_action (action);
	}
}


static std::string
read_guid (std::uint8_t* buffer)
{
	return_val_if_fail (buffer, "");

	std::uint32_t a = le32_to_cpup (buffer+0);
	std::uint16_t b = le16_to_cpup (buffer+4);
	std::uint16_t c = le16_to_cpup (buffer+6);
	std::uint16_t d = be16_to_cpup (buffer+8);

	std::stringstream ss;

	ss << std::setfill ('0') << std::hex << std::setiosflags (std::ios::uppercase);

	ss << a << '-' << b << '-' << c << '-' << d << '-';

	for (int i = 10; i < 16; ++i) {
		ss << std::setw(2) << static_cast<unsigned> (buffer[i]);
	}

	return ss.str();
}

bool
Gpt::probe (ContainerPtr& parent, std::uint8_t* buffer, std::uint64_t bufsize)
{
	return_val_if_fail (parent, false);
	return_val_if_fail (buffer, false);
	LOG_TRACE;

	if (bufsize < 36864)		// Min size for gpt is 36KiB
		return false;

	// If container is smaller that this, even an empty Gpt won't fit

	if (strncmp ((char*) buffer+512, "EFI PART", 8))	//XXX replace with strict identify function (static) and all other probes
		return false;

	/* LBA		Description
	 * ---------------------------------
	 * 0		protective mbr
	 * 1		primary gpt header
	 * 2-33		128 gpt entries
	 * ...
	 * -33		128 gpt entries
	 * -1		secondary gpt header
	 */

	GptPtr g = create();

	std::vector<std::pair<std::uint64_t, std::uint64_t>> empty = { { 0, (parent->bytes_size/512)-1 } };

	g->bytes_size = parent->bytes_size;
	g->bytes_used = 0;
	g->parent_offset = 0;
	g->block_size = 0;
	g->uuid = read_guid (buffer+568);

	ContainerPtr new_parent = Container::start_transaction (parent, "Gpt: probe");
	if (!new_parent) {
		log_error ("gpt probe failed");
		return false;
	}

	if (!new_parent->add_child (g, false))
		return false;

	// Assumption: 1MiB alignment (for now)
	// Should reserved bits be allocated after actual partitions?
	// Should we allow Misc to overlap real partitions?
	// Then once a non-aligned partition is deleted
	// it would be replaced by an aligned partition.

	PartitionPtr res1 = Partition::create();
	res1->sub_type ("Space");
	res1->sub_type ("Reserved");
	res1->bytes_size    = 512 * 34;		// align (512 * 34, 1024*1024);
	res1->bytes_used    = res1->bytes_size;
	res1->parent_offset = 0;					// Start of the partition
	if (!g->add_child (res1, false)) {		// change to add_reserved?
		return false;
	}

	PartitionPtr res2 = Partition::create();
	res2->sub_type ("Space");
	res2->sub_type ("Reserved");
	res2->bytes_size    = 512 * 33;		// align (512 * 33, 1024*1024);
	res2->bytes_used    = res2->bytes_size;
	res2->parent_offset = g->bytes_size - res2->bytes_size;		// End of the partition
	if (!g->add_child (res2, false)) {
		return false;
	}

	delete_region (empty, 0, 34);

	std::uint64_t sect_offset = (g->bytes_size - res2->bytes_size) / 512;
	delete_region (empty, sect_offset, 33);

	buffer += 1024;
	// bufsize -= 1024;	//XXX not used
				// for range checking
	std::string device = g->get_device_name();

	for (int i = 0; i < 128; ++i, buffer += 128) {
		if (le64_to_cpup (buffer+32) == 0)
			continue;			// Skip empty slots

		GptPartitionPtr p = GptPartition::create();
		p->bytes_used = 0;
		p->uuid = read_guid (buffer+16);
		std::string part_type_uuid = read_guid (buffer+0);		//XXX not saved

		p->device = make_part_dev (device, i+1);
		//XXX check it's not empty

		std::uint64_t start  = le64_to_cpup (buffer+32);
		std::uint64_t finish = le64_to_cpup (buffer+40);

		log_debug ("%2d: %9ld -%9ld  %10ld  %ld", i, start, finish, (finish-start+1)*512, (finish-start+1)*512/1024/1024);

		delete_region (empty, start, finish-start+1);

		p->parent_offset = start * 512;
		p->bytes_size = (finish - start + 1) * 512;
#if 0
		p->name = get_fixed_str (buffer+56, 72);		//XXX UTF-16?
#else
		if (buffer[56]) {
			p->name.clear();
			for (int j = 0; j < 32; j += 2) {
				if (buffer[56+j] == 0) {
					break;
				}
				p->name += buffer[56+j];
			}
		}
#endif

		std::string s = get_size (p->bytes_size);
		log_debug ("\t\tlabel  = %s",   SP(p->name));
		log_debug ("\t\t\tstart  = %ld", le64_to_cpup (buffer+32) * 512);
		log_debug ("\t\t\tfinish = %ld", le64_to_cpup (buffer+40) * 512);
		log_debug ("\t\t\tsize   = %ld (%s)", p->bytes_size, SP(s));

		if (!g->add_child (p, true)) {
			return false;
		}
	}

	for (auto& r : empty) {
		PartitionPtr p = Partition::create();
		p->bytes_size = (r.second-r.first+1);	p->bytes_size    *= 512;	//XXX two parts to avoid overflow
		p->parent_offset = r.first;		p->parent_offset *= 512;
		if (r.first == 0) {
			p->sub_type ("Space");
			p->sub_type ("Reserved");
			p->bytes_used = p->bytes_size;
		} else {
			p->sub_type ("Space");
			p->sub_type ("Unallocated");
			p->bytes_used = p->bytes_size;
		}
		if ((p->parent_offset == 17408) && (p->bytes_size == 1031168)) {
			log_error ("skipping alignment space");
		} else {
			if (!g->add_child (p, false)) {
				return false;
			}
		}
	}

	Container::commit_transaction();
	return true;
}

