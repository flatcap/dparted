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

#include <iostream>
#include <sstream>
#include <utility>
#include <ctime>

#include "dot_visitor.h"
#include "container.h"
#include "loop.h"
#include "gpt.h"
#include "whole.h"
#include "block.h"
#include "partition.h"
#include "filesystem.h"
#include "luks_table.h"
#include "lvm_group.h"
#include "lvm_volume.h"
#include "log_trace.h"
#include "utils.h"

DotVisitor::DotVisitor (void)
{
}

DotVisitor::~DotVisitor()
{
}


bool
DotVisitor::visit_enter (ContainerPtr& c)
{
	parents.push (c);
	return true;
}

bool
DotVisitor::visit_leave (void)
{
	parents.pop();
	return true;
}


std::string
get_colour (ContainerPtr c)
{
	if (c->is_a ("Table"))      return "#FFFFCC";
	if (c->is_a ("Block"))      return "#FFB6C1";
	if (c->is_a ("Whole"))      return "#2BDCFF";
	if (c->is_a ("Filesystem")) return "#B4A0F3";
	if (c->is_a ("Piece"))      return "#B0D0B0";
	if (c->is_a ("File"))       return "#00FF00";
	if (c->is_a ("Misc"))       return "#DC4C46";
	if (c->is_a ("Container"))  return "#FFFFFF";

	return "white";
}


/**
 * dot_row (bool)
 */
std::string
dot_row (const char* name, bool value)
{
	std::stringstream row;

	row << "\t\t<tr>";
	row << "<td align=\"left\">" << name << "</td>";
	row << "<td>=</td>";
	row << "<td align=\"left\">" << (value ? "true" : "false") << "</td>";
	row << "</tr>\n";

	return row.str();
}

/**
 * dot_row (int)
 */
std::string
dot_row (const char* name, int value)
{
	std::stringstream row;

	row << "\t\t<tr>";
	row << "<td align=\"left\">" << name << "</td>";
	row << "<td>=</td>";
	row << "<td align=\"left\">" << value << "</td>";
	row << "</tr>\n";

	return row.str();
}

/**
 * dot_row (long)
 */
std::string
dot_row (const char* name, long value)
{
	std::stringstream row;
	std::string str;

	if (value > 1023) {
		str = " (" + get_size (value) + ")";
	}

	row << "\t\t<tr>";
	row << "<td align=\"left\">" << name << "</td>";
	row << "<td>=</td>";
#if 0
	row << "<td align=\"left\">" << str << "</td>";
#else
	row << "<td align=\"left\">" << value << str << "</td>";
#endif
	row << "</tr>\n";

	return row.str();
}

/**
 * dot_row (std::string)
 */
std::string
dot_row (const char* name, const std::string& value)
{
	std::stringstream row;

	row << "\t\t<tr>";
	row << "<td align=\"left\">" << name << "</td>";
	row << "<td>=</td>";
	row << "<td align=\"left\">" << value << "</td>";
	row << "</tr>\n";

	return row.str();
}

/**
 * dot_row (std::stringstream)
 */
std::string
dot_row (const char* name, const std::stringstream& value)
{
	std::stringstream row;

	row << "\t\t<tr>";
	row << "<td align=\"left\">" << name << "</td>";
	row << "<td>=</td>";
	row << "<td align=\"left\">" << value.str() << "</td>";
	row << "</tr>\n";

	return row.str();
}

/**
 * dot_row (char*)
 */
std::string
dot_row (const char* name, const char* value)
{
	std::stringstream row;

	row << "\t\t<tr>";
	row << "<td align=\"left\">" << name << "</td>";
	row << "<td>=</td>";
	row << "<td align=\"left\">" << value << "</td>";
	row << "</tr>\n";

	return row.str();
}

/**
 * dot_row (unsigned int)
 */
std::string
dot_row (const char* name, unsigned int value)
{
	std::stringstream row;

	row << "\t\t<tr>";
	row << "<td align=\"left\">" << name << "</td>";
	row << "<td>=</td>";
	row << "<td align=\"left\">" << value << "</td>";
	row << "</tr>\n";

	return row.str();
}

/**
 * dot_row (void*)
 */
std::string
dot_row (const char* name, void* value)
{
	std::stringstream row;

	row << "\t\t<tr>";
	row << "<td align=\"left\">" << name << "</td>";
	row << "<td>=</td>";
	if (value) {
		row << "<td align=\"left\">" << value << "</td>";
	} else {
		row << "<td align=\"left\">NULL</td>";
	}
	row << "</tr>\n";

	return row.str();
}

/**
 * dot_row (ContainerPtr)
 */
std::string
dot_row (const char* name, ContainerPtr value)
{
	std::stringstream row;
	std::string dest;

	if (value) {
		dest = " (" + value->type.back() + ")";
	}

	row << "\t\t<tr>";
	row << "<td align=\"left\">" << name << "</td>";
	row << "<td>=</td>";
	if (value) {
		row << "<td align=\"left\">" << (void*) value.get() << dest << "</td>";
	} else {
		row << "<td align=\"left\">NULL</td>";
	}
	row << "</tr>\n";

	return row.str();
}


template <class T>
std::string
dump_table (std::shared_ptr<T> t, const std::string& props)
{
	ContainerPtr c(t);
	std::stringstream table;

	if (c) {
		std::string colour = get_colour(c);
		std::string name   = c->name;
		if (name.empty()) {
			name = "UNKNOWN";
		}

		table << "obj_" << (void*) c.get() << " [fillcolor=\"" << colour << "\",label=<<table cellspacing=\"0\" border=\"0\">\n";
		table << "\t\t<tr><td align=\"left\" bgcolor=\"white\" colspan=\"3\"><font color=\"#000000\" point-size=\"20\"><b>" << name << "</b></font> (" << (void*) c.get() << ")<font color=\"#ff0000\" point-size=\"20\"><b> : " << c.use_count() << "</b></font></td></tr>\n";
		table << props;
		table << "\t</table>>];\n\n";
	}

	return table.str();
}

template <class T>
std::string
DotVisitor::parent_link (std::shared_ptr<T> t)
{
	std::stringstream link;

#if 1
	if (parents.size() > 0) {
		ContainerPtr c(t);
		link << "obj_" << (void*) parents.top().get() << " -> obj_" << (void*) c.get() << ";\n";
	}
#else
	ContainerPtr c(t);
	if (c) {
		ContainerPtr parent = c->parent.lock();
		if (parent) {
			link << "obj_" << (void*) parent.get() << " -> obj_" << (void*) c.get() << ";\n";
		}
	}
#endif

	return link.str();
}


template <class T>
std::string
dot_container (std::shared_ptr<T> t)
{
	ContainerPtr p(t);
	if (!p)
		return "";

	std::stringstream output;

	std::string uuid_short = p->uuid;

	if ((uuid_short.size() > 8) && (uuid_short[0] != '/')) {
		uuid_short = p->uuid.substr (0, 6) + "...";
	}

	output << dot_row ("type",          p->type.back());
	//output << dot_row ("name",          p->name);
	output << dot_row ("uuid",          uuid_short); //XXX temp
	if (p->device.empty()) {
		if (p->is_a ("Whole")) {
			output << dot_row ("device", "[segments]");
		} else {
			output << dot_row ("device", "[inherit]");
		}
	} else {
		output << dot_row ("device", p->device);
#if 1
		std::stringstream mm;
		mm << t->device_major << ":" << t->device_minor;
		output << dot_row ("device maj:min",	mm);
#else
		output << dot_row ("device_major",	p->device_major);
		output << dot_row ("device_minor",	p->device_minor);
#endif
	}

	output << dot_row ("fd",       p->fd);
	output << dot_row ("parent_offset", p->parent_offset);
	if (p->block_size) {
		output << dot_row ("block_size",    p->block_size);
	} else {
		output << dot_row ("block_size",    "[inherit]");
	}
	output << dot_row ("bytes_size",    p->bytes_size);
	output << dot_row ("bytes_used",    p->bytes_used);
	output << dot_row ("bytes_free",    p->bytes_size - p->bytes_used);
#if 0
	ContainerPtr cwhole = p->whole;
	output << dot_row ("whole",         cwhole);		//XXX what's this doing here?
#endif
	//output << dot_row ("parent",        p->parent);

	if (p->missing)
		output << dot_row ("missing", p->missing);

#if 0
	unsigned int count = p->children.size();
	if (count > 0) {
		output << dot_row ("children", count);
		for (auto i : p->children) {
			output << dot_row ("", i);
		}
	}
#endif

	return output.str();
}

template <class T>
std::string
dot_block (std::shared_ptr<T> t)
{
	BlockPtr p(t);
	if (!p)
		return "";

	std::stringstream output;

	output << dot_container(p);

	// no specifics for now

	return output.str();
}

template <class T>
std::string
dot_disk (std::shared_ptr<T> t)
{
	DiskPtr p(t);
	if (!p)
		return "";

	std::stringstream output;

	output << dot_block(p);

	//output << dot_row ("hw_cylinders",   hw_cylinders);
	//output << dot_row ("hw_heads",       hw_heads);
	//output << dot_row ("hw_sectors",     hw_sectors);
	//output << dot_row ("read_only",      read_only);
	//output << dot_row ("bios_cylinders", bios_cylinders);
	//output << dot_row ("bios_heads",     bios_heads);
	//output << dot_row ("bios_sectors",   bios_sectors);
	//output << dot_row ("host",           host);
	//output << dot_row ("did",            did);

	return output.str();
}

template <class T>
std::string
dot_file (std::shared_ptr<T> t)
{
	FilePtr p(t);
	if (!p)
		return "";

	std::stringstream output;

	output << dot_container(p);

	// no specifics for now

	return output.str();
}

template <class T>
std::string
dot_loop (std::shared_ptr<T> t)
{
	LoopPtr p(t);
	if (!p)
		return "";

	std::stringstream output;
	std::stringstream mm;

	output << dot_block(p);

	std::string flags;
	if (p->autoclear)	flags += ", autoclear";
	if (p->partscan)	flags += ", partscan";
	if (p->deleted)		flags += ", deleted";
	if (p->read_only)	flags += ", read only";
	if (flags.empty()) {
		flags = "n/a";
	} else {
		flags = flags.substr(2);
	}

	// Backing file
	output << dot_row ("file_name",		p->file_name);
#if 1
	mm << p->file_major << ":" << p->file_minor;
	output << dot_row ("file maj:min",	mm);
#else
	output << dot_row ("file_major",	p->file_major);
	output << dot_row ("file_minor",	p->file_minor);
#endif
	output << dot_row ("file_inode",	p->file_inode);

	// Loop device
	output << dot_row ("offset",		p->offset);
	output << dot_row ("sizelimit",		p->sizelimit);

	output << dot_row ("flags",		flags);
#if 0
	output << dot_row ("autoclear",		p->autoclear);
	output << dot_row ("partscan",		p->partscan);
	output << dot_row ("read_only",		p->read_only);
	output << dot_row ("deleted",		p->deleted);
#endif

	return output.str();
}

template <class T>
std::string
dot_table (std::shared_ptr<T> t)
{
	TablePtr p(t);
	if (!p)
		return "";

	std::stringstream output;

	output << dot_container(p);

	// no specifics for now

	return output.str();
}

template <class T>
std::string
dot_gpt (std::shared_ptr<T> t)
{
	GptPtr p(t);
	if (!p)
		return "";

	std::stringstream output;

	output << dot_table(p);

	// no specifics for now

	return output.str();
}

template <class T>
std::string
dot_msdos (std::shared_ptr<T> t)
{
	MsdosPtr p(t);
	if (!p)
		return "";

	std::stringstream output;

	output << dot_table(p);

	// no specifics for now

	return output.str();
}

template <class T>
std::string
dot_extended (std::shared_ptr<T> t)
{
	ExtendedPtr p(t);
	if (!p)
		return "";

	std::stringstream output;

	output << dot_msdos(p);

	// no specifics for now

	//XXX how? output << "{ rank=same obj_" << (void*) this << " obj_" << parent << " }\n";

	return output.str();
}

template <class T>
std::string
dot_lvm_table (std::shared_ptr<T> t)
{
	LvmTablePtr p(t);
	if (!p)
		return "";

	std::stringstream output;

	output << dot_table(p);

	output << dot_row ("metadata_size", p->metadata_size);
	output << dot_row ("pv_attr",       p->pv_attr);
	//output << dot_row ("config",        p->config);

	return output.str();
}

template <class T>
std::string
dot_md_table (std::shared_ptr<T> t)
{
	MdTablePtr p(t);
	if (!p)
		return "";

	std::stringstream output;

	output << dot_table(p);

	output << dot_row ("vol_uuid",    p->vol_uuid);
	output << dot_row ("vol_name",    p->vol_name);
	output << dot_row ("raid_type",   p->raid_type);
	output << dot_row ("raid_layout", p->raid_layout);
	output << dot_row ("raid_disks",  p->raid_disks);
	output << dot_row ("chunk_size",  p->chunk_size);
	output << dot_row ("chunks_used", p->chunks_used);
	output << dot_row ("data_offset", p->data_offset);
	output << dot_row ("data_size",   p->data_size);

	return output.str();
}

template <class T>
std::string
dot_partition (std::shared_ptr<T> t)
{
	PartitionPtr p(t);
	if (!p)
		return "";

	std::stringstream output;

	output << dot_container(p);

	// no specifics for now

	return output.str();
}

template <class T>
std::string
dot_msdos_partition (std::shared_ptr<T> t)
{
	MsdosPartitionPtr p(t);
	if (!p)
		return "";

	std::stringstream output;

	output << dot_partition(p);

	// no specifics for now

	return output.str();
}

template <class T>
std::string
dot_gpt_partition (std::shared_ptr<T> t)
{
	GptPartitionPtr p(t);
	if (!p)
		return "";

	std::stringstream output;

	output << dot_partition(p);

	// no specifics for now

	return output.str();
}

template <class T>
std::string
dot_lvm_partition (std::shared_ptr<T> t)
{
	LvmPartitionPtr p(t);
	if (!p)
		return "";

	std::stringstream output;

	output << dot_partition(p);

#if 0
	output << dot_row ("dev_size",		p->dev_size);
	output << dot_row ("pe_start",		p->pe_start);
	output << dot_row ("pv_attr",		p->pv_attr);
	output << dot_row ("pv_count",		p->pv_count);
	output << dot_row ("pv_free",		p->pv_free);
	output << dot_row ("pv_name",		p->pv_name);
	output << dot_row ("pv_pe_alloc",	p->pv_pe_alloc);
	output << dot_row ("pv_pe_count",	p->pv_pe_count);
	output << dot_row ("pv_size",		p->pv_size);
	output << dot_row ("pv_used",		p->pv_used);
	output << dot_row ("pv_uuid",		p->pv_uuid);

	output << dot_row ("pvseg_size",	p->pvseg_size);
	output << dot_row ("pvseg_start",	p->pvseg_start);

	output << dot_row ("lv_name",		p->lv_name);
	output << dot_row ("lv_type",		p->lv_type);
	output << dot_row ("lv_uuid",		p->lv_uuid);

	output << dot_row ("vg_extent",		p->vg_extent);
	output << dot_row ("vg_name",		p->vg_name);
	output << dot_row ("vg_seqno",		p->vg_seqno);
	output << dot_row ("vg_uuid",		p->vg_uuid);
#endif

	return output.str();
}

template <class T>
std::string
dot_md_partition (std::shared_ptr<T> t)
{
	MdPartitionPtr p(t);
	if (!p)
		return "";

	std::stringstream output;

	output << dot_partition(p);

	// no specifics for now

	return output.str();
}

template <class T>
std::string
dot_filesystem (std::shared_ptr<T> t)
{
	FilesystemPtr p(t);
	if (!p)
		return "";

	std::stringstream output;

	output << dot_container(p);

	// no specifics for now

	return output.str();
}

template <class T>
std::string
dot_ntfs (std::shared_ptr<T> t)
{
	NtfsPtr p(t);
	if (!p)
		return "";

	std::stringstream output;

	output << dot_filesystem(p);

	// no specifics for now

	return output.str();
}

template <class T>
std::string
dot_extfs (std::shared_ptr<T> t)
{
	ExtfsPtr p(t);
	if (!p)
		return "";

	std::stringstream output;

	output << dot_filesystem(p);

	// no specifics for now

	return output.str();
}

template <class T>
std::string
dot_btrfs (std::shared_ptr<T> t)
{
	BtrfsPtr p(t);
	if (!p)
		return "";

	std::stringstream output;

	output << dot_filesystem(p);

	// no specifics for now

	return output.str();
}

template <class T>
std::string
dot_misc (std::shared_ptr<T> t)
{
	MiscPtr p(t);
	if (!p)
		return "";

	std::stringstream output;

	output << dot_container(p);

	// no specifics for now

	return output.str();
}

template <class T>
std::string
dot_luks (std::shared_ptr<T> t)
{
	LuksTablePtr p(t);
	if (!p)
		return "";

	std::stringstream output;

	output << dot_container(p);

	output << dot_row ("version"    , p->version);
	output << dot_row ("cipher_name", p->cipher_name);
	output << dot_row ("cipher_mode", p->cipher_mode);
	output << dot_row ("hash_spec"  , p->hash_spec);

	return output.str();
}

template <class T>
std::string
dot_whole (std::shared_ptr<T> t)
{
	WholePtr p(t);
	if (!p)
		return "";

	std::stringstream output;
	unsigned int count = p->segments.size();

	output << dot_container(p);

	if (count > 0) {
		//std::cout << count << " segments" << std::endl;
		output << dot_row ("segments", count);
		for (auto i : p->segments) {
			output << dot_row ("", i);
		}
	}

	return output.str();
}

template <class T>
std::string
dot_lvm_group (std::shared_ptr<T> t)
{
	LvmGroupPtr p(t);
	if (!p)
		return "";

	std::stringstream output;

	output << dot_whole(p);

	output << dot_row ("pv_count", p->pv_count);
	output << dot_row ("lv_count", p->lv_count);
	output << dot_row ("vg_attr",  p->vg_attr);
	output << dot_row ("vg_seqno", p->vg_seqno);

	return output.str();
}

template <class T>
std::string
dot_volume (std::shared_ptr<T> t)
{
	VolumePtr p(t);
	if (!p)
		return "";

	std::stringstream output;

	output << dot_whole(p);

	// no specifics for now

	return output.str();
}

template <class T>
std::string
dot_lvm_volume (std::shared_ptr<T> t)
{
	LvmVolumePtr p = std::dynamic_pointer_cast<LvmVolume>(t);
	if (!p)
		return "";

	std::stringstream output;

	output << dot_volume(p);

	unsigned int count = p->metadata.size();
	if (count > 0) {
		output << dot_row ("metadata", count);
		for (auto i : p->metadata) {
			output << dot_row ("", i);
		}
	}

	count = p->subvols.size();
	if (count > 0) {
		output << dot_row ("subvols", count);
		for (auto i : p->subvols) {
			output << dot_row ("", i);
		}
	}

	output << dot_row ("sibling", p->sibling);

	return output.str();
}

template <class T>
std::string
dot_lvm_linear (std::shared_ptr<T> t)
{
	LvmLinearPtr p = std::dynamic_pointer_cast<LvmLinear>(t);
	if (!p)
		return "";

	std::stringstream output;

	output << dot_lvm_volume(p);

	// no specifics for now

	return output.str();
}

template <class T>
std::string
dot_lvm_mirror (std::shared_ptr<T> t)
{
	LvmMirrorPtr p(t);
	if (!p)
		return "";

	std::stringstream output;

	output << dot_lvm_volume(p);

	// no specifics for now

	return output.str();
}

template <class T>
std::string
dot_lvm_raid (std::shared_ptr<T> t)
{
	LvmRaidPtr p(t);
	if (!p)
		return "";

	std::stringstream output;

	output << dot_lvm_volume(p);

	// no specifics for now

	return output.str();
}

template <class T>
std::string
dot_lvm_stripe (std::shared_ptr<T> t)
{
	LvmStripePtr p(t);
	if (!p)
		return "";

	std::stringstream output;

	output << dot_lvm_volume(p);

	// no specifics for now

	return output.str();
}

template <class T>
std::string
dot_md_volume (std::shared_ptr<T> t)
{
	MdVolumePtr p(t);
	if (!p)
		return "";

	std::stringstream output;

	output << dot_volume(p);

	// no specifics for now

	return output.str();
}

template <class T>
std::string
dot_md_linear (std::shared_ptr<T> t)
{
	MdLinearPtr p(t);
	if (!p)
		return "";

	std::stringstream output;

	output << dot_md_volume(p);

	// no specifics for now

	return output.str();
}

template <class T>
std::string
dot_md_mirror (std::shared_ptr<T> t)
{
	MdMirrorPtr p(t);
	if (!p)
		return "";

	std::stringstream output;

	output << dot_md_volume(p);

	// no specifics for now

	return output.str();
}

template <class T>
std::string
dot_md_raid (std::shared_ptr<T> t)
{
	MdRaidPtr p(t);
	if (!p)
		return "";

	std::stringstream output;

	output << dot_md_volume(p);

	// no specifics for now

	return output.str();
}

template <class T>
std::string
dot_md_stripe (std::shared_ptr<T> t)
{
	MdStripePtr p(t);
	if (!p)
		return "";

	std::stringstream output;

	output << dot_md_volume(p);

	// no specifics for now

	return output.str();
}


/**
 * visit (ContainerPtr)
 */
bool
DotVisitor::visit (ContainerPtr c)
{
	//LOG_TRACE;
	dot_objects << dump_table (c, dot_container(c));
	dot_links   << parent_link(c);

	return true;
}

/**
 * visit (LoopPtr)
 */
bool
DotVisitor::visit (LoopPtr l)
{
	//LOG_TRACE;
	dot_objects << dump_table (l, dot_loop(l));
	dot_links   << parent_link(l);

	return true;
}

/**
 * visit (GptPtr)
 */
bool
DotVisitor::visit (GptPtr g)
{
	//LOG_TRACE;
	dot_objects << dump_table (g, dot_gpt(g));
	dot_links   << parent_link(g);

	return true;
}

/**
 * visit (MsdosPtr)
 */
bool
DotVisitor::visit (MsdosPtr m)
{
	//LOG_TRACE;
	dot_objects << dump_table (m, dot_msdos(m));
	dot_links   << parent_link(m);

	return true;
}

/**
 * visit (ExtendedPtr)
 */
bool
DotVisitor::visit (ExtendedPtr m)
{
	//LOG_TRACE;
	dot_objects << dump_table (m, dot_extended(m));

	// Custom parent pointer (same rank)
	if (parents.size() > 0) {
		ContainerPtr c(m);
		dot_links << "obj_"             << (void*) parents.top().get() << " -> obj_" << (void*) c.get() << ";\n";
		// dot_links << "{ rank=same obj_" << (void*) parents.top().get() <<    " obj_" << (void*) c.get() << "}\n";
	}

	return true;
}

/**
 * visit (MiscPtr)
 */
bool
DotVisitor::visit (MiscPtr m)
{
	//LOG_TRACE;
	dot_objects << dump_table (m, dot_misc(m));
	dot_links   << parent_link(m);

	return true;
}

/**
 * visit (PartitionPtr)
 */
bool
DotVisitor::visit (PartitionPtr p)
{
	//LOG_TRACE;
	dot_objects << dump_table (p, dot_partition(p));
	dot_links   << parent_link(p);

	return true;
}

/**
 * visit (FilesystemPtr)
 */
bool
DotVisitor::visit (FilesystemPtr f)
{
	//LOG_TRACE;
	dot_objects << dump_table (f, dot_filesystem(f));
	dot_links   << parent_link(f);

	return true;
}


/**
 * visit (WholePtr)
 */
bool
DotVisitor::visit (WholePtr f)
{
	//LOG_TRACE;
	dot_objects << dump_table (f, dot_whole(f));
	dot_links   << parent_link(f);

	for (auto i : f->segments) {
		dot_objects << dump_table (i, dot_container(i));
		dot_links   << parent_link(i);
	}

	return true;
}

/**
 * visit (LvmVolumePtr)
 */
bool
DotVisitor::visit (LvmVolumePtr f)
{
	//LOG_TRACE;
	dot_objects << dump_table (f, dot_lvm_volume(f));
	dot_links   << parent_link(f);

	return true;
}

/**
 * visit (LvmRaidPtr)
 */
bool
DotVisitor::visit (LvmRaidPtr f)
{
	//LOG_TRACE;
	dot_objects << dump_table (f, dot_lvm_raid(f));
	dot_links   << parent_link(f);

	return true;
}

/**
 * visit (LvmGroupPtr)
 */
bool
DotVisitor::visit (LvmGroupPtr f)
{
	//LOG_TRACE;
	dot_objects << dump_table (f, dot_lvm_group(f));
	dot_links   << parent_link(f);

	return true;
}

/**
 * visit (LvmLinearPtr)
 */
bool
DotVisitor::visit (LvmLinearPtr f)
{
	//LOG_TRACE;
	dot_objects << dump_table (f, dot_lvm_linear(f));
	dot_links   << parent_link(f);

	//XXX this needs to be at dot_lvm_volume level
	if ((f->sibling) && (f->is_a ("LvmMetadata"))) {	// Link sibling, but only in one direction
		ContainerPtr c1(f);
		ContainerPtr c2 (f->sibling);

		// We link them invisibly to keep them together
		dot_links << "obj_" << (void*) c1.get() << " -> obj_" << (void*) c2.get() << " [ style=invis ];\n";
		dot_links << "{ rank=same obj_" << (void*) c1.get() << " obj_" << (void*) c2.get() << "}\n";
	}

	return true;
}


std::string
DotVisitor::get_dot (void)
{
	std::string str;

	str  = "digraph disks {\n";
	str += "graph [ rankdir=\"TB\", color=\"white\",bgcolor=\"#000000\" ];\n";
	str += "node [ shape=\"record\", color=\"black\", fillcolor=\"lightcyan\", style=\"filled\" ];\n";
	str += "edge [ penwidth=3.0,color=\"#cccccc\" ];\n";
	str += "\n";

	str += dot_objects.str();
	str += "\n";
	str += dot_links.str();

	str += "\n}\n";

	return str;
}

void
DotVisitor::run_dotty (void)
{
	char timebuf[8];
	time_t rawtime;
	struct tm* timeinfo;

	time (&rawtime);
	timeinfo = localtime (&rawtime);
	strftime (timebuf, sizeof (timebuf), "%H:%M", timeinfo);

	std::string now (timebuf);

	if (!save_gv && !save_png)
		display = true;

	std::string input = get_dot();
	std::string nothing;
	std::string dir;

	if (display) {
		std::string title = "DParted: " + std::string (timebuf);
		std::string size;
		if (resize != -1) {
			size = " -resize " + std::to_string (resize) + "%";
		}
		std::string command = "dot -Tpng | display -title \"" + title + "\"" + size + " - &";
		execute_command2 (command, input);
	}

	if (save_gv) {
		dir = "gv_" + now;
		execute_command2 ("mkdir -p " + dir, nothing);
		std::string command = "cat > " + dir + "/$RANDOM.gv";
		execute_command2 (command, input);
	}

	if (save_png) {
		dir = "png_" + now;
		execute_command2 ("mkdir -p " + dir, nothing);
		std::string command = "dot -Tpng > " + dir + "/$RANDOM.png";
		execute_command2 (command, input);
	}
}

// --dot={display|dump|save_gv|save_png}
// --dot-resize=PERCENTAGE

