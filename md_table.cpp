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

#include <string>

#include "md_table.h"
#include "action.h"
#include "log_trace.h"
#include "utils.h"
#include "visitor.h"

MdTable::MdTable (void)
{
	// Save a bit of space
	const char* me = "MdTable";
	const int   s  = (int) BaseProperty::Flags::Size;

	sub_type (me);

	declare_prop (me, "chunks_used", chunks_used, "desc of chunks_used", 0);
	declare_prop (me, "chunk_size",  chunk_size,  "desc of chunk_size",  s);
	declare_prop (me, "data_offset", data_offset, "desc of data_offset", s);
	declare_prop (me, "data_size",   data_size,   "desc of data_size",   s);
	declare_prop (me, "raid_disks",  raid_disks,  "desc of raid_disks",  0);
	declare_prop (me, "raid_layout", raid_layout, "desc of raid_layout", 0);
	declare_prop (me, "raid_type",   raid_type,   "desc of raid_type",   0);
	declare_prop (me, "vol_name",    vol_name,    "desc of vol_name",    0);
	declare_prop (me, "vol_uuid",    vol_uuid,    "desc of vol_uuid",    0);
}

MdTable::~MdTable()
{
}

MdTablePtr
MdTable::create (void)
{
	MdTablePtr p (new MdTable());
	p->weak = p;

	return p;
}


bool
MdTable::accept (Visitor& v)
{
	MdTablePtr m = std::dynamic_pointer_cast<MdTable> (get_smart());
	if (!v.visit(m))
		return false;
	return visit_children(v);
}


std::vector<Action>
MdTable::get_actions (void)
{
	// LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.md_table", true },
	};

	std::vector<Action> parent_actions = Table::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

bool
MdTable::perform_action (Action action)
{
	if (action.name == "dummy.md_table") {
		std::cout << "MdTable perform: " << action.name << std::endl;
		return true;
	} else {
		return Table::perform_action (action);
	}
}


std::string
read_uuid_string (std::uint8_t* buffer)
{
	char uuid[40];

	snprintf (uuid, sizeof (uuid), "%02x%02x%02x%02x:%02x%02x%02x%02x:%02x%02x%02x%02x:%02x%02x%02x%02x",
		buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8], buffer[9], buffer[10], buffer[11], buffer[12], buffer[13], buffer[14], buffer[15]);

	return uuid;
}


static bool
is_mdtable (std::uint8_t* buffer)
{
	if (!buffer)
		return false;

	if ((*(unsigned int*) buffer) != 0xA92B4EFC)
		return false;

	return true;
}


ContainerPtr
MdTable::probe (ContainerPtr& parent, std::uint8_t* buffer, std::uint64_t bufsize)
{
	//LOG_TRACE;

	if (!parent || !buffer || !bufsize)
		return nullptr;

	buffer  += 4096;	// Ignore the first 4KiB
	//bufsize -= 4096;

	if (!is_mdtable (buffer))
		return nullptr;

	std::string vol_uuid = read_uuid_string (buffer + 16);
	//log_info ("vol uuid = %s\n", vol_uuid.c_str());

	std::string vol_name ((char*) (buffer + 32));
	//log_info ("vol name = %s\n", vol_name.c_str());

	int raid_type   = buffer[72];
	int raid_layout = buffer[76];
	//log_info ("raid type = %d (%d)\n", raid_type, raid_layout);

	int raid_disks = buffer[92];
	//log_info ("%d raid disks\n", raid_disks);

	int chunk_size = *(int*) (buffer + 88);
	//log_info ("chunk size = %d\n", chunk_size);

	long chunks_used = *(long*) (buffer + 80);
	//log_info ("chunks used = %ld (%s)\n", chunks_used, get_size (chunk_size * chunks_used).c_str());

	std::string dev_uuid = read_uuid_string (buffer + 168);
	//log_info ("dev uuid = %s\n", dev_uuid.c_str());

	long data_offset = *(long*) (buffer + 128) * 512;
	long data_size   = *(long*) (buffer + 136) * 512;

	//log_info ("data offset/size = %ld (%s), %ld (%s)\n", data_offset, get_size (data_offset).c_str(), data_size, get_size (data_size).c_str());

	//log_info ("mdtable\n");
	MdTablePtr t = MdTable::create();

	t->bytes_size		= data_offset + data_size;
	t->uuid			= dev_uuid;
	t->vol_uuid		= vol_uuid;
	t->vol_name		= vol_name;
	t->raid_type		= raid_type;
	t->raid_layout		= raid_layout;
	t->raid_disks		= raid_disks;
	t->chunk_size		= chunk_size;
	t->chunks_used		= chunks_used;
	t->data_offset		= data_offset;
	t->data_size		= data_size;

	ContainerPtr c(t);
	parent->add_child(c);

	return t;
}


