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

#include "uuid_visitor.h"

UuidVisitor::UuidVisitor (const std::string& search) :
	uuid (search)
{
}

UuidVisitor::~UuidVisitor()
{
}

bool
UuidVisitor::visit_enter (ContainerPtr& UNUSED(c))
{
	return true;
}

bool
UuidVisitor::visit_leave (void)
{
	return true;
}

bool
UuidVisitor::visit (ContainerPtr c)
{
	if (c->uuid == uuid) {
		match = c;
		return false;	// Stop looking
	}

	return true;
}


ContainerPtr
find_first_uuid (ContainerPtr c, const std::string& uuid)
{
	if (!c)
		return nullptr;

	ContainerPtr match;

	UuidVisitor uv (uuid);
	if (c->accept (uv)) {
		std::cout << "no match for " << uuid << std::endl;
		return nullptr;
	}

	std::cout << "match for " << uuid << std::endl;
	return uv.match;
}


