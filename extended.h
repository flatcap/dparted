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

#ifndef _EXTENDED_H
#define _EXTENDED_H

#include <string>

#include "msdos.h"
#include "pointers.h"

/**
 * class Extended
 */
class Extended : public Msdos
{
public:
	static ExtendedPtr create (void);

	static ExtendedPtr probe (ContainerPtr& top_level, ContainerPtr& parent, long offset, long size);

protected:
private:

};


#endif // _EXTENDED_H

