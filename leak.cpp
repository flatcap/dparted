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

#include <cstdio>
#include <unordered_set>

#include "container.h"
#include "log.h"
#include "log_trace.h"

std::unordered_set<DPContainer*> obj_set;

/**
 * dump_leaks
 */
std::size_t
dump_leaks (void)
{
	if (obj_set.size() == 0)
		return 0;

	log_debug ("Leaks (%lu):\n", obj_set.size());
	for (auto c : obj_set) {
		log_debug ("\t0x%p - %s, %s, %d\n", (void*) c, c->name.c_str(), c->device.c_str(), c->ref_count);
	}

	return obj_set.size();
}

/**
 * object_track
 */
DPContainer *
object_track (DPContainer *obj)
{
	obj_set.insert (obj);
	return obj;
}

/**
 * object_untrack
 */
bool
object_untrack (DPContainer *obj)
{
	//XXX check for presence
	obj_set.erase (obj);
	return true;

}


/**
 * main_pre
 */
__attribute__((constructor))
void
main_pre (void)
{
	//LOG_TRACE;
}

/**
 * main_post
 */
__attribute__((destructor))
void
main_post (void)
{
	//LOG_TRACE;
	dump_leaks();
}

