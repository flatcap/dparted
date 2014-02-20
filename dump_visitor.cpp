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

#include "dump_visitor.h"
#include "container.h"
#include "log_trace.h"

DumpVisitor::DumpVisitor (void)
{
}

DumpVisitor::~DumpVisitor()
{
}


bool
DumpVisitor::visit_enter (ContainerPtr& UNUSED(c))
{
	indent++;
	return true;
}

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
	ContainerPtr c(l);

	std::string tabs;
	if (indent > 0) {
		tabs.resize (indent, '\t');
	}
	output << tabs << c << "\n";

	tabs += "\t";

	for (auto c : l->subvols) {
		output << tabs << c << "\n";
	}

	for (auto c : l->metadata) {
		output << tabs << c << "\n";
	}

	return true;
}


void
DumpVisitor::dump (void)
{
	std::cout << output.str();
}
