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

#ifndef _WHOLE_H_
#define _WHOLE_H_

#include <string>
#include <vector>
#include <memory>

#include "container.h"

class Visitor;
class Whole;

typedef std::shared_ptr<Whole> WholePtr;

/**
 * class Whole
 */
class Whole : public Container
{
public:
	virtual ~Whole() = default;
	static WholePtr create (void);
	virtual bool accept (Visitor& v);

	virtual void add_segment (ContainerPtr& seg);

public:
	// seg_count - LVM2_SEG_COUNT
	std::vector<ContainerPtr> segments;

protected:
	Whole (void);

private:

};


#endif // _WHOLE_H_

