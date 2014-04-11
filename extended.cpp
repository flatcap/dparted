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

#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>

#include <sstream>
#include <string>

#include "action.h"
#include "app.h"
#include "endian.h"
#include "extended.h"
#include "log.h"
#include "log_trace.h"
#include "main.h"
#include "msdos_partition.h"
#include "utils.h"
#include "visitor.h"

Extended::Extended (void)
{
	const char* me = "Extended";

	sub_type (me);
}

Extended::~Extended()
{
}

ExtendedPtr
Extended::create (void)
{
	ExtendedPtr p (new Extended());
	p->self = p;

	return p;
}


bool
Extended::accept (Visitor& v)
{
	ExtendedPtr e = std::dynamic_pointer_cast<Extended> (get_smart());
	if (!v.visit(e))
		return false;

	return visit_children(v);
}


std::vector<Action>
Extended::get_actions (void)
{
	LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.extended", true },
	};

	std::vector<Action> parent_actions = Msdos::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

bool
Extended::perform_action (Action action)
{
	if (action.name == "dummy.extended") {
		log_debug ("Extended perform: %s\n", action.name.c_str());
		return true;
	} else {
		return Msdos::perform_action (action);
	}
}


ContainerPtr
Extended::probe (ContainerPtr& parent, std::uint8_t* buffer, std::uint64_t bufsize)
{
	return_val_if_fail (parent, nullptr);
	return_val_if_fail (buffer, nullptr);
	LOG_TRACE;

	if (bufsize < 512)
		return nullptr;

	ExtendedPtr ext;
	std::uint64_t table_offset = 0;

	// create extended
	// add to parent
	// create partitions
	// add to extended

	ext = Extended::create();

	ext->bytes_size = bufsize;
	ext->device = parent->device;
	//ext->parent_offset = 0;		// Will be set by our parent

	PartitionPtr res1 = Partition::create();
	res1->sub_type ("Space");
	res1->sub_type ("Reserved");
	res1->bytes_size    = 512;		//align (512, 1024*1024);
	res1->bytes_used    = res1->bytes_size;
	res1->parent_offset = 0;					// Start of the partition
	ext->add_child (res1);		// change to add_reserved?

	for (int loop = 0; loop < 50; ++loop) {		//what's the upper limit? prob 255 in the kernel
		if (le16_to_cpup (table_offset+buffer+510) != 0xAA55) {
			log_error ("not an extended partition\n");
			//log_debug ("%s (%s), %ld\n", parent->name.c_str(), parent->device.c_str(), parent->parent_offset);
			return nullptr;
		}

		//log_debug ("extended partition\n");

		int num = 0;
		std::vector<struct partition> vp;
		num = ext->read_table (table_offset+buffer, bufsize, 0, vp);
		//log_debug ("num = %d\n", num);
		//dump_hex (buffer, bufsize);

		if ((num < 0) || (vp.size() > 2)) {
			log_error ("partition table is corrupt\n");	// bugger
			return nullptr;
		}

		for (auto& part : vp) {
#if 0
			if ((part.type != 0x05) || (part.type != 0x0F)) {
				std::string s1 = get_size (le64_to_cpu (part.start));
				std::string s2 = get_size (le64_to_cpu (part.size));

				log_debug ("\tpartition (0x%02x)\n", part.type);
				log_debug ("\t\tstart = %ld (%s)\n", le64_to_cpu (part.start), s1.c_str());
				log_debug ("\t\tsize  = %ld (%s)\n", le64_to_cpu (part.size),  s2.c_str());
				log_debug ("\n");
			}
#endif
			if ((part.type == 0x05) || (part.type == 0x0F)) {
				table_offset = le64_to_cpu (part.start);		// This is absolute
			} else {
				MsdosPartitionPtr m;

				m = MsdosPartition::create();
				m->bytes_size = le64_to_cpu (part.size);

				//m->parent_offset = table_offset + le64_to_cpu (part.start);
				//m->device = parent->device;

				m->parent_offset = table_offset + le64_to_cpu (part.start) - ext->parent_offset;	// This is relative

				m->device = make_part_dev (parent->get_device_name(), loop+5);
				//XXX check it's not empty

				ext->add_child(m);
				main_app->queue_add_probe(m);
			}
		}
		if (vp.size() == 1)
			break;
	}

	ext->fill_space();		// optional

	return ext;
}


std::uint8_t*
Extended::get_buffer (std::uint64_t offset, std::uint64_t size)
{
	// Our device is defective, so delegate to our parent
	// range check
	if ((size < 1) || ((offset + size) > bytes_size)) {
		log_error ("out of range\n");
		return nullptr;
	}

	ContainerPtr p = parent.lock();
	if (p) {
		return p->get_buffer (offset + parent_offset, size);
	} else {
		log_debug ("%s\n", this->dump());
		log_error ("no device and no parent\n");
		return nullptr;
	}
}

