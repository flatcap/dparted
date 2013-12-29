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

#include <iostream>

#include "prop_visitor.h"

#include "container.h"

/**
 * PropVisitor
 */
PropVisitor::PropVisitor (void)
{
}

/**
 * ~PropVisitor
 */
PropVisitor::~PropVisitor()
{
}


/**
 * visit_enter
 */
bool
PropVisitor::visit_enter (ContainerPtr& c)
{
	indent++;
	return true;
}

/**
 * visit_leave
 */
bool
PropVisitor::visit_leave (void)
{
	indent--;
	return true;
}


/**
 * visit (ContainerPtr)
 */
bool
PropVisitor::visit (ContainerPtr c)
{
	if (c->name != "dummy") {
		std::string tabs;
		if (indent > 0) {
			tabs.resize (indent, '\t');
		}
		for (auto n : c->get_prop_names()) {
			VPtr v = c->get_prop (n);
			output << tabs << v->owner << ": " << v->name << " = " << (std::string) *v << "\n";
		}
	}

	return true;
}


/**
 * dump
 */
void
PropVisitor::dump (void)
{
	std::cout << output.str();
}
