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

#include <memory>
#include <set>
#include <string>
#include <vector>

#include "container.h"

typedef std::shared_ptr<class Whole> WholePtr;

class Whole : public Container
{
public:
	static WholePtr create (void);
	virtual ~Whole();
	Whole& operator= (const Whole& c);
	Whole& operator= (Whole&& c);

	void swap (Whole& c);
	friend void swap (Whole& lhs, Whole& rhs);

	virtual bool accept (Visitor& v);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

	virtual void add_segment (ContainerPtr seg);

public:
	// properties
	// seg_count - LVM2_SEG_COUNT
	std::vector<ContainerPtr> segments;	//XXX not declared

protected:
	Whole (void);
	Whole (const Whole& c);
	Whole (Whole&& c);

	virtual Whole* clone (void);
};

#endif // _WHOLE_H_

