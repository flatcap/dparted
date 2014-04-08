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

#ifndef _LIST_VISITOR_H_
#define _LIST_VISITOR_H_

#include <sstream>

#include "visitor.h"

class ListVisitor : public Visitor
{
public:
	ListVisitor (void);
	virtual ~ListVisitor();

	virtual bool visit_enter (ContainerPtr& c);
	virtual bool visit_leave (void);

	virtual bool visit (ContainerPtr c);

	void list (void);

protected:
	std::stringstream output;

	int indent = -1;
};

// Simple wrapper
template<class T>
void run_list (std::shared_ptr<T>& top)
{
	ContainerPtr c (top);
	if (!c)
		return;

	ListVisitor lv;
	c->accept (lv);
	lv.list();
}

#endif // _LIST_VISITOR_H_

