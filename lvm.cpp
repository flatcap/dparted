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
	iprintf (indent, "\e[34m%s\e[0m\n", device.c_str());

	Container::Dump (indent);
}

