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

#include "dot_visitor.h"

#include "container.h"
#include "loop.h"
#include "gpt.h"
#include "whole.h"
#include "device.h"
#include "partition.h"
#include "filesystem.h"
#include "luks.h"
#include "lvm_group.h"
#include "lvm_volume.h"

#include "log_trace.h"
#include "utils.h"

/**
 * DotVisitor
 */
DotVisitor::DotVisitor (void)
{
	output << "digraph disks {\n";
	output << "graph [ rankdir=\"TB\", color=\"white\",bgcolor=\"#000000\" ];\n";
	output << "node [ shape=\"record\", color=\"black\", fillcolor=\"lightcyan\", style=\"filled\" ];\n";
	output << "edge [ penwidth=3.0,color=\"#cccccc\" ];\n";
	output << "\n";
}

/**
 * ~DotVisitor
 */
DotVisitor::~DotVisitor()
{
}


/**
 * visit_enter
 */
bool
DotVisitor::visit_enter (ContainerPtr& c)
{
	std::stringstream addr;
	addr << (void*) c.get();
	parents.push (addr.str());
	return true;
}

/**
 * visit_leave
 */
bool
DotVisitor::visit_leave (void)
{
	parents.pop();
#if 0
	if (parents.empty()) {
		output << "\n}";
		output << "\n";
	}
#endif

	return true;
}


/**
 * get_colour
 */
std::string
get_colour (ContainerPtr c)
{
	if (c->is_a ("Container"))  return "#FFFFFF";
	if (c->is_a ("Table"))      return "#FFFFCC";
	if (c->is_a ("Device"))     return "#FFB6C1";
	if (c->is_a ("Whole"))      return "#2BDCFF";
	if (c->is_a ("Filesystem")) return "#B4A0F3";
	if (c->is_a ("Piece"))      return "#B0D0B0";
	if (c->is_a ("File"))       return "#00FF00";
	if (c->is_a ("Misc"))       return "#DC4C46";

	return "white";
}


/**
 * dot_row (bool)
 */
std::string
dot_row (const char* name, bool value)
{
	std::stringstream output;

	output << "<tr>";
	output << "<td align=\"left\">" << name << "</td>";
	output << "<td>=</td>";
	output << "<td align=\"left\">" << (value ? "true" : "false") << "</td>";
	output << "</tr>\n";

	return output.str();
}

/**
 * dot_row (int)
 */
std::string
dot_row (const char* name, int value)
{
	std::stringstream output;

	output << "<tr>";
	output << "<td align=\"left\">" << name << "</td>";
	output << "<td>=</td>";
	output << "<td align=\"left\">" << value << "</td>";
	output << "</tr>\n";

	return output.str();
}

/**
 * dot_row (long)
 */
std::string
dot_row (const char* name, long value)
{
	std::stringstream output;
	std::string str;

	if (value > 1023) {
		str = " (" + get_size (value) + ")";
	}

	output << "<tr>";
	output << "<td align=\"left\">" << name << "</td>";
	output << "<td>=</td>";
#if 0
	output << "<td align=\"left\">" << str << "</td>";
#else
	output << "<td align=\"left\">" << value << str << "</td>";
#endif
	output << "</tr>\n";

	return output.str();
}

/**
 * dot_row (std::string)
 */
std::string
dot_row (const char* name, const std::string& value)
{
	std::stringstream output;

	output << "<tr>";
	output << "<td align=\"left\">" << name << "</td>";
	output << "<td>=</td>";
	output << "<td align=\"left\">" << value << "</td>";
	output << "</tr>\n";

	return output.str();
}

/**
 * dot_row (std::stringstream)
 */
std::string
dot_row (const char* name, const std::stringstream& value)
{
	std::stringstream output;

	output << "<tr>";
	output << "<td align=\"left\">" << name << "</td>";
	output << "<td>=</td>";
	output << "<td align=\"left\">" << value.str() << "</td>";
	output << "</tr>\n";

	return output.str();
}

/**
 * dot_row (char*)
 */
std::string
dot_row (const char* name, const char* value)
{
	std::stringstream output;

	output << "<tr>";
	output << "<td align=\"left\">" << name << "</td>";
	output << "<td>=</td>";
	output << "<td align=\"left\">" << value << "</td>";
	output << "</tr>\n";

	return output.str();
}

/**
 * dot_row (unsigned int)
 */
std::string
dot_row (const char* name, unsigned int value)
{
	std::stringstream output;

	output << "<tr>";
	output << "<td align=\"left\">" << name << "</td>";
	output << "<td>=</td>";
	output << "<td align=\"left\">" << value << "</td>";
	output << "</tr>\n";

	return output.str();
}

/**
 * dot_row (void*)
 */
std::string
dot_row (const char* name, void* value)
{
	std::stringstream output;

	output << "<tr>";
	output << "<td align=\"left\">" << name << "</td>";
	output << "<td>=</td>";
	if (value) {
		output << "<td align=\"left\">" << value << "</td>";
	} else {
		output << "<td align=\"left\">NULL</td>";
	}
	output << "</tr>\n";

	return output.str();
}

/**
 * dot_row (ContainerPtr)
 */
std::string
dot_row (const char* name, ContainerPtr value)
{
	std::stringstream output;
	std::string dest;

	if (value) {
		dest = " (" + value->type.back() + ")";
	}

	output << "<tr>";
	output << "<td align=\"left\">" << name << "</td>";
	output << "<td>=</td>";
	if (value) {
		output << "<td align=\"left\">" << (void*) value.get() << dest << "</td>";
	} else {
		output << "<td align=\"left\">NULL</td>";
	}
	output << "</tr>\n";

	return output.str();
}


/**
 * dot_container
 */
template <class T>
std::string
dot_container (std::shared_ptr<T> t)
{
	ContainerPtr c(t);
	if (!c)
		return "";

	std::stringstream output;

	std::string uuid_short = c->uuid;

	if ((uuid_short.size() > 8) && (uuid_short[0] != '/')) {
		uuid_short = c->uuid.substr (0, 6) + "...";
	}

	output << dot_row ("type",          c->type.back());
	//output << dot_row ("name",          c->name);
	output << dot_row ("uuid",          uuid_short); //XXX temp
	if (c->device.empty()) {
		if (c->is_a ("Whole")) {
			output << dot_row ("device", "[segments]");
		} else {
			output << dot_row ("device", "[inherit]");
		}
	} else {
		output << dot_row ("device", c->device);
	}

	output << dot_row ("fd",       c->fd);
	output << dot_row ("parent_offset", c->parent_offset);
	if (c->block_size) {
		output << dot_row ("block_size",    c->block_size);
	} else {
		output << dot_row ("block_size",    "[inherit]");
	}
	output << dot_row ("bytes_size",    c->bytes_size);
	output << dot_row ("bytes_used",    c->bytes_used);
	output << dot_row ("bytes_free",    c->bytes_size - c->bytes_used);
#if 0
	ContainerPtr cwhole = c->whole;
	output << dot_row ("whole",         cwhole);		//XXX what's this doing here?
#endif
	//output << dot_row ("parent",        c->parent);

	if (c->missing)
		output << dot_row ("missing", c->missing);

#if 0
	unsigned int count = c->children.size();
	if (count > 0) {
		output << dot_row ("children", count);
		for (auto i : c->children) {
			output << dot_row ("", i);
		}
	}
#endif

	return output.str();
}

/**
 * dot_device
 */
template <class T>
std::string
dot_device (std::shared_ptr<T> t)
{
	DevicePtr b(t);
	if (!b)
		return "";

	std::stringstream output;

	output << dot_container(b);

	// no specifics for now

	return output.str();
}

/**
 * dot_disk
 */
template <class T>
std::string
dot_disk (std::shared_ptr<T> t)
{
	DiskPtr d(t);
	if (!d)
		return "";

	std::stringstream output;

	output << dot_device(d);

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

/**
 * dot_file
 */
template <class T>
std::string
dot_file (std::shared_ptr<T> t)
{
	FilePtr f(t);
	if (!f)
		return "";

	std::stringstream output;

	// no specifics for now

	output << dot_container(f);

	return output.str();
}

/**
 * dot_loop
 */
template <class T>
std::string
dot_loop (std::shared_ptr<T> t)
{
	LoopPtr l(t);
	if (!l)
		return "";

	std::stringstream output;
	std::stringstream mm;

	output << dot_device(l);

	std::string flags;
	if (l->autoclear)	flags += ", autoclear";
	if (l->partscan)	flags += ", partscan";
	if (l->deleted)		flags += ", deleted";
	if (l->read_only)	flags += ", read only";
	if (flags.empty()) {
		flags = "n/a";
	} else {
		flags = flags.substr(2);
	}

	// Backing file
	output << dot_row ("file_name",		l->file_name);
#if 1
	mm << l->file_major << ":" << l->file_minor;
	output << dot_row ("file maj:min",	mm);
#else
	output << dot_row ("file_major",	l->file_major);
	output << dot_row ("file_minor",	l->file_minor);
#endif
	output << dot_row ("file_inode",	l->file_inode);

	// Loop device
#if 1
	mm.str ("");
	mm << l->loop_major << ":" << l->loop_minor;
	output << dot_row ("loop maj:min",	mm);
#else
	output << dot_row ("loop_major",	l->loop_major);
	output << dot_row ("loop_minor",	l->loop_minor);
#endif
	output << dot_row ("offset",		l->offset);
	output << dot_row ("sizelimit",		l->sizelimit);

	output << dot_row ("flags",		flags);
#if 0
	output << dot_row ("autoclear",		l->autoclear);
	output << dot_row ("partscan",		l->partscan);
	output << dot_row ("read_only",		l->read_only);
	output << dot_row ("deleted",		l->deleted);
#endif

	return output.str();
}

/**
 * dot_table
 */
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

/**
 * dot_gpt
 */
template <class T>
std::string
dot_gpt (std::shared_ptr<T> t)
{
	GptPtr g(t);
	if (!g)
		return "";

	std::stringstream output;

	output << dot_table(g);

	// no specifics for now

	return output.str();
}

/**
 * dot_lvm_table
 */
template <class T>
std::string
dot_lvm_table (std::shared_ptr<T> t)
{
	LvmTablePtr l(t);
	if (!l)
		return "";

	std::stringstream output;

	output << dot_table (std::dynamic_pointer_cast<Table>(t));

	output << dot_row ("metadata_size", t->metadata_size);
	output << dot_row ("pv_attr",       t->pv_attr);
	//output << dot_row ("config",   t->config);

	return output.str();
}

/**
 * dot_md_table
 */
template <class T>
std::string
dot_md_table (std::shared_ptr<T> t)
{
	MdTablePtr m(t);
	if (!m)
		return "";

	std::stringstream output;

	output << dot_table (std::dynamic_pointer_cast<Table>(t));

	output << dot_row ("vol_uuid",    t->vol_uuid);
	output << dot_row ("vol_name",    t->vol_name);
	output << dot_row ("raid_type",   t->raid_type);
	output << dot_row ("raid_layout", t->raid_layout);
	output << dot_row ("raid_disks",  t->raid_disks);
	output << dot_row ("chunk_size",  t->chunk_size);
	output << dot_row ("chunks_used", t->chunks_used);
	output << dot_row ("data_offset", t->data_offset);
	output << dot_row ("data_size",   t->data_size);

	return output.str();
}

/**
 * dot_msdos
 */
template <class T>
std::string
dot_msdos (std::shared_ptr<T> t)
{
	MsdosPtr m(t);
	if (!m)
		return "";

	std::stringstream output;

	output << dot_table(m);

	// no specifics for now

	return output.str();
}

/**
 * dot_extended
 */
template <class T>
std::string
dot_extended (std::shared_ptr<T> t)
{
	ExtendedPtr e(t);
	if (!e)
		return "";

	std::stringstream output;

	output << dot_msdos(e);
	output << dot_device(e);	//XXX argh!

	// no specifics for now

	//XXX how? output << "{ rank=same obj_" << (void*) this << " obj_" << parent << " }\n";

	return output.str();
}

/**
 * dot_partition
 */
template <class T>
std::string
dot_partition (std::shared_ptr<T> t)
{
	PartitionPtr p(t);
	if (!p)
		return "";

	std::stringstream output;

	output << dot_device(p);

	// no specifics for now

	return output.str();
}

/**
 * dot_luks
 */
template <class T>
std::string
dot_luks (std::shared_ptr<T> t)
{
	LuksPtr l(t);
	if (!l)
		return "";

	std::stringstream output;

	output << dot_device(l);

	output << dot_row ("version"    , l->version);
	output << dot_row ("cipher_name", l->cipher_name);
	output << dot_row ("cipher_mode", l->cipher_mode);
	output << dot_row ("hash_spec"  , l->hash_spec);

	return output.str();
}

/**
 * dot_filesystem
 */
template <class T>
std::string
dot_filesystem (std::shared_ptr<T> t)
{
	FilesystemPtr f(t);
	if (!f)
		return "";

	std::stringstream output;

	output << dot_container(f);

	return output.str();
}

/**
 * dot_whole
 */
template <class T>
std::string
dot_whole (std::shared_ptr<T> t)
{
	WholePtr w(t);
	if (!w)
		return "";

	std::stringstream output;
	unsigned int count = w->segments.size();

	output << dot_container (std::dynamic_pointer_cast<Container>(w));

	if (count > 0) {
		output << dot_row ("segments", count);
		for (auto i : w->segments) {
			output << dot_row ("", i);
		}
	}

	return output.str();
}

/**
 * dot_volume
 */
template <class T>
std::string
dot_volume (std::shared_ptr<T> t)
{
	VolumePtr v(t);
	if (!v)
		return "";

	std::stringstream output;

	output << dot_whole(v);
	output << dot_device(v);	//XXX argh

	// no specifics for now

	return output.str();
}

/**
 * dot_md_group
 */
template <class T>
std::string
dot_md_group (std::shared_ptr<T> t)
{
	MdGroupPtr m(t);
	if (!m)
		return "";

	std::stringstream output;

	output << dot_whole(m);

	// no specifics for now

	return output.str();
}

/**
 * dot_misc
 */
template <class T>
std::string
dot_misc (std::shared_ptr<T> t)
{
	MiscPtr m(t);
	if (!m)
		return "";

	std::stringstream output;

	output << dot_container(m);

	// no specifics for now

	return output.str();
}

/**
 * dot_lvm_group
 */
template <class T>
std::string
dot_lvm_group (std::shared_ptr<T> t)
{
	LvmGroupPtr l(t);
	if (!l)
		return "";

	std::stringstream output;

	output << dot_whole(l);

	output << dot_row ("pv_count", l->pv_count);
	output << dot_row ("lv_count", l->lv_count);
	output << dot_row ("vg_attr",  l->vg_attr);
	output << dot_row ("vg_seqno", l->vg_seqno);

	return output.str();
}

/**
 * dot_lvm_volume
 */
template <class T>
std::string
dot_lvm_volume (std::shared_ptr<T> t)
{
	LvmVolumePtr l(t);
	if (!l)
		return "";

	std::stringstream output;

	output << dot_volume(l);

	unsigned int count = l->metadata.size();
	if (count > 0) {
		output << dot_row ("metadata", count);
		for (auto i : l->metadata) {
			output << dot_row ("", i);
		}
	}

	count = l->subvols.size();
	if (count > 0) {
		output << dot_row ("subvols", count);
		for (auto i : l->subvols) {
			output << dot_row ("", i);
		}
	}

	output << dot_row ("sibling", l->sibling);

	return output.str();
}

/**
 * dot_lvm_linear
 */
template <class T>
std::string
dot_lvm_linear (std::shared_ptr<T> t)
{
	LvmLinearPtr l(t);
	if (!l)
		return "";

	std::stringstream output;

	output << dot_lvm_volume(l);

	// no specifics for now

	return output.str();
}

/**
 * dot_lvm_metadata
 */
template <class T>
std::string
dot_lvm_metadata (std::shared_ptr<T> t)
{
	LvmMetadataPtr l(t);
	if (!l)
		return "";

	std::stringstream output;

	output << dot_lvm_linear(l);

	// no specifics for now

	return output.str();
}

/**
 * dot_lvm_mirror
 */
template <class T>
std::string
dot_lvm_mirror (std::shared_ptr<T> t)
{
	LvmMirrorPtr l(t);
	if (!l)
		return "";

	std::stringstream output;

	output << dot_lvm_volume(l);

	// no specifics for now

	return output.str();
}

/**
 * dot_lvm_raid
 */
template <class T>
std::string
dot_lvm_raid (std::shared_ptr<T> t)
{
	LvmRaidPtr l(t);
	if (!l)
		return "";

	std::stringstream output;

	output << dot_lvm_volume(l);

	// no specifics for now

	return output.str();
}

/**
 * dot_lvm_stripe
 */
template <class T>
std::string
dot_lvm_stripe (std::shared_ptr<T> t)
{
	LvmStripePtr l(t);
	if (!l)
		return "";

	std::stringstream output;

	output << dot_lvm_volume(l);

	// no specifics for now

	return output.str();
}

/**
 * dot_btrfs
 */
template <class T>
std::string
dot_btrfs  (std::shared_ptr<T> t)
{
	BtrfsPtr b(t);
	if (!b)
		return "";

	std::stringstream output;

	output << dot_filesystem(b);

	// no specifics for now

	return output.str();
}

/**
 * dot_extfs
 */
template <class T>
std::string
dot_extfs  (std::shared_ptr<T> t)
{
	NtfsPtr e(t);
	if (!e)
		return "";

	std::stringstream output;

	output << dot_filesystem(e);

	// no specifics for now

	return output.str();
}

/**
 * dot_ntfs
 */
template <class T>
std::string
dot_ntfs  (std::shared_ptr<T> t)
{
	NtfsPtr n(t);
	if (!n)
		return "";

	std::stringstream output;

	output << dot_filesystem(n);

	// no specifics for now

	return output.str();
}

/**
 * dot_piece
 */
template <class T>
std::string
dot_piece  (std::shared_ptr<T> t)
{
	PiecePtr p(t);
	if (!p)
		return "";

	std::stringstream output;

	output << dot_container(p);

	// no specifics for now

	return output.str();
}

/**
 * dot_md_partition
 */
template <class T>
std::string
dot_md_partition  (std::shared_ptr<T> t)
{
	MdPartitionPtr m(t);
	if (!m)
		return "";

	std::stringstream output;

	output << dot_piece(m);

	// no specifics for now

	return output.str();
}

/**
 * dot_lvm_partition
 */
template <class T>
std::string
dot_lvm_partition (std::shared_ptr<T> t)
{
	LvmPartitionPtr l(t);
	if (!l)
		return "";

	std::stringstream output;

	output << dot_piece(l);

#if 0
	output << dot_row ("dev_size",		l->dev_size);
	output << dot_row ("pe_start",		l->pe_start);
	output << dot_row ("pv_attr",		l->pv_attr);
	output << dot_row ("pv_count",		l->pv_count);
	output << dot_row ("pv_free",		l->pv_free);
	output << dot_row ("pv_name",		l->pv_name);
	output << dot_row ("pv_pe_alloc",	l->pv_pe_alloc);
	output << dot_row ("pv_pe_count",	l->pv_pe_count);
	output << dot_row ("pv_size",		l->pv_size);
	output << dot_row ("pv_used",		l->pv_used);
	output << dot_row ("pv_uuid",		l->pv_uuid);

	output << dot_row ("pvseg_size",	l->pvseg_size);
	output << dot_row ("pvseg_start",	l->pvseg_start);

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


/**
 * visit (ContainerPtr)
 */
bool
DotVisitor::visit (ContainerPtr c)
{
	LOG_TRACE;
	output << "\n";
	output << "// " << (void*) c.get() << "\n";

	std::string name = c->name;
	if (name.empty()) {
		name = "UNKNOWN";
	}

	output << "obj_" << (void*) c.get() << " [fillcolor=\"#c0c0c0\",label=<<table cellspacing=\"0\" border=\"0\">\n";
	output << "<tr><td align=\"left\" bgcolor=\"white\" colspan=\"3\"><font color=\"#000000\" point-size=\"20\"><b>" << name << "</b></font> (" << (void*) c.get() << ")<font color=\"#ff0000\" point-size=\"20\"><b> : " << c.use_count() << "</b></font></td></tr>\n";

	output << dot_container(c);

	output << "</table>>];\n";

#if 1
	if (parents.size() > 0)
		output << "obj_" << parents.top() << " -> obj_" << (void*) c.get() << ";\n";
#endif

	return true;
}

/**
 * visit (LoopPtr)
 */
bool
DotVisitor::visit (LoopPtr l)
{
	LOG_TRACE;
	output << "\n";
	output << "// " << (void*) l.get() << "\n";

	std::string name = l->name;
	if (name.empty()) {
		name = "UNKNOWN";
	}

	output << "obj_" << (void*) l.get() << " [fillcolor=\"#ffc0c0\",label=<<table cellspacing=\"0\" border=\"0\">\n";
	output << "<tr><td align=\"left\" bgcolor=\"white\" colspan=\"3\"><font color=\"#000000\" point-size=\"20\"><b>" << name << "</b></font> (" << (void*) l.get() << ")<font color=\"#ff0000\" point-size=\"20\"><b> : " << l.use_count() << "</b></font></td></tr>\n";

	std::stringstream mm;

	output << dot_device (std::dynamic_pointer_cast<Device>(l));

	std::string flags;
	if (l->autoclear)	flags += ", autoclear";
	if (l->partscan)	flags += ", partscan";
	if (l->deleted)		flags += ", deleted";
	if (l->read_only)	flags += ", read only";
	if (flags.empty()) {
		flags = "n/a";
	} else {
		flags = flags.substr(2);
	}

	// Backing file
	output << dot_row ("file_name",		l->file_name);
#if 1
	mm << l->file_major << ":" << l->file_minor;
	output << dot_row ("file maj:min",	mm);
#else
	output << dot_row ("file_major",	l->file_major);
	output << dot_row ("file_minor",	l->file_minor);
#endif
	output << dot_row ("file_inode",	l->file_inode);

	// Loop device
#if 1
	mm.str ("");
	mm << l->loop_major << ":" << l->loop_minor;
	output << dot_row ("loop maj:min",	mm);
#else
	output << dot_row ("loop_major",	l->loop_major);
	output << dot_row ("loop_minor",	l->loop_minor);
#endif
	output << dot_row ("offset",		l->offset);
	output << dot_row ("sizelimit",		l->sizelimit);

	output << dot_row ("flags",		flags);
#if 0
	output << dot_row ("autoclear",		l->autoclear);
	output << dot_row ("partscan",		l->partscan);
	output << dot_row ("read_only",		l->read_only);
	output << dot_row ("deleted",		l->deleted);
#endif

	output << "</table>>];\n";

#if 1
	if (parents.size() > 0)
		output << "obj_" << parents.top() << " -> obj_" << (void*) l.get() << ";\n";
#endif

	return true;
}

/**
 * visit (GptPtr)
 */
bool
DotVisitor::visit (GptPtr g)
{
	LOG_TRACE;
	output << "\n";
	output << "// " << (void*) g.get() << "\n";

	std::string name = g->name;
	if (name.empty()) {
		name = "UNKNOWN";
	}

	output << "obj_" << (void*) g.get() << " [fillcolor=\"#ffc0c0\",label=<<table cellspacing=\"0\" border=\"0\">\n";
	output << "<tr><td align=\"left\" bgcolor=\"white\" colspan=\"3\"><font color=\"#000000\" point-size=\"20\"><b>" << name << "</b></font> (" << (void*) g.get() << ")<font color=\"#ff0000\" point-size=\"20\"><b> : " << g.use_count() << "</b></font></td></tr>\n";

	output << dot_gpt(g);

	output << "</table>>];\n";

#if 1
	if (parents.size() > 0)
		output << "obj_" << parents.top() << " -> obj_" << (void*) g.get() << ";\n";
#endif

	return true;
}

/**
 * visit (MsdosPtr)
 */
bool
DotVisitor::visit (MsdosPtr m)
{
	LOG_TRACE;
	output << "\n";
	output << "// " << (void*) m.get() << "\n";

	std::string name = m->name;
	if (name.empty()) {
		name = "UNKNOWN";
	}

	output << "obj_" << (void*) m.get() << " [fillcolor=\"#ffc0c0\",label=<<table cellspacing=\"0\" border=\"0\">\n";
	output << "<tr><td align=\"left\" bgcolor=\"white\" colspan=\"3\"><font color=\"#000000\" point-size=\"20\"><b>" << name << "</b></font> (" << (void*) m.get() << ")<font color=\"#ff0000\" point-size=\"20\"><b> : " << m.use_count() << "</b></font></td></tr>\n";

	output << dot_msdos(m);

	output << "</table>>];\n";

#if 1
	if (parents.size() > 0)
		output << "obj_" << parents.top() << " -> obj_" << (void*) m.get() << ";\n";
#endif

	return true;
}

/**
 * visit (MiscPtr)
 */
bool
DotVisitor::visit (MiscPtr m)
{
	LOG_TRACE;
	output << "\n";
	output << "// " << (void*) m.get() << "\n";

	std::string name = m->name;
	if (name.empty()) {
		name = "UNKNOWN";
	}

	output << "obj_" << (void*) m.get() << " [fillcolor=\"#ffc0c0\",label=<<table cellspacing=\"0\" border=\"0\">\n";
	output << "<tr><td align=\"left\" bgcolor=\"white\" colspan=\"3\"><font color=\"#000000\" point-size=\"20\"><b>" << name << "</b></font> (" << (void*) m.get() << ")<font color=\"#ff0000\" point-size=\"20\"><b> : " << m.use_count() << "</b></font></td></tr>\n";

	output << dot_misc(m);

	output << "</table>>];\n";

#if 1
	if (parents.size() > 0)
		output << "obj_" << parents.top() << " -> obj_" << (void*) m.get() << ";\n";
#endif

	return true;
}

/**
 * visit (PartitionPtr)
 */
bool
DotVisitor::visit (PartitionPtr p)
{
	LOG_TRACE;
	output << "\n";
	output << "// " << (void*) p.get() << "\n";

	std::string name = p->name;
	if (name.empty()) {
		name = "UNKNOWN";
	}

	output << "obj_" << (void*) p.get() << " [fillcolor=\"#d0d080\",label=<<table cellspacing=\"0\" border=\"0\">\n";
	output << "<tr><td align=\"left\" bgcolor=\"white\" colspan=\"3\"><font color=\"#000000\" point-size=\"20\"><b>" << name << "</b></font> (" << (void*) p.get() << ")<font color=\"#ff0000\" point-size=\"20\"><b> : " << p.use_count() << "</b></font></td></tr>\n";

	output << dot_partition(p);

	output << "</table>>];\n";

#if 1
	if (parents.size() > 0)
		output << "obj_" << parents.top() << " -> obj_" << (void*) p.get() << ";\n";
#endif

	return true;
}

/**
 * visit (FilesystemPtr)
 */
bool
DotVisitor::visit (FilesystemPtr f)
{
	LOG_TRACE;
	output << "\n";
	output << "// " << (void*) f.get() << "\n";

	std::string name = f->name;
	if (name.empty()) {
		name = "UNKNOWN";
	}

	output << "obj_" << (void*) f.get() << " [fillcolor=\"#80c080\",label=<<table cellspacing=\"0\" border=\"0\">\n";
	output << "<tr><td align=\"left\" bgcolor=\"white\" colspan=\"3\"><font color=\"#000000\" point-size=\"20\"><b>" << name << "</b></font> (" << (void*) f.get() << ")<font color=\"#ff0000\" point-size=\"20\"><b> : " << f.use_count() << "</b></font></td></tr>\n";

	output << dot_filesystem(f);

	output << "</table>>];\n";

#if 1
	if (parents.size() > 0)
		output << "obj_" << parents.top() << " -> obj_" << (void*) f.get() << ";\n";
#endif

	return true;
}


#if 0
/**
 * dump_dot_inner
 */
static std::string
dump_dot_inner (const std::vector <ContainerPtr>& v)
{
	std::stringstream dot;
	int count = 0;

	for (auto c : v) {
		std::string type = c->type.back();
		// Isolate the top-level objects
		if (c->parent && (c->parent->parent == nullptr))
			dot << "subgraph cluster_" << count++ << " { color=transparent;\n";

		dot << "\n";
		dot << "// " << c << "\n";

		if (c->name.empty()) {
			c->name = "UNKNOWN";
		}

		std::string colour = get_colour(c);
		std::string missing;
		if (c->missing) {
			missing = " MISSING";
		}

		dot << "obj_" << (void*) c.get() << " [fillcolor=\"" << colour << "\",label=<<table cellspacing=\"0\" border=\"0\">\n";
		dot << "<tr><td align=\"left\" bgcolor=\"white\" colspan=\"3\"><font color=\"#000000\" point-size=\"20\"><b>" << c->name << "</b></font> (" << (void*) c.get() << ")<font color=\"#ff0000\" point-size=\"20\"><b> : " << c.use_count() << missing << "</b></font></td></tr>\n";

		if (type == "block")         { dot << dot_device        (c); }
		//...

		dot << "</table>>];\n";

#if 0
		if (c->parent) {
			dot << "obj_" << (void*) c << " -> obj_" << (void*) c->parent << " [constraint=false color=red];\n";
		}
#endif

		for (auto c2 : c->get_children()) {
			dot << "obj_" << (void*) c.get() << " -> obj_" << (void*) c2.get() << ";\n";
		}

#if 0
		if (c->is_a ("Whole")) {
			WholePtr w = std::dynamic_pointer_cast<Whole>(c);
			if (w) {
				for (auto w2 : w->segments) {
					dot << "obj_" << (void*) w << " -> obj_" << (void*) w2 << " [constraint=false style=dashed];\n";
				}
			}
		}
#endif

#if 1
		if (c->is_a ("LvmMetadata")) {
			LvmVolumePtr m = std::dynamic_pointer_cast<LvmVolume>(c);
			if (m && m->sibling) {
				dot << "obj_" << (void*) m->sibling.get() << " -> obj_" << (void*) m.get() << " [constraint=false style=dashed dir=none];\n";
			}
		}
#endif

#if 1
		if (c->is_a ("LvmVolume")) {
			LvmVolumePtr v = std::dynamic_pointer_cast<LvmVolume>(c);
			if (v) {
				//log_info ("VOLUME %s, %ld\n", v->type.back().c_str(), v->subvols.size());
				dot << dump_dot_inner (v->metadata);

				for (auto v2 : v->metadata) {
					dot << "obj_" << (void*) v.get() << " -> obj_" << (void*) v2.get() << ";\n";
				}

				dot << dump_dot_inner (v->subvols);

				for (auto v2 : v->subvols) {
					dot << "obj_" << (void*) v.get() << " -> obj_" << (void*) v2.get() << ";\n";
				}
			}
		}
#endif

		dot << dump_dot_inner (c->get_children());

		if (c->parent && (c->parent->parent == nullptr))
			dot << "}\n";
	}

	return dot.str();
}

#endif

/**
 * get_output
 */
std::string
DotVisitor::get_output (void)
{
	std::string str = output.str();
	str += "\n}\n";

	return str;
}


/**
 * run_dotty
 */
void
DotVisitor::run_dotty (void)
{
	std::string command = "dot -Tpng | display -resize 70% - &";
	std::string input = output.str();
	input += "\n}\n";

#if 0
	std::cout << input << std::endl;
#else
	execute_command2 (command, input);
#endif
}

