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
#include <cstring>

#include "log.h"
#include "utils.h"
#include "log_trace.h"
#include "main.h"
#include "lvm_group.h"
#include "lvm_partition.h"
#include "lvm2.h"

#include "lvm_table.h"
#include "action.h"
#include "utils.h"
#include "visitor.h"

LvmTable::LvmTable (void)
{
	// Save a bit of space
	const char* me = "LvmTable";
	const int   s  = (int) BaseProperty::Flags::Size;

	sub_type (me);

	declare_prop (me, "config",        config,        "desc of config",        0);
	declare_prop (me, "metadata_size", metadata_size, "desc of metadata_size", s);
	declare_prop (me, "pv_attr",       pv_attr,       "desc of pv_attr",       0);
}

LvmTable::~LvmTable()
{
}

LvmTablePtr
LvmTable::create (void)
{
	LvmTablePtr p (new LvmTable());
	p->weak = p;

	return p;
}


bool
LvmTable::accept (Visitor& v)
{
	LvmTablePtr l = std::dynamic_pointer_cast<LvmTable> (get_smart());
	if (!v.visit(l))
		return false;
	return visit_children(v);
}


std::vector<Action>
LvmTable::get_actions (void)
{
	// LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.lvm_table", true },
	};

	std::vector<Action> parent_actions = Table::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

bool
LvmTable::perform_action (Action action)
{
	if (action.name == "dummy.lvm_table") {
		std::cout << "LvmTable perform: " << action.name << std::endl;
		return true;
	} else {
		return Table::perform_action (action);
	}
}


std::string
read_uuid_string (char* buffer)
{
	if (!buffer)
		return "";

	std::string uuid ((char*) buffer, 32);

	uuid.insert (26, 1, '-');
	uuid.insert (22, 1, '-');
	uuid.insert (18, 1, '-');
	uuid.insert (14, 1, '-');
	uuid.insert (10, 1, '-');
	uuid.insert ( 6, 1, '-');

	return uuid;
}


static struct label_header*
get_label_header (std::uint8_t* buffer)
{
	struct label_header* lh = (struct label_header*) buffer;

	if (!lh)
		return nullptr;

	if (strncmp ((char*) lh->id, "LABELONE", 8))
		return nullptr;

	if (strncmp ((char*) lh->type, "LVM2 001", 8))
		return nullptr;

	//XXX validate crc, range check offset
	return lh;
}

static struct pv_header*
get_pv_header (std::uint8_t* buffer)
{
	struct pv_header* ph = (struct pv_header*) buffer;

	if (!ph)
		return nullptr;

	//XXX validate chars in uuid, disk_areas
	return ph;
}

static struct mda_header*
get_mda_header (std::uint8_t* buffer)
{
	struct mda_header* mh = (struct mda_header*) buffer;

	if (!mh)
		return nullptr;

	if (strncmp ((char*) mh->magic, FMTT_MAGIC, 16))
		return nullptr;

	if (mh->version != FMTT_VERSION)
		return nullptr;

	//XXX validate checksum
	return mh;
}

#if 0
static int
get_seq_num (const std::string& config)
{
	size_t index = config.find ("seqno = ");
	size_t end   = config.find ('\n', index);

	if (index == std::string::npos)
		return 0;

	//log_info ("index = %ld, end = %ld, count = %ld\n", index, end, end-index-8);

	StringNum sn (config.substr (index+8, end-index-7));
	//log_info ("num = %.5s\n", sn.c_str());

	return sn;
}

#endif
static std::string
get_vol_name (const std::string& config)
{
	size_t end = config.find (" {\n");

	if (end == std::string::npos)
		return "";

	return config.substr (0, end);
}


#if 0
static void
format_config (std::string& config)
{
	size_t index = 0;
	size_t first;
	std::string indent;

	for (int i = 0; i < 1000; i++) {
		first = config.find_first_of ("[]{}\n", index);
		if (first == std::string::npos)
			break;
		//log_info ("first = %lu '%c'\n", first, config[first] == '\n' ? '@' : config[first]);

		switch (config[first]) {
			case '[':
			case '{':
				indent += "\t";
				break;
			case ']':
			case '}':
				indent = indent.substr(1);
				if (config[first-1] == '\t') {
					config.erase (first-1, 1);
					first--;
				}
				break;
			case '\n':
				if (config[first+1] != '\n') {
					config.insert (first+1, indent);
					first += indent.size();
				}
				break;
		}

		index = first + 1;
	}
}

#endif
ContainerPtr
LvmTable::probe (ContainerPtr& parent, std::uint8_t* buffer, std::uint64_t bufsize)
{
	//LOG_TRACE;
	LvmTablePtr t;

	//XXX check bufsize gives us all the data we need

	struct label_header* lh = get_label_header (buffer+512);
	if (!lh)
		return nullptr;

	//log_info ("'%.8s', %lu, 0x%8x, %u, '%.8s'\n", lh->id, lh->sector_xl, lh->crc_xl, lh->offset_xl, lh->type);

	struct pv_header* ph = get_pv_header (buffer + 512 + lh->offset_xl);
	if (!ph)
		return nullptr;

	std::string pv_uuid = read_uuid_string ((char*) ph->pv_uuid);

	//log_info ("%s, %lu (%s)\n", pv_uuid.c_str(), ph->device_size_xl, get_size (ph->device_size_xl).c_str());

#if 0
	log_info ("Disk locations:\n");
	int i;
	for (i = 0; i < 8; i++) {
		if (ph->disk_areas_xl[i].offset == 0)
			break;
		log_info ("\t%lu, %lu\n", ph->disk_areas_xl[i].offset, ph->disk_areas_xl[i].size);
	}
#endif

#if 0
	log_info ("Metadata locations:\n");
	for (i++; i < 8; i++) {
		if (ph->disk_areas_xl[i].offset == 0)
			break;
		log_info ("\t%lu, %lu (%lu)\n", ph->disk_areas_xl[i].offset, ph->disk_areas_xl[i].size, ph->disk_areas_xl[i].offset + ph->disk_areas_xl[i].size);
	}
#endif

	//XXX 4096 from metadata location
	struct mda_header* mh = get_mda_header (buffer + 4096);
	if (!mh)
		return nullptr;

	//log_info ("0x%08x, '%.16s', %u, %lu, %lu\n", mh->checksum_xl, mh->magic, mh->version, mh->start, mh->size);

#if 0
	log_info ("Metadata:\n");
	for (i = 0; i < 4; i++) {
		if (mh->raw_locns[i].offset == 0)
			break;
		log_info ("\t%lu (0x%lx), %lu, 0x%08x, %u\n", mh->raw_locns[i].offset, mh->raw_locns[i].offset, mh->raw_locns[i].size, mh->raw_locns[i].checksum, mh->raw_locns[i].flags);
	}
#endif

	std::uint64_t offset = mh->raw_locns[0].offset;
	std::uint64_t size   = mh->raw_locns[0].size;

	std::string config;
	std::string vol_name;
	//int seq_num = -1;

	if ((offset+size) > bufsize) {
		//log_info ("TOO BIG (%d > %d)\n", (offset+size), bufsize);
		//return nullptr;
	} else {
		if (size > 0) {
			config = std::string ((char*) (buffer+4096+offset), size-1);

			//seq_num = get_seq_num (config);

			//log_info ("seq num = %d\n", seq_num);
			vol_name = get_vol_name (config);
#if 0
			log_info ("Config (0x%0x):\n", 4096+offset);
			format_config (config);
			std::cout << "\n" << config << "\n";
#endif
		}
	}

	t = LvmTable::create();
	//log_debug ("new LvmTable %s (%p)\n", pv_uuid.c_str(), (void*) t.get());

	t->bytes_size = ph->device_size_xl;
	t->parent_offset = 0;
	t->bytes_used = 0;
	t->config = config;
	t->uuid = pv_uuid;

	t->metadata_size = 1048576;		//XXX read from header

	ContainerPtr c(t);
	parent->add_child(c);

	PartitionPtr p = Partition::create();
	p->sub_type ("Space");
	p->sub_type ("Reserved");
	p->bytes_size = t->metadata_size;
	p->bytes_used = t->metadata_size;
	p->parent_offset = 0;
	c->add_child(p);

	//XXX add alignment -- can't do this without the group's block size

	return t;
}


void
LvmTable::add_child (ContainerPtr& child)
{
	if (!child)
		return;

	//printf ("TABLE: parent offset = %ld\n", child->parent_offset);
	if (!child->is_a ("Space")) {
		child->parent_offset += metadata_size;
	}

	Table::add_child (child);

	//child->open_device();	// get a buffer

#if 0
	printf ("%p, name %s, type %s, uuid: %s\n", child->mmap_buffer, child->name.c_str(), child->type.back().c_str(), child->uuid.c_str());
	dump_hex2 (child->mmap_buffer, 0, 4096);
	printf ("\n");
#endif
}


bool
LvmTable::set_alignment (long bytes)
{
	Table::set_alignment (bytes);

	// We expect a Reserved at the start...
	if (children.empty())
		return false;

	// and possibly an Alignment at the end
	auto it = children.rbegin();
	ContainerPtr last = *it;
	if (last->is_a ("Alignment")) {
		children.erase (last);
	}

	ContainerPtr reserved = *children.begin();

	//XXX validate numbers
	long remainder = (bytes_size - reserved->bytes_size) % block_size;

#if 0
	printf ("size of device   = %10ld\n", bytes_size);
	printf ("size of reserved = %10ld\n", reserved->bytes_size);
	printf ("block size       = %10ld\n", block_size);
	printf ("remainder        = %10ld\n", remainder);
#endif

	PartitionPtr s = Partition::create();
	s->sub_type ("Space");
	s->sub_type ("Alignment");
	s->bytes_size = remainder;
	s->bytes_used = remainder;
	s->parent_offset = bytes_size - remainder;
	ContainerPtr c(s);
	add_child (c);

	return true;
}
