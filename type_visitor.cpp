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

#include "type_visitor.h"
#include "log.h"

TypeVisitor::TypeVisitor (const std::string& search) :
	type (search)
{
}

TypeVisitor::~TypeVisitor()
{
}

bool
TypeVisitor::visit_enter (ContainerPtr& UNUSED(c))
{
	return true;
}

bool
TypeVisitor::visit_leave (void)
{
	return true;
}

bool
TypeVisitor::visit (ContainerPtr c)
{
	if (c->is_a (type)) {
		matches.push_back(c);
	}

	return true;	// Keep looking
}


std::vector<ContainerPtr>
find_all_type (ContainerPtr c, const std::string& type)
{
	return_val_if_fail (c, {});

	TypeVisitor tv (type);
	if (!c->accept (tv))
		return {};		// Something went wrong

	return tv.matches;
}


