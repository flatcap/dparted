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

#ifndef _WHOLE_H_
#define _WHOLE_H_

#include <string>
#include <vector>
#include <memory>

#include "container.h"
#include "action.h"

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

	virtual void add_segment (ContainerPtr seg);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

public:
	// seg_count - LVM2_SEG_COUNT
	std::vector<ContainerPtr> segments;

protected:
	Whole (void);

private:

};


#endif // _WHOLE_H_

