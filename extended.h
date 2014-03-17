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

#ifndef _EXTENDED_H_
#define _EXTENDED_H_

#include <memory>
#include <string>
#include <vector>

#include "msdos.h"

class Extended;

typedef std::shared_ptr<Extended> ExtendedPtr;

class Extended : public Msdos
{
public:
	static ExtendedPtr create (void);
	virtual ~Extended();
	virtual bool accept (Visitor& v);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

	static ExtendedPtr probe (ContainerPtr& top_level, ContainerPtr& parent, std::uint64_t offset, std::uint64_t size);
	virtual unsigned char* get_buffer (std::uint64_t offset, std::uint64_t size);

public:
	//properties

protected:
	Extended (void);

private:

};

#endif // _EXTENDED_H_

