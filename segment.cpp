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

#include "segment.h"
#include "container.h"

/**
 * Segment
 */
Segment::Segment (void) :
	whole (NULL)
{
	type = "segment";
}

/**
 * ~Segment
 */
Segment::~Segment()
{
}


/**
 * dump
 */
void Segment::dump (int indent /* = 0 */)
{
	Container::dump (indent);
}

/**
 * dump_csv
 */
void Segment::dump_csv (void)
{
	Container::dump_csv();
}

/**
 * dump_dot
 */
std::string Segment::dump_dot (void)
{
	std::ostringstream output;

	output << dump_table_header ("Segment", "magenta");

	output << Container::dump_dot();

	output << dump_row ("whole", whole);

	output << dump_table_footer();

#if 1
	// now iterate through all the children
	for (std::vector<Container*>::iterator i = children.begin(); i != children.end(); i++) {
		output << "\n";
		output << (*i)->dump_dot();
		output << "obj_" << this << " -> " << "obj_" << (*i) << ";\n";
		output << "\n";
	}
#endif

	return output.str();
}
