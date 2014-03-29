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


#include "list_visitor.h"
#include "container.h"
#include "log_trace.h"

ListVisitor::ListVisitor (void)
{
}

ListVisitor::~ListVisitor()
{
}


bool
ListVisitor::visit_enter (ContainerPtr& UNUSED(c))
{
	++indent;
	return true;
}

bool
ListVisitor::visit_leave (void)
{
	indent--;
	return true;
}


/**
 * visit (ContainerPtr)
 */
bool
ListVisitor::visit (ContainerPtr c)
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


void
ListVisitor::list (void)
{
	log_debug ("%s\n", output.str().c_str());
}
