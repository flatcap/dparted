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
#include <cstring>

#include "log.h"
#include "lvm_table.h"
#include "utils.h"
#include "log_trace.h"
#include "main.h"
#include "lvm_group.h"

/**
 * LVMTable
 */
LVMTable::LVMTable (void)
{
	declare ("lvm_table");
}

/**
 * ~LVMTable
 */
LVMTable::~LVMTable()
{
}


/**
 * read_uuid_string
 */
std::string read_uuid_string (unsigned char *buffer)
{
	char text[33];
	std::string dashes;

	memcpy (text, buffer, sizeof (text) - 1);
	text[sizeof (text) - 1] = 0;

	dashes = text;

	dashes = dashes.substr(0,6) + "-" +
		 dashes.substr(6,4) + "-" +
		 dashes.substr(10,4) + "-" +
		 dashes.substr(14,4) + "-" +
		 dashes.substr(18,4) + "-" +
		 dashes.substr(22,4) + "-" +
		 dashes.substr(26,6);
	return dashes;
}

/**
 * probe
 */
DPContainer * LVMTable::probe (DPContainer &top_level, DPContainer *parent, unsigned char *buffer, int bufsize)
{
	//LOG_TRACE;
	LVMTable *t = NULL;

	if (strncmp ((const char*) buffer+512, "LABELONE", 8))
		return NULL;

	if (strncmp ((const char*) buffer+536, "LVM2 001", 8))
		return NULL;

	t = new LVMTable();
	//log_debug ("new LVMTable (%p)\n", t);

	t->uuid = read_uuid_string (buffer+544);

	//log_info ("table: %s\n", t->uuid.c_str());

	const int bufsize2 = 4096;
	std::vector<unsigned char> buffer2 (bufsize2);

	parent->open_device();
	parent->read_data (4608, bufsize2, &buffer2[0]);	// check read num

	//dump_hex2 (&buffer2[0], 0, bufsize2);

	auto iter = buffer2.begin();
	for (;; iter++) {
		if (*iter == ' ')
			break;
	}

	std::string v(buffer2.begin(), iter);
	//log_error ("vol name = '%s'\n", v.c_str());
	t->vol_name = v;
#if 0
	std::string config (buffer2.begin(), buffer2.end());
	printf ("config = \n%s\n", buffer2.data());
#endif

	parent->add_child (t);
	//queue_add_probe (t);

	LVMGroup::discover (top_level, t);

	return t;
}


