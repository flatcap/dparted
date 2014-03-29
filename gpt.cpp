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
#include "log_trace.h"
#include "main.h"
#ifdef DP_MSDOS
#include "msdos_partition.h"
#endif
#include "utils.h"
#include "visitor.h"

Gpt::Gpt (void)
{
	const char* me = "Gpt";

	sub_type (me);
}

Gpt::~Gpt()
{
}

GptPtr
Gpt::create (void)
{
	GptPtr p (new Gpt());
	p->self = p;

	return p;
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
	 //LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.gpt", true },
	};

	std::vector<Action> parent_actions = Table::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

bool
Gpt::perform_action (Action action)
{
	if (action.name == "dummy.gpt") {
		log_debug ("Gpt perform: %s\n", action.name.c_str());
		return true;
	} else {
		return Table::perform_action (action);
	}
}


static std::string
read_guid (std::uint8_t* buffer)
{
	if (!buffer)
		return "";

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

void
delete_region (std::vector<std::pair<int,int>>& region, int start, int finish)
{
	 //log_debug ("delete: (%d,%d): ", start, finish);
	for (auto it = std::begin (region); it != std::end (region); ++it) {
		if (finish < (*it).first)
			continue;

		if (start > (*it).second)
			continue;

		/* xxxx = Delete, <--> = Remainder
		 * 1 |xxxxxxxxxxx|	 s &  f		delete entire region
		 * 2 |xxx<------>|	 s & !f		move start
		 * 3 |<------>xxx|	!s &  f		move end
		 * 4 |<-->xxx<-->|	!s & !f		split region
		 */
		if (start == (*it).first) {
			if (finish == (*it).second) {		//1
				 //log_debug ("1\n");
				region.erase (it);
				break;
			} else {				//2
				 //log_debug ("2\n");
				(*it).first = finish;
				break;
			}
		} else {
			if (finish == (*it).second) {		//3
				 //log_debug ("3\n");
				(*it).second = start;
				break;
			} else {				//4
				 //log_debug ("4\n");
				int end = (*it).second;
				(*it).second = start;
				region.insert (it+1, { finish, end });
				break;
			}
		}
	}
}

bool
Gpt::probe (ContainerPtr& parent, std::uint8_t* buffer, std::uint64_t bufsize)
{
	//LOG_TRACE;

	if (!parent || !buffer || !bufsize)
		return false;

	//XXX check min size against bufsize ((34*512) + (33*512)) bytes and all other probes
	// If container is smaller that this, even an empty Gpt won't fit

	if (strncmp ((char*) buffer+512, "EFI PART", 8))	//XXX replace with strict identify function (static) and all other probes
		return false;

	// LBA		Description
	// ---------------------------------
	// 0		protective mbr
	// 1		primary gpt header
	// 2-33		128 gpt entries
	// ...
	// -33		128 gpt entries
	// -1		secondary gpt header

	GptPtr g = create();

	std::vector<std::pair<int,int>> empty = { { 0, (parent->bytes_size/512)-1 } };

	g->bytes_size = parent->bytes_size;
	g->bytes_used = 0;
	g->parent_offset = 0;
	g->block_size = 0;
	g->uuid = read_guid (buffer+568);

	parent->add_child(g);

	// Assumption: 1MiB alignment (for now)
	// Should reserved bits be allocated after actual partitions?
	// Should we allow Misc to overlap real partitions?
	// Then once a non-aligned partition is deleted
	// it would be replaced by an aligned partition.

	PartitionPtr res1 = Partition::create();
	res1->sub_type ("Space");
	res1->sub_type ("Reserved");
	res1->bytes_size    = 512 * 34;		//align (512 * 34, 1024*1024);
	res1->bytes_used    = res1->bytes_size;
	res1->parent_offset = 0;					// Start of the partition
	g->add_child (res1);		// change to add_reserved?

	PartitionPtr res2 = Partition::create();
	res2->sub_type ("Space");
	res2->sub_type ("Reserved");
	res2->bytes_size    = 512 * 33;		//align (512 * 33, 1024*1024);
	res2->bytes_used    = res2->bytes_size;
	res2->parent_offset = g->bytes_size - res2->bytes_size;		// End of the partition
	g->add_child (res2);

	delete_region (empty, 0, 33);

	std::uint64_t sect_offset = (g->bytes_size - res2->bytes_size) / 512;
	delete_region (empty, sect_offset, sect_offset+32);

	buffer += 1024;	//bufsize -= 1024; for range checking

	std::string device = g->get_device_name();

	for (int i = 0; i < 128; ++i, buffer += 128) {
		if (le64_to_cpup (buffer+32) == 0)
			continue;			// Skip empty slots

		GptPartitionPtr p = GptPartition::create();
		p->bytes_used = 0;
		p->uuid = read_guid (buffer+16);
		std::string part_type_uuid = read_guid (buffer+0);		//XXX not saved

		p->device = make_part_dev (device, i+i);
		//XXX check it's not empty

		std::uint64_t start  = le64_to_cpup (buffer+32);
		std::uint64_t finish = le64_to_cpup (buffer+40);

		//log_debug ("%2d: %9ld -%9ld  %10ld  %ld\n", i, start, finish, (finish-start+1)*512, (finish-start+1)*512/1024/1024);

		delete_region (empty, start-1, finish);

		p->parent_offset = start * 512;
		p->bytes_size = (finish - start + 1) * 512;
		p->name = get_fixed_str (buffer+56, 72);

#if 0
		std::string s = get_size (p->bytes_size);
		log_debug ("\t\tlabel  = %s\n",   p->name.c_str());
		log_debug ("\t\t\tstart  = %ld\n", le64_to_cpup (buffer+32) * 512);
		log_debug ("\t\t\tfinish = %ld\n", le64_to_cpup (buffer+40) * 512);
		log_debug ("\t\t\tsize   = %ld (%s)\n", p->bytes_size, s.c_str());
#endif
		g->add_child(p);
		main_app->queue_add_probe(p);
	}

	for (auto r : empty) {
		//log_debug ("(%d,%d) ", r.first, r.second);

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
			p->bytes_used = 0;
		}
		g->add_child(p);		// change to add_reserved?
	}
	//log_debug ("\n");


#if 0
	g->fill_space();		// optional
#endif

	return true;
}


