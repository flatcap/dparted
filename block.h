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

#ifndef _BLOCK_H_
#define _BLOCK_H_

#include <string>
#include <memory>

#include "container.h"

class Block;
class Visitor;

typedef std::shared_ptr<Block> BlockPtr;

/**
 * class Block
 */
class Block : public Container
{
public:
	virtual ~Block() = default;
	static BlockPtr create (void);
	virtual bool accept (Visitor& v);

public:
	int kernel_major = 0;	//XXX rename device major
	int kernel_minor = 0;

protected:
	Block (void);

private:

};


#endif // _BLOCK_H_

