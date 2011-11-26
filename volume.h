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


#ifndef _VOLUME_H_
#define _VOLUME_H_

#include <string>

#include "container.h"

class Volume : public Container
{
public:
	Volume (void);
	virtual ~Volume();

	virtual void Dump (int indent = 0);

	std::string	lv_name;
	std::string	lv_attr;
	long long	lv_size;
	std::string	lv_path;
	int		kernel_major;
	int		kernel_minor;

protected:

private:

};

#endif /* _VOLUME_H_ */

