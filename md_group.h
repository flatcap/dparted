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

#ifndef _MD_GROUP_H_
#define _MD_GROUP_H_

#include <memory>

#include "whole.h"

class MdGroup;
class MdTable;
class Visitor;

typedef std::shared_ptr<MdGroup> MdGroupPtr;

/**
 * class MdGroup
 */
class MdGroup : public Whole
{
public:
	virtual ~MdGroup() = default;
	static MdGroupPtr create (void);
	virtual bool accept (Visitor& v);

	static void discover (ContainerPtr& top_level);

public:
	//properties

protected:
	MdGroup (void);

private:

};


#endif // _MD_GROUP_H_

