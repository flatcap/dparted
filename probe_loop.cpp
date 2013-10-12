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


#include "probeloop.h"

static ProbeLoop *prober = NULL;

/**
 * ProbeLoop
 */
ProbeLoop::ProbeLoop()
{
}

/**
 * ~ProbeLoop
 */
ProbeLoop::~ProbeLoop()
{
}


/**
 * initialise
 */
void initialise (void)
{
	if (!prober) {
		prober = new ProbeLoop();
	}

	return prober;

}

/**
 * shutdown
 */
void ProbeLoop::shutdown (void)
{
	if (prober) {
		delete prober;
		prober = NULL;
	}
}

/**
 * discover
 */
void
ProbeLoop::discover (void)
{
}

/**
 * get_name
 */
std::string
ProbeLoop::get_name (void)
{
	return "";
}

/**
 * get_description
 */
std::string
ProbeLoop::get_description (void)
{
	return "";
}

