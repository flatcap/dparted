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


#include "prop_visitor.h"

#include "container.h"
#include "utils.h"

PropVisitor::PropVisitor (void)
{
	LOG_CTOR;
}

PropVisitor::~PropVisitor()
{
	LOG_DTOR;
}


bool
PropVisitor::visit_enter (ContainerPtr& UNUSED(c))
{
	++indent;
	return true;
}

bool
PropVisitor::visit_leave (void)
{
	--indent;
	return true;
}


/**
 * visit (ContainerPtr)
 */
bool
PropVisitor::visit (ContainerPtr c)
{
	if (c->name != "TopLevel") {
		std::string tabs;
		if (indent > 0) {
			tabs.resize (indent, '\t');
		}
		for (auto& n : c->get_prop_names()) {
			PPtr p = c->get_prop(n);
			output << tabs << p->owner << ": " << p->name << " = " << (std::string) *p << " (" << p->get_type_name() << ")\n";
		}
	}

	return true;
}


void
PropVisitor::list (void)
{
	log_info (output);
}
