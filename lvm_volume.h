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

#ifndef _LVM_VOLUME_H_
#define _LVM_VOLUME_H_

#include "volume.h"

class LVMTable;

/**
 * class LVMVolume
 */
class LVMVolume : public Volume
{
public:
	LVMVolume (void);
	virtual ~LVMVolume();

	virtual std::string dump_dot (void);

	std::string	lv_attr;
	long		kernel_major;
	long		kernel_minor;

	virtual int read_data (long long offset, long long size, unsigned char *buffer);

protected:
	std::vector<LVMTable*> tables;

private:

};

#endif // _LVM_VOLUME_H_

