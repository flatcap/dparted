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

#ifndef _TYPE_VISITOR_H_
#define _TYPE_VISITOR_H_

#include <string>
#include <vector>

#include "visitor.h"

std::vector<ContainerPtr> find_all_type (ContainerPtr c, const std::string& type);

class TypeVisitor : public Visitor
{
public:
	TypeVisitor (const std::string& search);
	virtual ~TypeVisitor();

	virtual bool visit_enter (ContainerPtr& c);
	virtual bool visit_leave (void);

	virtual bool visit (ContainerPtr c);

	std::string type;
	std::vector<ContainerPtr> matches;
};

#endif // _TYPE_VISITOR_H_

