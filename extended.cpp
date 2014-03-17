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

#include "extended.h"
#include "action.h"
#include "app.h"
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
	p->weak = p;

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
	// LOG_TRACE;
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
		std::cout << "Extended perform: " << action.name << std::endl;
		return true;
	} else {
		return Msdos::perform_action (action);
	}
}


ExtendedPtr
Extended::probe (ContainerPtr& UNUSED(top_level), ContainerPtr& parent, std::uint64_t offset, std::uint64_t size)
{
	//LOG_TRACE;
	ExtendedPtr ext;

#if 1
	std::uint8_t* buffer = nullptr;
	int bufsize = 512;
	//off_t seek = 0;
	//ssize_t count = 0;
	long table_offset = offset;

	// create extended
	// add to parent
	// create partitions
	// add to extended

	ext = Extended::create();

	ext->name = "extended";
	ext->bytes_size = size;
	ext->device = parent->device;
	ext->parent_offset = offset;

	buffer = (std::uint8_t*) malloc (bufsize);
	if (!buffer)
		return nullptr;

	PartitionPtr res1 = Partition::create();
	res1->name          = "Reserved";
	res1->sub_type ("Space");
	res1->sub_type ("Reserved");
	res1->bytes_size    = 512;		//align (512, 1024*1024);
	res1->bytes_used    = res1->bytes_size;
	res1->parent_offset = 0;					// Start of the partition
	ext->add_child (res1);		// change to add_reserved?

	for (int loop = 0; loop < 50; loop++) {		//what's the upper limit? prob 255 in the kernel
		std::string s = get_size (table_offset);
		//log_debug ("table_offset = %lld (%s)\n", table_offset, s.c_str());
#if 0
		/*FILE* f =*/ parent->open_device();
		/*int r =*/ parent->read_data (table_offset, bufsize, buffer);
#endif
		//log_debug ("f = %p, r = %d\n", f, r);
		long		bufsize = 1048576;	// 1 MiB
		std::uint8_t*	buffer  = parent->get_buffer (table_offset, bufsize);

		//dump_hex (buffer, bufsize);

		if (*(unsigned short int*) (buffer+510) != 0xAA55) {
			log_error ("not an extended partition\n");
			//log_debug ("%s (%s), %lld\n", parent->name.c_str(), parent->device.c_str(), parent->parent_offset);
			return nullptr;
		}

		//log_debug ("extended partition\n");

		int num = 0;
		std::vector<struct partition> vp;
		num = ext->read_table (buffer, bufsize, 0, vp);
		//log_debug ("num = %d\n", num);
		//dump_hex (buffer, bufsize);

		if ((num < 0) || (vp.size() > 2)) {
			log_error ("partition table is corrupt\n");	// bugger
			return nullptr;
		}

		for (auto& part : vp) {
#if 0
			if ((part.type != 5) || (part.type != 5)) {
				std::string s1 = get_size (part.start);
				std::string s2 = get_size (part.size);

				log_debug ("\tpartition (0x%02x)\n", part.type);
				log_debug ("\t\tstart = %lld (%s)\n", part.start, s1.c_str());
				log_debug ("\t\tsize  = %lld (%s)\n", part.size,  s2.c_str());
				log_debug ("\n");
			}
#endif
			ContainerPtr c;

			if ((part.type == 0x05) || (part.type == 0x0F)) {
				table_offset = offset + part.start;
			} else {
				c = MsdosPartition::create();
				c->name = "partition";
				c->bytes_size = part.size;

				//c->parent_offset = table_offset + part.start;
				//c->device = parent->device;

				c->parent_offset = table_offset + part.start - ext->parent_offset;

				std::string part_name = parent->get_device_name();
				//XXX check part_name isn't empty
				if (isdigit (part_name.back())) {
					part_name += 'p';
				}
				part_name += std::to_string (loop+5);

				c->device = part_name;

				ext->add_child(c);
				main_app->queue_add_probe(c);
			}
		}
		if (vp.size() == 1)
			break;
	}

	ext->fill_space();		// optional

	free (buffer);
#endif
	return ext;
}


std::uint8_t*
Extended::get_buffer (std::uint64_t offset, std::uint64_t size)
{
	// Our device is defective, so delegate to our parent
	// range check
	if ((size < 1) || ((offset + size) > bytes_size)) {
		log_error ("%s: out of range\n", __FUNCTION__);
		return nullptr;
	}

	ContainerPtr p = parent.lock();
	if (p) {
		return p->get_buffer (offset + parent_offset, size);
	} else {
		std::cout << this << std::endl;
		log_error ("%s: no device and no parent\n", __FUNCTION__);
		return nullptr;
	}
}

