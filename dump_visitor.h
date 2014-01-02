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

#ifndef _DUMP_VISITOR_H_
#define _DUMP_VISITOR_H_

#include <sstream>

#include "visitor.h"

/**
 * class DumpVisitor
 */
class DumpVisitor : public Visitor
{
public:
	DumpVisitor (void);
	virtual ~DumpVisitor();

	virtual bool visit_enter (ContainerPtr& c);
	virtual bool visit_leave (void);

	virtual bool visit (ContainerPtr c);
	virtual bool visit (LvmVolumePtr c);

	void dump (void);

protected:
	std::stringstream output;

	int indent = -1;
};


#endif // _DUMP_VISITOR_H_

