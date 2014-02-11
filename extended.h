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

#ifndef _EXTENDED_H
#define _EXTENDED_H

#include <string>
#include <memory>

#include "msdos.h"

class Extended;
class Visitor;

typedef std::shared_ptr<Extended> ExtendedPtr;

/**
 * class Extended
 */
class Extended : public Msdos
{
public:
	virtual ~Extended() = default;
	static ExtendedPtr create (void);
	virtual bool accept (Visitor& v);

	static ExtendedPtr probe (ContainerPtr& top_level, ContainerPtr& parent, long offset, long size);

public:
	//properties

protected:
	Extended (void);

private:

};


#endif // _EXTENDED_H

