/* Copyright (c) 2012 Richard Russon (FlatCap)
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


#include <sstream>

#include "lvm_mirror.h"

/**
 * LVMMirror
 */
LVMMirror::LVMMirror (void)
{
	declare ("lvm_mirror");
}

/**
 * ~LVMMirror
 */
LVMMirror::~LVMMirror()
{
}


/**
 * dump_dot
 */
std::string LVMMirror::dump_dot (void)
{
	std::ostringstream output;

	// no specifics for now

	output << LVMVolume::dump_dot();

	return output.str();
}


