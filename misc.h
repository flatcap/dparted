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

#ifndef _MISC_H_
#define _MISC_H_

#include <memory>
#include <string>
#include <vector>

#include "container.h"

typedef std::shared_ptr<class Misc> MiscPtr;

class Misc : public Container
{
public:
	static MiscPtr create (void);
	virtual ~Misc();
	Misc& operator= (const Misc& c);
	Misc& operator= (Misc&& c);

	void swap (Misc& c);
	friend void swap (Misc& lhs, Misc& rhs);

	MiscPtr copy (void);

	virtual bool accept (Visitor& v);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

	static bool probe (ContainerPtr& parent, std::uint8_t* buffer, std::uint64_t bufsize);

public:
	// properties

protected:
	Misc (void);
	Misc (const Misc& c);
	Misc (Misc&& c);

	virtual Misc* clone (void);
};

#endif // _MISC_H_

