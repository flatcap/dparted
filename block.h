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

#ifndef _BLOCK_H_
#define _BLOCK_H_

#include <memory>
#include <string>
#include <vector>

#include "container.h"

typedef std::shared_ptr<class Block> BlockPtr;

/**
 * class Block - Mass storage device
 */
class Block : public Container
{
public:
	static BlockPtr create (void);
	virtual ~Block();

	Block& operator= (const Block& c);
	Block& operator= (Block&& c);

	void swap (Block& c);
	friend void swap (Block& lhs, Block& rhs);

	virtual bool accept (Visitor& v);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);
	virtual bool is_resizeable (void);

public:
	// properties

protected:
	Block (void);
	Block (const Block& c);
	Block (Block&& c);

	virtual Block* clone (void);
};

#endif // _BLOCK_H_

