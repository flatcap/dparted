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

#include <sstream>
#include <string>

#include "log.h"
#include "whole.h"
#include "log_trace.h"

/**
 * Whole
 */
Whole::Whole (void)
{
	declare ("whole");
}

/**
 * ~Whole
 */
Whole::~Whole()
{
}


/**
 * add_segment
 */
void
Whole::add_segment (DPContainer *seg)
{
	bool inserted = false;

	for (auto i = segments.begin(); i != segments.end(); i++) {
		if ((*i) == seg)
			return;
		if ((*i)->parent_offset > seg->parent_offset) {
			segments.insert (i, seg);
			inserted = true;
			break;
		}
	}

	if (!inserted) {
		segments.push_back (seg);
	}

	seg->whole = this;
}


/**
 * get_children
 */
std::vector<DPContainer*>
Whole::get_children (void)
{
	log_info ("whole::get_children\n");
	if (children.empty() && whole)
		return whole->get_children();

	return children;
}

