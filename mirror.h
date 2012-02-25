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


#ifndef _MIRROR_H_
#define _MIRROR_H_

#include <string>
#include <vector>

#include "volume.h"

class Mirror : public Volume
{
public:
	Mirror (void);
	virtual ~Mirror();

	virtual std::string dump_dot (void);

protected:
	//XXX need custom AddChild()
	// children are this.bytes_size
	// num of children
	//	stripes - LVM2_STRIPES
	// num of segments = 0
	// Linear *log_file;

private:

};

#endif /* _MIRROR_H_ */

