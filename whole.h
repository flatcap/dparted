/* Copyright (c) 2011 Richard Russon (FlatCap)
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


#ifndef _WHOLE_H_
#define _WHOLE_H_

#include <string>

#include "container.h"

class Segment;

class Whole : public Container
{
public:
	Whole (void);
	virtual ~Whole();

	virtual std::string dump_dot (void);

	std::vector<Segment*> segments;

protected:

private:

};

#endif /* _WHOLE_H_ */
