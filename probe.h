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

#ifndef _PROBE_H_
#define _PROBE_H_

#include <string>
#include <vector>
#include <set>
#include <queue>

#include "disk.h"

class DPContainer;

/**
 * functor compare
 */
struct compare_d {
	bool operator() (DPContainer *a, DPContainer *b)
	{
		Block *d1 = dynamic_cast<Block*> (a);
		Block *d2 = dynamic_cast<Block*> (b);

		return ((d1->kernel_major*256+d1->kernel_minor) < (d2->kernel_major*256+d2->kernel_minor));
	}
};


/**
 * class Probe
 */
class Probe
{
public:
	Probe();
	virtual ~Probe();

	static  void initialise (void);
	        void shutdown   (void);
	virtual void discover   (std::queue<DPContainer*> &probe_queue);

	std::string get_name        (void);
	std::string get_description (void);

	std::vector<DPContainer*> get_children (void);

protected:
	std::string name;
	std::string description;

	std::set<DPContainer *,compare_d> children;
};


#endif // _PROBE_H_

