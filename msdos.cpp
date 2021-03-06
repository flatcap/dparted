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
#include <fcntl.h>
#include <linux/fs.h>
#include <linux/hdreg.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <sstream>
#include <string>

#include "action.h"
#include "app.h"
#include "endian.h"
#include "extended.h"
#include "log.h"
#include "msdos.h"
#include "partition.h"
#include "utils.h"
#include "visitor.h"

Msdos::Msdos (void)
{
	LOG_CTOR;
	const char* me = "Msdos";

	sub_type (me);
}

Msdos::Msdos (const Msdos& c) :
	Table(c)
{
	Msdos();
	LOG_CTOR;
	// No properties
}

Msdos::Msdos (Msdos&& c)
{
	LOG_CTOR;
	swap(c);
}

Msdos::~Msdos()
{
	LOG_DTOR;
}

MsdosPtr
Msdos::create (void)
{
	MsdosPtr p (new Msdos());
	p->self = p;

	return p;
}


Msdos&
Msdos::operator= (const Msdos& UNUSED(c))
{
	// No properties

	return *this;
}

Msdos&
Msdos::operator= (Msdos&& c)
{
	swap(c);
	return *this;
}


void
Msdos::swap (Msdos& UNUSED(c))
{
	// No properties
}

void
swap (Msdos& lhs, Msdos& rhs)
{
	lhs.swap (rhs);
}


Msdos*
Msdos::clone (void)
{
	LOG_TRACE;
	return new Msdos (*this);
}


bool
Msdos::accept (Visitor& v)
{
	MsdosPtr m = std::dynamic_pointer_cast<Msdos> (get_smart());
	if (!v.visit(m))
		return false;

	return visit_children(v);
}


std::vector<Action>
Msdos::get_actions (void)
{
	LOG_TRACE;

	ContainerPtr me = get_smart();
	std::vector<Action> actions = {
		{ "dummy.msdos", "Dummy/Msdos", me, true },
	};

	std::vector<Action> base_actions = Table::get_actions();

	actions.insert (std::end (actions), std::begin (base_actions), std::end (base_actions));

	return actions;
}

bool
Msdos::perform_action (Action action)
{
	if (action.name == "dummy.msdos") {
		log_debug ("Msdos perform: %s", SP(action.name));
		return true;
	} else {
		return Table::perform_action (action);
	}
}


#if 0
void
Msdos::read_chs (std::uint8_t* buffer, std::uint16_t& cylinder, std::uint8_t& head, std::uint8_t& sector)
{
	return_if_fail (buffer);

	head     = buffer[0];
	sector   = buffer[1] & 0x3F;
	cylinder = buffer[2] + ((buffer[1] & 0xC0) << 2);
}
#endif

bool
Msdos::read_partition (std::uint8_t* buffer, int index, struct partition* part)
{
	//XXX include this in read_table?
	return_val_if_fail (buffer, false);
	return_val_if_fail (part,   false);

	if ((index < 0) || (index > 3))
		return false;

	buffer += 446;
	index  *= 16;

	if (buffer[index + 4] == 0)
		return false;

	part->type = buffer[index + 4];

	part->start = le32_to_cpup (buffer + index + 8);
	part->start *= 512;

	part->size = le32_to_cpup (buffer + index + 12);
	part->size *= 512;

	return true;
}

unsigned int
Msdos::read_table (std::uint8_t* buffer, std::uint64_t bufsize, std::uint64_t offset, std::vector<struct partition>& vp)
{
	return_val_if_fail (buffer, 0);

	if ((offset+512) > bufsize)		// Min size for Msdos is 512B
		return 0;

	struct partition part;

	for (int i = 0; i < 4; ++i) {
		if (!read_partition (buffer, i, &part))	// could flag Msdos object as invalid
			continue;		//XXX or return -1

		vp.push_back (part);
	}

	return vp.size();
}


bool
Msdos::probe (ContainerPtr& parent, std::uint8_t* buffer, std::uint64_t bufsize)
{
	return_val_if_fail (parent,  false);
	return_val_if_fail (buffer,  false);
	return_val_if_fail (bufsize, false);
	LOG_TRACE;

	int count = 0;

	if (le16_to_cpup (buffer+510) != 0xAA55)	//XXX declare magic elsewhere
		return false;

	// and some other quick checks

	MsdosPtr m = Msdos::create();

	ContainerPtr new_parent = Container::start_transaction (parent, "Msdos: probe");
	if (!new_parent) {
		log_error ("gpt probe failed");
		return false;
	}

	m->bytes_size = new_parent->bytes_size;
	// m->device = new_parent->device;	//XXX only for partitions, main body should inherit
	m->parent_offset = 0;

	if (!new_parent->add_child (m, false)) {
		log_error ("add failed");
		Container::cancel_transaction();
		return false;
	}

	std::vector<struct partition> vp;
	count = m->read_table (buffer, bufsize, 0, vp);

	if ((count < 0) || (vp.size() > 4)) {
		log_debug ("partition table is corrupt");	// bugger
		Container::cancel_transaction();
		return false;
	}

	PartitionPtr res1 = Partition::create();
	res1->sub_type ("Space");
	res1->sub_type ("Reserved");
	res1->bytes_size    = 512;		// align (512, 1024*1024);
	res1->bytes_used    = res1->bytes_size;
	res1->parent_offset = 0;					// Start of the partition
	if (!m->add_child (res1, false)) {
		log_error ("add failed");
		Container::cancel_transaction();
		return false;
	}

	for (unsigned int i = 0; i < vp.size(); ++i) {
		std::string s1 = get_size (le64_to_cpu (vp[i].start));
		std::string s2 = get_size (le64_to_cpu (vp[i].size));

		log_debug ("partition %d (0x%02x)", i+1, vp[i].type);
		log_debug ("\tstart = %ld (%s)", le64_to_cpu (vp[i].start), SP(s1));
		log_debug ("\tsize  = %ld (%s)", le64_to_cpu (vp[i].size),  SP(s2));

		ContainerPtr c;

		std::string part_name = make_part_dev (new_parent->device, i+1);
		//XXX check it's not empty

		if ((vp[i].type == 0x05) || (vp[i].type == 0x0F)) {
			//XXX validate start&size against parent buffer
#if 0
			log_debug ("vp[i].start = %ld (%s)", le64_to_cpu (vp[i].start), SP(get_size (le64_to_cpu (vp[i].start))));
			log_debug ("vp[i].size  = %ld (%s)", le64_to_cpu (vp[i].size),  SP(get_size (le64_to_cpu (vp[i].size))));
			std::uint64_t xstart = le64_to_cpu (vp[i].start);
			std::uint64_t xsize  = le64_to_cpu (vp[i].size);
			dump_hex2 (buffer, xstart-2000, 4096);
#endif
			ContainerPtr m2(m);
			c = Extended::probe (m2, buffer+le64_to_cpu (vp[i].start), le64_to_cpu (vp[i].size));
			if (!c)
				continue;

			c->parent_offset = le64_to_cpu (vp[i].start);
			c->device = part_name;
			if (!m->add_child (c, false)) {
				log_error ("add failed");
				Container::cancel_transaction();
				return false;
			}
		} else {
			PartitionPtr p = Partition::create();
			p->ptype = vp[i].type;
			c = p;
			c->bytes_size = le64_to_cpu (vp[i].size);

			c->parent_offset = le64_to_cpu (vp[i].start);
			c->device = part_name;

			if (!m->add_child (c, true)) {
				log_error ("add failed");
				Container::cancel_transaction();
				return false;
			}
		}
	}

	m->fill_space();		// optional

	Container::commit_transaction();

	return true;
}


