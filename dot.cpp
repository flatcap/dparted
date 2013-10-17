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
#include "lvm_partition.h"
#include "lvm_stripe.h"
#include "lvm_table.h"
#include "lvm_volume.h"
#include "misc.h"
#include "msdos.h"
#include "partition.h"
#include "table.h"
#include "volume.h"
#include "whole.h"

#include "utils.h"
#include "log_trace.h"

#if 0
/**
 * dot_row (bool)
 */
static std::string
dot_row (const char *name, bool value)
{
	std::ostringstream s;

	s << "<tr>";
	s << "<td align=\"left\">" << name << "</td>";
	s << "<td>=</td>";
	s << "<td align=\"left\">" << (value ? "true" : "false") << "</td>";
	s << "</tr>\n";

	return s.str();
}

/**
 * dot_row (int)
 */
static std::string
dot_row (const char *name, int value)
{
	std::ostringstream s;

	s << "<tr>";
	s << "<td align=\"left\">" << name << "</td>";
	s << "<td>=</td>";
	s << "<td align=\"left\">" << value << "</td>";
	s << "</tr>\n";

	return s.str();
}

#endif
/**
 * dot_row (long)
 */
static std::string
dot_row (const char *name, long value)
{
	std::ostringstream s;

	s << "<tr>";
	s << "<td align=\"left\">" << name << "</td>";
	s << "<td>=</td>";
	s << "<td align=\"left\">" << value << "</td>";
	s << "</tr>\n";

	return s.str();
}

/**
 * dot_row (long long)
 */
static std::string
dot_row (const char *name, long long value)
{
	std::ostringstream s;
	std::string str = get_size (value);

	s << "<tr>";
	s << "<td align=\"left\">" << name << "</td>";
	s << "<td>=</td>";
#if 1
	s << "<td align=\"left\">" << str << "</td>";
#else
	s << "<td align=\"left\">" << value << " (" << str << ")</td>";
#endif
	s << "</tr>\n";

	return s.str();
}

/**
 * dot_row (std::string)
 */
static std::string
dot_row (const char *name, std::string &value)
{
	std::ostringstream s;

	s << "<tr>";
	s << "<td align=\"left\">" << name << "</td>";
	s << "<td>=</td>";
	s << "<td align=\"left\">" << value << "</td>";
	s << "</tr>\n";

	return s.str();
}

/**
 * dot_row (unsigned int)
 */
static std::string
dot_row (const char *name, unsigned int value)
{
	std::ostringstream s;

	s << "<tr>";
	s << "<td align=\"left\">" << name << "</td>";
	s << "<td>=</td>";
	s << "<td align=\"left\">" << value << "</td>";
	s << "</tr>\n";

	return s.str();
}

/**
 * dot_row (void *)
 */
static std::string
dot_row (const char *name, void *value)
{
	std::ostringstream s;

	s << "<tr>";
	s << "<td align=\"left\">" << name << "</td>";
	s << "<td>=</td>";
	s << "<td align=\"left\">" << value << "</td>";
	s << "</tr>\n";

	return s.str();
}


/**
 * dot_container
 */
static std::string
dot_container (DPContainer *c)
{
	std::ostringstream output;
	std::string uuid_short = c->uuid;

	if (!uuid_short.empty()) {
		uuid_short = c->uuid.substr (0, 6) + "...";
	}

	output << dot_row ("type",          c->type.back());
	output << dot_row ("name",          c->name);
	output << dot_row ("uuid",          uuid_short); //RAR temp
	output << dot_row ("device",        c->device);
	output << dot_row ("file desc",     c->fd);
	output << dot_row ("parent_offset", c->parent_offset);
	//output << dot_row ("block_size",    c->block_size);
	output << dot_row ("bytes_size",    c->bytes_size);
	output << dot_row ("bytes_used",    c->bytes_used);
	output << dot_row ("bytes_free",    c->bytes_size - c->bytes_used);
	output << dot_row ("whole",         c->whole);
	//output << dot_row ("parent",        c->parent);

	return output.str();
}


/**
 * dot_block
 */
static std::string
dot_block (Block *b)
{
	std::ostringstream output;

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
	std::ostringstream output;

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
	std::ostringstream output;

	output << dot_block(dynamic_cast<Block *> (l));

	// no specifics for now

	return output.str();
}


/**
 * dot_table
 */
static std::string
dot_table (Table *t)
{
	std::ostringstream output;

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
	std::ostringstream output;

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
	std::ostringstream output;

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
	std::ostringstream output;

	output << dot_table(dynamic_cast<Table *> (g));

	// no specifics for now

	return output.str();
}

/**
 * dot_lvm_table
 */
static std::string
dot_lvm_table (LVMTable *t)
{
	std::ostringstream output;

	output << dot_table(dynamic_cast<Table *> (t));

	// no specifics for now

	return output.str();
}


/**
 * dot_whole
 */
static std::string
dot_whole (Whole *w)
{
	std::ostringstream output;
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
	std::ostringstream output;

	output << dot_whole(dynamic_cast<Whole *> (v));

	// no specifics for now

	return output.str();
}

/**
 * dot_lvm_group
 */
static std::string
dot_lvm_group (LVMGroup *g)
{
	std::ostringstream output;

	output << dot_whole(dynamic_cast<Whole *> (g));

	output << dot_row ("pv_count",        g->pv_count);
	output << dot_row ("lv_count",        g->lv_count);
	output << dot_row ("vg_attr",         g->vg_attr);
	output << dot_row ("vg_extent_count", g->vg_extent_count);
	output << dot_row ("vg_free_count",   g->vg_free_count);
	output << dot_row ("vg_seqno",        g->vg_seqno);

	return output.str();
}

/**
 * dot_lvm_volume
 */
static std::string
dot_lvm_volume (LVMVolume *v)
{
	std::ostringstream output;
	unsigned int count = v->tables.size();

	output << dot_volume(dynamic_cast<Volume *> (v));

	if (count > 0) {
		output << dot_row ("tables", count);
		for (auto i : v->tables) {
			output << dot_row ("", i);
		}
	}

	return output.str();
}


/**
 * dot_lvm_linear
 */
static std::string
dot_lvm_linear (LVMLinear *l)
{
	std::ostringstream output;

	output << dot_lvm_volume(dynamic_cast<LVMVolume *> (l));

	// no specifics for now

	return output.str();
}

/**
 * dot_lvm_mirror
 */
static std::string
dot_lvm_mirror (LVMMirror *m)
{
	std::ostringstream output;

	output << dot_lvm_volume(dynamic_cast<LVMVolume *> (m));

	// no specifics for now

	return output.str();
}

/**
 * dot_lvm_stripe
 */
static std::string
dot_lvm_stripe (LVMStripe *s)
{
	std::ostringstream output;

	output << dot_lvm_volume(dynamic_cast<LVMVolume *> (s));

	// no specifics for now

	return output.str();
}


/**
 * dot_partition
 */
static std::string
dot_partition (Partition *p)
{
	std::ostringstream output;

	output << dot_container(dynamic_cast<Partition *> (p));

	// no specifics for now

	return output.str();
}

/**
 * dot_lvm_partition
 */
static std::string
dot_lvm_partition (LVMPartition *p)
{
	std::ostringstream output;

	output << dot_partition(dynamic_cast<Partition *> (p));

	// no specifics for now

	return output.str();
}


/**
 * dot_file
 */
static std::string
dot_file (File *f)
{
	std::ostringstream output;

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
	std::ostringstream output;

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
	std::ostringstream output;

	output << dot_container(dynamic_cast<DPContainer *> (m));

	// no specifics for now

	return output.str();
}


/**
 * dump_dot_inner
 */
static std::string
dump_dot_inner (std::vector <DPContainer*> v)
{
	std::ostringstream dot;

	for (auto c : v) {
		//if (c->dot_colour == "#ccccff") continue;
		//printf ("%s\n", c->name.c_str());
		dot << "\n";
		dot << "// " << c << "\n";

		if (c->name.empty()) {
			c->name = "UNKNOWN";
		}
		if (c->dot_colour.empty()) {
			//log_error ("empty colour on %s\n", c->name.c_str());
			c->dot_colour = "white";
		}

		dot << "obj_" << (void*) c << " [fillcolor=\"" << c->dot_colour << "\",label=<<table cellspacing=\"0\" border=\"0\">\n";
		dot << "<tr><td align=\"left\" bgcolor=\"white\" colspan=\"3\"><font color=\"#000000\" point-size=\"20\"><b>" << c->name << "</b></font> (" << (void*) c << ")<font color=\"#ff0000\" point-size=\"20\"><b> : " << c->ref_count << "</b></font></td></tr>\n";

		std::string type = c->type.back();
		     if (type == "block")         { dot << dot_block         (dynamic_cast<Block        *> (c)); }
		else if (type == "container")     { dot << dot_container     (dynamic_cast<DPContainer  *> (c)); }
		else if (type == "disk")          { dot << dot_disk          (dynamic_cast<Disk         *> (c)); }
		else if (type == "extended")      { dot << dot_extended      (dynamic_cast<Extended     *> (c)); }
		else if (type == "file")          { dot << dot_file          (dynamic_cast<File         *> (c)); }
		else if (type == "filesystem")    { dot << dot_filesystem    (dynamic_cast<Filesystem   *> (c)); }
		else if (type == "gpt")           { dot << dot_gpt           (dynamic_cast<Gpt          *> (c)); }
		else if (type == "loop")          { dot << dot_loop          (dynamic_cast<Loop         *> (c)); }
		else if (type == "lvm_group")     { dot << dot_lvm_group     (dynamic_cast<LVMGroup     *> (c)); }
		else if (type == "lvm_linear")    { dot << dot_lvm_linear    (dynamic_cast<LVMLinear    *> (c)); }
		else if (type == "lvm_mirror")    { dot << dot_lvm_mirror    (dynamic_cast<LVMMirror    *> (c)); }
		else if (type == "lvm_partition") { dot << dot_lvm_partition (dynamic_cast<LVMPartition *> (c)); }
		else if (type == "lvm_stripe")    { dot << dot_lvm_stripe    (dynamic_cast<LVMStripe    *> (c)); }
		else if (type == "lvm_table")     { dot << dot_lvm_table     (dynamic_cast<LVMTable     *> (c)); }
		else if (type == "lvm_volume")    { dot << dot_lvm_volume    (dynamic_cast<LVMVolume    *> (c)); }
		else if (type == "misc")          { dot << dot_misc          (dynamic_cast<Misc         *> (c)); }
		else if (type == "msdos")         { dot << dot_msdos         (dynamic_cast<Msdos        *> (c)); }
		else if (type == "partition")     { dot << dot_partition     (dynamic_cast<Partition    *> (c)); }
		else if (type == "table")         { dot << dot_table         (dynamic_cast<Table        *> (c)); }
		else if (type == "volume")        { dot << dot_volume        (dynamic_cast<Volume       *> (c)); }
		else if (type == "whole")         { dot << dot_whole         (dynamic_cast<Whole        *> (c)); }

		dot << "</table>>];\n";

		if (c->parent) {
			dot << "obj_" << (void*) c->parent << " -> obj_" << (void*) c << ";\n";
		}

		dot << dump_dot_inner (c->children);
	}

	return dot.str();
}

/**
 * dump_dot
 */
std::string
dump_dot (std::vector <DPContainer*> v)
{
	std::ostringstream dot;

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
	std::string input = dump_dot(v);

	execute_command2 ("dot -Tpng | display -resize 50% - &", input);
}


