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


#include <stdio.h>
#include <string>
#include <sstream>

#include "mirror.h"
#include "utils.h"

/**
 * Mirror
 */
Mirror::Mirror (void)
{
	type = "mirror";
}

/**
 * ~Mirror
 */
Mirror::~Mirror()
{
}


/**
 * dump_dot
 */
std::string Mirror::dump_dot (void)
{
	std::ostringstream output;

	output << dump_table_header ("Mirror", "purple");

	output << Whole::dump_dot();

	output << dump_table_footer();
	output << dump_dot_children();

	return output.str();
}

