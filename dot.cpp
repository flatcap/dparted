/* Copyright (c) 2013 Richard Russon (FlatCap)
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Library General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <sstream>
#include <string>

#include "block.h"
#include "container.h"
#include "disk.h"
#include "extended.h"
#include "file.h"
#include "filesystem.h"
#include "gpt.h"
#include "loop.h"
#include "lvm_group.h"
#include "lvm_linear.h"
#include "lvm_mirror.h"
#include "lvm_metadata.h"
#include "lvm_partition.h"
#include "lvm_raid.h"
#include "lvm_stripe.h"
#include "lvm_table.h"
#include "lvm_volume.h"
#include "md_table.h"
#include "misc.h"
#include "msdos.h"
#include "partition.h"
#include "table.h"
#include "volume.h"
#include "whole.h"

#include "utils.h"
#include "log_trace.h"

/**
 * get_colour
 */
static std::string
get_colour (DPContainer *c)
{
	if (c->is_a ("block"))      return "#aaffaa";
	if (c->is_a ("filesystem")) return "#bbffff";
	if (c->is_a ("table"))      return "#ffbbbb";
	if (c->is_a ("partition"))  return "#ffffdd";
	if (c->is_a ("whole"))      return "#ccccff";
	if (c->is_a ("misc"))       return "orange";

	return "white";
}


/**
 * dot_row (bool)
 */
static std::string
dot_row (const char *name, bool value)
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
static std::string
dot_row (const char *name, int value)
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
static std::string
dot_row (const char *name, long value)
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
static std::string
dot_row (const char *name, const std::string &value)
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
static std::string
dot_row (const char *name, const std::stringstream &value)
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
 * dot_row (char *)
 */
static std::string
dot_row (const char *name, const char *value)
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
static std::string
dot_row (const char *name, unsigned int value)
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
 * dot_row (void *)
 */
static std::string
dot_row (const char *name, void *value)
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
 * dot_container
 */
static std::string
dot_container (DPContainer *c)
{
	std::stringstream output;
	std::string uuid_short = c->uuid;

	if ((uuid_short.size() > 8) && (uuid_short[0] != '/')) {
		uuid_short = c->uuid.substr (0, 6) + "...";
	}

	output << dot_row ("type",          c->type.back());
	//output << dot_row ("name",          c->name);
	output << dot_row ("uuid",          uuid_short); //RAR temp
	if (c->device.empty())
		output << dot_row ("device", "[inherit]");
	else
		output << dot_row ("device", c->device);
	output << dot_row ("file desc",     c->fd);
	output << dot_row ("parent_offset", c->parent_offset);
	output << dot_row ("block_size",    c->block_size);
	output << dot_row ("bytes_size",    c->bytes_size);
	output << dot_row ("bytes_used",    c->bytes_used);
	output << dot_row ("bytes_free",    c->bytes_size - c->bytes_used);
	output << dot_row ("whole",         c->whole);		//XXX what's this doing here?
	//output << dot_row ("parent",        c->parent);

	if (c->missing)
		output << dot_row ("missing", c->missing);

	return output.str();
}


/**
 * dot_block
 */
static std::string
dot_block (Block *b)
{
	std::stringstream output;

	output << dot_container (dynamic_cast<DPContainer *> (b));

	// no specifics for now

	return output.str();
}

/**
 * dot_disk
 */
static std::string
dot_disk (Disk *d)
{
	std::stringstream output;

	output << dot_block (dynamic_cast<Block *> (d));

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
 * dot_loop
 */
static std::string
dot_loop (Loop *l)
{
	std::stringstream output;
	std::stringstream mm;

	output << dot_block(dynamic_cast<Block *> (l));

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
	mm.str("");
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
static std::string
dot_table (Table *t)
{
	std::stringstream output;

	output << dot_container(dynamic_cast<DPContainer *> (t));

	// no specifics for now

	return output.str();
}

/**
 * dot_msdos
 */
static std::string
dot_msdos (Msdos *m)
{
	std::stringstream output;

	output << dot_table(dynamic_cast<Table *> (m));

	// no specifics for now

	return output.str();
}

/**
 * dot_extended
 */
static std::string
dot_extended (Extended *e)
{
	std::stringstream output;

	output << dot_msdos(dynamic_cast<Msdos *> (e));

	// no specifics for now

	//RAR how? output << "{ rank=same obj_" << (void*) this << " obj_" << parent << " }\n";

	return output.str();
}

/**
 * dot_gpt
 */
static std::string
dot_gpt (Gpt *g)
{
	std::stringstream output;

	output << dot_table(dynamic_cast<Table *> (g));

	// no specifics for now

	return output.str();
}

/**
 * dot_md_table
 */
static std::string
dot_md_table (MdTable *t)
{
	std::stringstream output;

	output << dot_table(dynamic_cast<Table *> (t));

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
 * dot_lvm_table
 */
static std::string
dot_lvm_table (LvmTable *t)
{
	std::stringstream output;

	output << dot_table(dynamic_cast<Table *> (t));

	output << dot_row ("vol_name",      t->vol_name);
	output << dot_row ("seq_num",       t->seq_num);
	output << dot_row ("metadata_size", t->metadata_size);
	output << dot_row ("attr",          t->attr);
	//output << dot_row ("config",   t->config);

	return output.str();
}


/**
 * dot_whole
 */
static std::string
dot_whole (Whole *w)
{
	std::stringstream output;
	unsigned int count = w->segments.size();

	output << dot_container(dynamic_cast<DPContainer *> (w));

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
static std::string
dot_volume (Volume *v)
{
	std::stringstream output;

	output << dot_whole(dynamic_cast<Whole *> (v));

	// no specifics for now

	return output.str();
}

/**
 * dot_lvm_group
 */
static std::string
dot_lvm_group (LvmGroup *g)
{
	std::stringstream output;

	output << dot_whole(dynamic_cast<Whole *> (g));

	output << dot_row ("pv_count",        g->pv_count);
	output << dot_row ("lv_count",        g->lv_count);
	output << dot_row ("vg_attr",         g->vg_attr);
	output << dot_row ("vg_seqno",        g->vg_seqno);

	return output.str();
}

/**
 * dot_lvm_volume
 */
static std::string
dot_lvm_volume (LvmVolume *v)
{
	std::stringstream output;

	output << dot_volume(dynamic_cast<Volume *> (v));

	unsigned int count = v->metadata.size();
	if (count > 0) {
		output << dot_row ("metadata", count);
		for (auto i : v->metadata) {
			output << dot_row ("", i);
		}
	}

	count = v->subvols.size();
	if (count > 0) {
		output << dot_row ("subvols", count);
		for (auto i : v->subvols) {
			output << dot_row ("", i);
		}
	}

	output << dot_row ("sibling", v->sibling);

	return output.str();
}


/**
 * dot_lvm_linear
 */
static std::string
dot_lvm_linear (LvmLinear *l)
{
	std::stringstream output;

	output << dot_lvm_volume(dynamic_cast<LvmVolume *> (l));

	// no specifics for now

	return output.str();
}

/**
 * dot_lvm_metadata
 */
static std::string
dot_lvm_metadata (LvmMetadata *m)
{
	std::stringstream output;

	output << dot_lvm_linear(dynamic_cast<LvmLinear *> (m));

	// no specifics for now

	return output.str();
}

/**
 * dot_lvm_mirror
 */
static std::string
dot_lvm_mirror (LvmMirror *m)
{
	std::stringstream output;

	output << dot_lvm_volume(dynamic_cast<LvmVolume *> (m));

	// no specifics for now

	return output.str();
}

/**
 * dot_lvm_raid
 */
static std::string
dot_lvm_raid (LvmRaid *s)
{
	std::stringstream output;

	output << dot_lvm_volume(dynamic_cast<LvmVolume *> (s));

	// no specifics for now

	return output.str();
}

/**
 * dot_lvm_stripe
 */
static std::string
dot_lvm_stripe (LvmStripe *s)
{
	std::stringstream output;

	output << dot_lvm_volume(dynamic_cast<LvmVolume *> (s));

	// no specifics for now

	return output.str();
}


/**
 * dot_partition
 */
static std::string
dot_partition (Partition *p)
{
	std::stringstream output;

	output << dot_container(dynamic_cast<Partition *> (p));

	// no specifics for now

	return output.str();
}

/**
 * dot_lvm_partition
 */
static std::string
dot_lvm_partition (LvmPartition *p)
{
	std::stringstream output;

	output << dot_partition(dynamic_cast<Partition *> (p));

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


/**
 * dot_file
 */
static std::string
dot_file (File *f)
{
	std::stringstream output;

	// no specifics for now

	output << dot_block(dynamic_cast<Block *> (f));

	return output.str();
}

/**
 * dot_filesystem
 */
static std::string
dot_filesystem (Filesystem *f)
{
	std::stringstream output;

	output << dot_container(dynamic_cast<DPContainer *> (f));

	output << dot_row ("label", f->label);

	return output.str();
}

/**
 * dot_misc
 */
static std::string
dot_misc (Misc *m)
{
	std::stringstream output;

	output << dot_container(dynamic_cast<DPContainer *> (m));

	// no specifics for now

	return output.str();
}


/**
 * dump_dot_inner
 */
static std::string
dump_dot_inner (std::vector <DPContainer*> &v)
{
	std::stringstream dot;
	int count = 0;

	for (auto c : v) {
		std::string type = c->type.back();
#if 0
		if (type == "loop")
			continue;
#endif

		//if (c->dot_colour == "#ccccff") continue;
		//log_info ("%s\n", c->name.c_str());

		// Isolate the top-level objects
#if 1
		if (c->parent && (c->parent->parent == nullptr))
			dot << "subgraph cluster_" << count++ << " { color=transparent\n;";
#endif

		dot << "\n";
		dot << "// " << c << "\n";

		if (c->name.empty()) {
			c->name = "UNKNOWN";
		}

		std::string colour = get_colour (c);
		std::string missing;
		if (c->missing) {
			missing = " MISSING";
		}

		dot << "obj_" << (void*) c << " [fillcolor=\"" << colour << "\",label=<<table cellspacing=\"0\" border=\"0\">\n";
		dot << "<tr><td align=\"left\" bgcolor=\"white\" colspan=\"3\"><font color=\"#000000\" point-size=\"20\"><b>" << c->name << "</b></font> (" << (void*) c << ")<font color=\"#ff0000\" point-size=\"20\"><b> : " << c->ref_count << missing << "</b></font></td></tr>\n";

		     if (type == "block")         { dot << dot_block         (dynamic_cast<Block        *> (c)); }
		else if (type == "container")     { dot << dot_container     (dynamic_cast<DPContainer  *> (c)); }
		else if (type == "disk")          { dot << dot_disk          (dynamic_cast<Disk         *> (c)); }
		else if (type == "extended")      { dot << dot_extended      (dynamic_cast<Extended     *> (c)); }
		else if (type == "file")          { dot << dot_file          (dynamic_cast<File         *> (c)); }
		else if (type == "filesystem")    { dot << dot_filesystem    (dynamic_cast<Filesystem   *> (c)); }
		else if (type == "gpt")           { dot << dot_gpt           (dynamic_cast<Gpt          *> (c)); }
		else if (type == "loop")          { dot << dot_loop          (dynamic_cast<Loop         *> (c)); }
		else if (type == "lvm_group")     { dot << dot_lvm_group     (dynamic_cast<LvmGroup     *> (c)); }
		else if (type == "lvm_linear")    { dot << dot_lvm_linear    (dynamic_cast<LvmLinear    *> (c)); }
		else if (type == "lvm_mirror")    { dot << dot_lvm_mirror    (dynamic_cast<LvmMirror    *> (c)); }
		else if (type == "lvm_partition") { dot << dot_lvm_partition (dynamic_cast<LvmPartition *> (c)); }
		else if (type == "lvm_metadata")  { dot << dot_lvm_metadata  (dynamic_cast<LvmMetadata  *> (c)); }
		else if (type == "lvm_raid")      { dot << dot_lvm_raid      (dynamic_cast<LvmRaid      *> (c)); }
		else if (type == "lvm_stripe")    { dot << dot_lvm_stripe    (dynamic_cast<LvmStripe    *> (c)); }
		else if (type == "lvm_table")     { dot << dot_lvm_table     (dynamic_cast<LvmTable     *> (c)); }
		else if (type == "lvm_volume")    { dot << dot_lvm_volume    (dynamic_cast<LvmVolume    *> (c)); }
		else if (type == "md_table")      { dot << dot_md_table      (dynamic_cast<MdTable      *> (c)); }
		else if (type == "misc")          { dot << dot_misc          (dynamic_cast<Misc         *> (c)); }
		else if (type == "msdos")         { dot << dot_msdos         (dynamic_cast<Msdos        *> (c)); }
		else if (type == "partition")     { dot << dot_partition     (dynamic_cast<Partition    *> (c)); }
		else if (type == "table")         { dot << dot_table         (dynamic_cast<Table        *> (c)); }
		else if (type == "volume")        { dot << dot_volume        (dynamic_cast<Volume       *> (c)); }
		else if (type == "whole")         { dot << dot_whole         (dynamic_cast<Whole        *> (c)); }

		dot << "</table>>];\n";

		for (auto c2 : c->children) {
			dot << "obj_" << (void*) c << " -> obj_" << (void*) c2 << ";\n";
		}

#if 0
		if (c->is_a("whole")) {
			Whole *w = dynamic_cast<Whole*>(c);
			if (w) {
				for (auto w2 : w->segments) {
					dot << "obj_" << (void*) w << " -> obj_" << (void*) w2 << " [constraint=false style=dashed];\n";
				}
			}
		}
#endif

#if 1
		if (c->is_a("lvm_volume")) {
			LvmVolume *v = dynamic_cast<LvmVolume*>(c);
			if (v) {
				//log_info ("VOLUME %s, %ld\n", v->type.back().c_str(), v->subvols.size());
				dot << dump_dot_inner (v->metadata);

				for (auto v2 : v->metadata) {
					dot << "obj_" << (void*) v << " -> obj_" << (void*) v2 << ";\n";
				}

				dot << dump_dot_inner (v->subvols);

				for (auto v2 : v->subvols) {
					dot << "obj_" << (void*) v << " -> obj_" << (void*) v2 << ";\n";
				}
			}
		}
#endif

		dot << dump_dot_inner (c->children);

#if 1
		if (c->parent && (c->parent->parent == nullptr))
			dot << "}\n";
#endif
	}

	return dot.str();
}

/**
 * dump_dot
 */
std::string
dump_dot (std::vector <DPContainer*> v)
{
	std::stringstream dot;

	dot << "digraph disks {\n";
	dot << "graph [ rankdir=\"TB\", color=\"white\",bgcolor=\"#000000\" ];\n";
	dot << "node [ shape=\"record\", color=\"black\", fillcolor=\"lightcyan\", style=\"filled\" ];\n";
	dot << "edge [ penwidth=3.0,color=\"#cccccc\" ];\n";
	dot << "\n";

	dot << dump_dot_inner (v);

	dot << "\n}";
	dot << "\n";

	return dot.str();
}

/**
 * display_dot
 */
void
display_dot (std::vector <DPContainer*> v)
{
	if (v.size() == 0)
		return;

	std::string input = dump_dot(v);
	//std::cout << input << std::endl;

	execute_command2 ("dot -Tpng | display -resize 60% - &", input);
}


