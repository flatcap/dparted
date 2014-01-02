/* Copyright (c) 2014 Richard Russon (FlatCap)
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

#include <iostream>

#include "dump_visitor.h"

#include "container.h"
#include "log_trace.h"

/**
 * DumpVisitor
 */
DumpVisitor::DumpVisitor (void)
{
}

/**
 * ~DumpVisitor
 */
DumpVisitor::~DumpVisitor()
{
}


/**
 * visit_enter
 */
bool
DumpVisitor::visit_enter (ContainerPtr& c)
{
	indent++;
	return true;
}

/**
 * visit_leave
 */
bool
DumpVisitor::visit_leave (void)
{
	indent--;
	return true;
}


/**
 * visit (ContainerPtr)
 */
bool
DumpVisitor::visit (ContainerPtr c)
{
	if (c->name != "dummy") {
		std::string tabs;
		if (indent > 0) {
			tabs.resize (indent, '\t');
		}
		output << tabs << c << "\n";
	}

	return true;
}

/**
 * visit (LvmVolumePtr)
 */
bool
DumpVisitor::visit (LvmVolumePtr l)
{
	std::string tabs;
	if (indent > 0) {
		tabs.resize (indent, '\t');
	}
	output << tabs << l << "\n";

	tabs += "\t";

	for (auto c : l->subvols) {
		output << tabs << c << "\n";
	}

	for (auto c : l->metadata) {
		output << tabs << c << "\n";
	}

	return true;
}


/**
 * dump
 */
void
DumpVisitor::dump (void)
{
	std::cout << output.str();
}
