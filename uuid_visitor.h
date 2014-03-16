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

#ifndef _UUID_VISITOR_H_
#define _UUID_VISITOR_H_

#include "visitor.h"

ContainerPtr find_first_uuid (ContainerPtr c, const std::string& uuid);

class UuidVisitor : public Visitor
{
public:
	UuidVisitor (const std::string& search);
	virtual ~UuidVisitor();

	virtual bool visit_enter (ContainerPtr& c);
	virtual bool visit_leave (void);

	virtual bool visit (ContainerPtr c);

	std::string uuid;
	ContainerPtr match;
protected:

};

#endif // _UUID_VISITOR_H_

