/* Copyright (c) 2011-2012 Richard Russon (FlatCap)
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


#include <stdio.h>

#include <string>

#include "table.h"
#include "identify.h"
#include "gpt.h"
#include "msdos.h"

/**
 * Table
 */
Table::Table (void)
{
	type = "table";
}

/**
 * ~Table
 */
Table::~Table()
{
}


/**
 * probe
 */
Table * Table::probe (Container *parent, unsigned char *buffer, int bufsize)
{
	Table *result = NULL;

	if ((result = Gpt::probe (parent, buffer, bufsize))) {
		// do nothing
	} else if (identify_msdos (buffer, bufsize)) {
		printf ("\tmsdos\n");
		result = new Msdos;
	}

	return result;
}

