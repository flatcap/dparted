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


#include <string>
#include <vector>
#include <cstring>
#include <queue>

#include "probe_file.h"
#include "file.h"

#include "utils.h"
#include "log.h"
#include "log_trace.h"

static ProbeFile *prober = NULL;

/**
 * ProbeFile
 */
ProbeFile::ProbeFile()
{
	name = "file",
	description = "Files";
}

/**
 * ~ProbeFile
 */
ProbeFile::~ProbeFile()
{
}


/**
 * shutdown
 */
void ProbeFile::shutdown (void)
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
ProbeFile::discover (DPContainer &top_level, std::queue<DPContainer*> &probe_queue)
{
	//LOG_TRACE;

	// Config
	//	[files]
	//	dir = ~/vm
	//	file = /tmp/test.img
	//	suffix = img

	// iterate through config
	//	file => add that file
	//	dir  => add dir/*.img
}

/**
 * identify
 */
void
ProbeFile::identify (DPContainer &top_level, const char *name, int fd, struct stat &st)
{
	//LOG_TRACE;

	/* dir:
	 *	Search for files: *.img
	 * file:
	 *	We create, and manage, a loop device for this file
	 */
}

