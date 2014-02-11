/* Copyright (c) 2014 Richard Russon (FlatCap)
 *
 * This file is part of DParted.
 *
 * DParted is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DParted is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DParted.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <sstream>
#include <string>

#include "log.h"
#include "whole.h"
#include "log_trace.h"
#include "visitor.h"

/**
 * Whole
 */
Whole::Whole (void)
{
	const char* me = "Whole";

	sub_type (me);
}

/**
 * create
 */
WholePtr
Whole::create (void)
{
	WholePtr w (new Whole());
	w->weak = w;

	return w;
}


/**
 * accept
 */
bool
Whole::accept (Visitor& v)
{
	WholePtr w = std::dynamic_pointer_cast<Whole> (get_smart());
	if (!v.visit(w))
		return false;
	return visit_children(v);
}


/**
 * add_segment
 */
void
Whole::add_segment (ContainerPtr seg)
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

	seg->whole = get_smart();
}


