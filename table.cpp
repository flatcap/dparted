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
#include <string>

#include "gpt.h"
#include "identify.h"
#include "log.h"
#include "msdos.h"
#include "table.h"

/**
 * Table
 */
Table::Table (void)
{
	declare ("table", "red");
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
bool Table::probe (DPContainer *parent, unsigned char *buffer, int bufsize)
{
	return (Gpt::probe (parent, buffer, bufsize) ||
		Msdos::probe (parent, buffer, bufsize));
}


/**
 * dump_dot
 */
std::string Table::dump_dot (void)
{
	std::ostringstream output;

	// no specifics for now

	output << DPContainer::dump_dot();

	return output.str();
}


