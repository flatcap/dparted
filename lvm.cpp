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


#include <string>

#include "lvm.h"
#include "utils.h"

/**
 * LVM
 */
LVM::LVM (void)
{
}

/**
 * ~LVM
 */
LVM::~LVM()
{
}

/**
 * Dump
 */
void LVM::Dump (int indent /* = 0 */)
{
	std::string size = get_size (vg_size);

	iprintf (indent, "\e[35m%s\e[0m (%s)\n", vg_name.c_str(), size.c_str());

	//iprintf (indent+8, "vg_name:         %s\n",   vg_name.c_str());
	iprintf (indent+8, "pv_count:        %d\n",   pv_count);
	iprintf (indent+8, "lv_count:        %d\n",   lv_count);
	iprintf (indent+8, "vg_attr:         %s\n",   vg_attr.c_str());
	//iprintf (indent+8, "vg_size:         %lld\n", vg_size);
	iprintf (indent+8, "vg_free:         %lld\n", vg_free);
	iprintf (indent+8, "vg_uuid:         %s\n",   vg_uuid.c_str());
	//iprintf (indent+8, "vg_extent_size:  %lld\n", vg_extent_size);
	//iprintf (indent+8, "vg_extent_count: %lld\n", vg_extent_count);
	//iprintf (indent+8, "vg_free_count:   %lld\n", vg_free_count);
	//iprintf (indent+8, "vg_seqno:        %ld\n",  vg_seqno);

	Container::Dump (indent);
}

