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

typedef std::shared_ptr<class Extended> ExtendedPtr;

class Extended : public Msdos
{
public:
	static ExtendedPtr create (void);
	virtual ~Extended();
	Extended& operator= (const Extended& c);
	Extended& operator= (Extended&& c);

	void swap (Extended& c);
	friend void swap (Extended& lhs, Extended& rhs);

	virtual bool accept (Visitor& v);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

	static ContainerPtr probe (ContainerPtr& parent, std::uint8_t* buffer, std::uint64_t bufsize);
	virtual std::uint8_t* get_buffer (std::uint64_t offset, std::uint64_t size);

public:
	// properties

protected:
	Extended (void);
	Extended (const Extended& c);
	Extended (Extended&& c);

	virtual Extended* clone (void);
};

#endif // _EXTENDED_H_

