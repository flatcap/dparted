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

#include <cstring>
#include <sstream>

#include "log.h"
#include "main.h"
#include "misc.h"
#include "partition.h"
#include "utils.h"
#include "log_trace.h"

/**
 * Misc
 */
Misc::Misc (void)
{
	declare ("misc");
}

/**
 * ~Misc
 */
Misc::~Misc()
{
}


/**
 * is_empty
 */
static bool
is_empty (unsigned char *buffer, int bufsize)
{
	for (int i = 0; i < bufsize; i++) {
		if (buffer[i]) {
			return false;
		}
	}

	return true;
}

/**
 * is_luks
 */
static bool
is_luks (unsigned char *buffer, int bufsize)
{
	const char *signature = "LUKS\xBA\xBE";

	if (strncmp ((const char *) buffer, signature, strlen (signature)))
		return false;
#if 1
	log_info ("LUKS:\n");
	log_info ("\tversion:     %d\n", *(short int*) (buffer+6));
	log_info ("\tcipher name: %s\n", buffer+8);
	log_info ("\tcipher mode: %s\n", buffer+40);
	log_info ("\thash spec:   %s\n", buffer+72);
	log_info ("\tuuid:        %s\n", buffer+168);
#endif
	return true;
}

/**
 * is_random
 */
static bool
is_random (unsigned char *buffer, int bufsize)
{
	double mean = 0;

	if (bufsize > 4096)
		bufsize = 4096;

	for (int i = 0; i < bufsize; i++) {
		mean += buffer[i];
	}

	mean /= bufsize;

	//log_debug ("data mean = %0.6f\n", mean);

	return ((mean > 125) && (mean < 130));
}


/**
 * probe
 */
DPContainer *
Misc::probe (DPContainer &top_level, DPContainer *parent, unsigned char *buffer, int bufsize)
{
	//LOG_TRACE;
	Misc *m = nullptr;

	if (is_empty (buffer, bufsize)) {
		//log_error ("probe empty\n");
		m = new Misc();
		m->name = "zero";
	} else if (is_luks (buffer, bufsize)) {
		//log_error ("probe luks\n");
		m = new Misc();
		m->name = "luks";
	} else if (is_random (buffer, bufsize)) {
		//log_error ("probe random\n");
		m = new Misc();
		m->name = "random";
	}

	if (m) {
		parent->add_child (m);

		m->bytes_size = parent->bytes_size;
		m->bytes_used = 0;
		m->parent_offset = 0;
	}

	return m;
}


