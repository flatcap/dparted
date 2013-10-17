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

#include "probe.h"
#include "log_trace.h"

/**
 * Probe
 */
Probe::Probe()
{
}

/**
 * ~Probe
 */
Probe::~Probe()
{
}


/**
 * initialise
 */
void
initialise (void)
{
}

/**
 * shutdown
 */
void
Probe::shutdown (void)
{
}

/**
 * discover
 */
void
Probe::discover (std::queue<DPContainer*> &probe_queue)
{
}

/**
 * get_name
 */
std::string
Probe::get_name (void)
{
	return name;
}

/**
 * get_description
 */
std::string
Probe::get_description (void)
{
	return description;
}


