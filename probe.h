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
	virtual void discover   (void);

	std::string get_name        (void);
	std::string get_description (void);

protected:
	std::string name;
	std::string description;
};


#endif // _PROBE_H_

