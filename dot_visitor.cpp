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
#include <utility>
#include <ctime>

#include "dot_visitor.h"
#include "container.h"
#include "loop.h"
#ifdef DP_GPT
#include "gpt.h"
#endif
#include "whole.h"
#include "block.h"
#include "partition.h"
#include "filesystem.h"
#ifdef DP_LUKS
#include "luks_table.h"
#endif
#ifdef DP_LVM
#include "lvm_group.h"
#include "lvm_volume.h"
#endif
#include "log_trace.h"
#include "utils.h"
#include "log.h"

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
	return_val_if_fail (c, "red");

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
 * dot_row (std::string)
 */
static
std::string
dot_row (const char* name, const std::string& value)
{
	return_val_if_fail (name, "");

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
static
std::string
dot_row (const char* name, unsigned int value)
{
	return_val_if_fail (name, "");

	std::stringstream row;

	row << "\t\t<tr>";
	row << "<td align=\"left\">" << name << "</td>";
	row << "<td>=</td>";
	row << "<td align=\"left\">" << value << "</td>";
	row << "</tr>\n";

	return row.str();
}

/**
 * dot_row (ContainerPtr)
 */
static
std::string
dot_row (const char* name, ContainerPtr value)
{
	return_val_if_fail (name,  "");
	return_val_if_fail (value, "");

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
		std::string name   = (std::string) *c->get_prop ("name_default");

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
	return_val_if_fail (t, "");

	std::stringstream link;

#if 1
	if (parents.size() > 0) {
		ContainerPtr c(t);
		ContainerPtr parent = parents.top();
		if (parent->name == "dummy") return ""; // Ignore the top level container

		link << "obj_" << (void*) parent.get() << " -> obj_" << (void*) c.get() << ";\n";
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
	return_val_if_fail (t, "");

	ContainerPtr p(t);
	if (!p)
		return "";

	std::stringstream output;

	for (auto prop : p->get_all_props()) {
		if (!(prop->flags & BaseProperty::Flags::Dot)) {
			// log_debug ("Dot ignore : %s\n", prop->name.c_str());
			continue;
		}

		std::string s = (std::string) *prop;
		std::size_t pos = s.find_first_of ('<');
		if (pos != std::string::npos) {
			s = s.replace (pos, 1, "(");
		}
		pos = s.find_first_of ('>');
		if (pos != std::string::npos) {
			s = s.replace (pos, 1, ")");
		}
		if (s.empty()) {
			continue;	// Skip empty values
		}
		output << dot_row (prop->name.c_str(), s);
	}

#if 0
	if (p->device.empty()) {
		if (p->is_a ("Whole")) {
			output << dot_row ("device", "[segments]");
		} else {
			output << dot_row ("device", "[inherit]");
		}
	} else {
		output << dot_row ("device", p->device);
	}

	output << dot_row ("fd",       p->fd);
	output << dot_row ("parent_offset", p->parent_offset);
	if (p->block_size) {
		output << dot_row ("block_size",    p->block_size);
	} else {
		output << dot_row ("block_size",    "[inherit]");
	}

	ContainerPtr cwhole = p->whole;
	output << dot_row ("whole",         cwhole);		//XXX what's this doing here? which class should it be in?
	output << dot_row ("parent",        p->parent);
	output << dot_row ("missing", p->missing);
#endif

	return output.str();
}

template <class T>
std::string
dot_whole (std::shared_ptr<T> t)
{
	return_val_if_fail (t, "");

	WholePtr p(t);
	if (!p)
		return "";

	std::stringstream output;
	unsigned int count = p->segments.size();

	output << dot_container(p);

	if (count > 0) {
		output << dot_row ("segments", count);
		for (auto i : p->segments) {
			output << dot_row ("", i);
		}
	}

	return output.str();
}

#ifdef DP_LVM
template <class T>
std::string
dot_lvm_volume (std::shared_ptr<T> t)
{
	return_val_if_fail (t, "");

	LvmVolumePtr p = std::dynamic_pointer_cast<LvmVolume>(t);
	if (!p)
		return "";

	std::stringstream output;

	output << dot_container(p);

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

#endif
#ifdef DP_MD
template <class T>
std::string
dot_md_volume (std::shared_ptr<T> t)
{
	return_val_if_fail (t, "");

	MdVolumePtr p(t);
	if (!p)
		return "";

	std::stringstream output;

	output << dot_container(p);

	// no specifics for now

	return output.str();
}

#endif

/**
 * visit (ContainerPtr)
 */
bool
DotVisitor::visit (ContainerPtr c)
{
	return_val_if_fail (c, false);

	if (c->name == "dummy")	// Ignore top-level containers
		return true;

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
	return_val_if_fail (l, false);
	//LOG_TRACE;

	dot_objects << dump_table (l, dot_container(l));
	dot_links   << parent_link(l);

	return true;
}

#ifdef DP_GPT
/**
 * visit (GptPtr)
 */
bool
DotVisitor::visit (GptPtr g)
{
	return_val_if_fail (g, false);
	//LOG_TRACE;

	dot_objects << dump_table (g, dot_container(g));
	dot_links   << parent_link(g);

	return true;
}

#endif
#ifdef DP_MSDOS
/**
 * visit (MsdosPtr)
 */
bool
DotVisitor::visit (MsdosPtr m)
{
	return_val_if_fail (m, false);
	//LOG_TRACE;

	dot_objects << dump_table (m, dot_container(m));
	dot_links   << parent_link(m);

	return true;
}

/**
 * visit (ExtendedPtr)
 */
bool
DotVisitor::visit (ExtendedPtr m)
{
	return_val_if_fail (m, false);
	//LOG_TRACE;

	dot_objects << dump_table (m, dot_container(m));

	// Custom parent pointer (same rank)
	if (parents.size() > 0) {
		ContainerPtr c(m);
		dot_links << "obj_"             << (void*) parents.top().get() << " -> obj_" << (void*) c.get() << ";\n";
		// dot_links << "{ rank=same obj_" << (void*) parents.top().get() <<    " obj_" << (void*) c.get() << "}\n";
	}

	return true;
}

#endif
/**
 * visit (MiscPtr)
 */
bool
DotVisitor::visit (MiscPtr m)
{
	return_val_if_fail (m, false);
	//LOG_TRACE;

	dot_objects << dump_table (m, dot_container(m));
	dot_links   << parent_link(m);

	return true;
}

/**
 * visit (PartitionPtr)
 */
bool
DotVisitor::visit (PartitionPtr p)
{
	return_val_if_fail (p, false);
	//LOG_TRACE;

	dot_objects << dump_table (p, dot_container(p));
	dot_links   << parent_link(p);

	return true;
}

/**
 * visit (FilesystemPtr)
 */
bool
DotVisitor::visit (FilesystemPtr f)
{
	return_val_if_fail (f, false);
	//LOG_TRACE;

	dot_objects << dump_table (f, dot_container(f));
	dot_links   << parent_link(f);

	return true;
}


/**
 * visit (WholePtr)
 */
bool
DotVisitor::visit (WholePtr f)
{
	return_val_if_fail (f, false);
	//LOG_TRACE;

	dot_objects << dump_table (f, dot_whole(f));
	dot_links   << parent_link(f);

	for (auto i : f->segments) {
		dot_objects << dump_table (i, dot_container(i));
		dot_links   << parent_link(i);
	}

	return true;
}

#ifdef DP_LVM
/**
 * visit (LvmVolumePtr)
 */
bool
DotVisitor::visit (LvmVolumePtr f)
{
	return_val_if_fail (f, false);
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
	return_val_if_fail (f, false);
	//LOG_TRACE;

	dot_objects << dump_table (f, dot_lvm_volume(f));
	dot_links   << parent_link(f);

	return true;
}

/**
 * visit (LvmGroupPtr)
 */
bool
DotVisitor::visit (LvmGroupPtr f)
{
	return_val_if_fail (f, false);
	//LOG_TRACE;

	dot_objects << dump_table (f, dot_container(f));
	dot_links   << parent_link(f);

	return true;
}

/**
 * visit (LvmLinearPtr)
 */
bool
DotVisitor::visit (LvmLinearPtr f)
{
	return_val_if_fail (f, false);
	//LOG_TRACE;

	dot_objects << dump_table (f, dot_lvm_volume(f));
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

#endif

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
		execute_command2 ("mkdir --parents " + dir, nothing);
		std::string command = "cat > " + dir + "/$RANDOM.gv";
		execute_command2 (command, input);
	}

	if (save_png) {
		dir = "png_" + now;
		execute_command2 ("mkdir --parents " + dir, nothing);
		std::string command = "dot -Tpng > " + dir + "/$RANDOM.png";
		execute_command2 (command, input);
	}
}


