/* Copyright (c) 2014 Richard Russon (FlatCap)
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

#include <iostream>
#include <sstream>

#include "ext_fs.h"
#include "utils.h"
#include "log_trace.h"

/**
 * ExtFs
 */
ExtFs::ExtFs (void)
{
}

/**
 * create
 */
ExtFsPtr
ExtFs::create (void)
{
	ExtFsPtr e (new ExtFs());
	e->weak = e;

	return e;
}


/**
 * get_ext_header
 */
void
ExtFs::get_ext_header (ContainerPtr parent)
{
	if (!parent)
		return;

	std::string dev = parent->get_device_name();
	if (dev.empty())	//XXX shouldn't happen
		return;

	std::string command;
	std::string output;

	// do something
	command = "tune2fs -l " + dev;
	//log_debug ("command = %s\n", command.c_str());

	execute_command3 (command, output);
	//log_debug ("result = \n%s\n", output.c_str());

	//interpret results
	std::string tmp;
	size_t pos1 = std::string::npos;
	size_t pos2 = std::string::npos;

	//log_debug ("volume:\n");

	pos1 = output.find ("Filesystem volume name:");
	if (pos1 != std::string::npos) {
		pos1 = output.find_first_not_of (" ", pos1 + 23);
		pos2 = output.find_first_of ("\n\r", pos1);

		tmp = output.substr (pos1, pos2 - pos1);
		//log_debug ("\tname = '%s'\n", tmp.c_str());
		name = tmp;	//XXX could be "<none>" if there's no label
	}

	pos1 = output.find ("Filesystem UUID:");
	if (pos1 != std::string::npos) {
		pos1 = output.find_first_not_of (" ", pos1 + 16);
		pos2 = output.find_first_of ("\n\r", pos1);

		tmp = output.substr (pos1, pos2 - pos1);
		//log_debug ("\tuuid = '%s'\n", tmp.c_str());
		uuid = tmp;
	}

	pos1 = output.find ("Block size:");
	if (pos1 != std::string::npos) {
		pos1 = output.find_first_not_of (" ", pos1 + 11);
		pos2 = output.find_first_of ("\n\r", pos1);

		tmp = output.substr (pos1, pos2 - pos1);
		StringNum s (tmp);
		block_size = s;
		//log_debug ("\tblock size = %ld\n", block_size);
	}

	pos1 = output.find ("Block count:");
	if (pos1 != std::string::npos) {
		pos1 = output.find_first_not_of (" ", pos1 + 12);
		pos2 = output.find_first_of ("\n\r", pos1);

		tmp = output.substr (pos1, pos2 - pos1);
		//log_debug ("\tblock count = %s\n", tmp.c_str());
		StringNum s (tmp);
		bytes_size = s;
		bytes_size *= block_size;
	}

	pos1 = output.find ("Free blocks:");
	if (pos1 != std::string::npos) {
		pos1 = output.find_first_not_of (" ", pos1 + 12);
		pos2 = output.find_first_of ("\n\r", pos1);

		tmp = output.substr (pos1, pos2 - pos1);
		StringNum s (tmp);
		//log_debug ("\tfree blocks = %s\n", tmp.c_str());

		bytes_used = s;
		bytes_used *= block_size;
		bytes_used = bytes_size - bytes_used;
	}
}

/**
 * get_ext2
 */
ExtFsPtr
ExtFs::get_ext2 (ContainerPtr parent, unsigned char* buffer, int bufsize)
{
	bool b1 = (*(unsigned short int*) (buffer+0x438) == 0xEF53);	// Magic
	bool b2 = !(*(unsigned int*) (buffer + 0x45C) & 0x0000004);	// Journal

	ExtFsPtr e;
	if (b1 && b2) {
		e = create();
		e->sub_type ("ext2");
		e->get_ext_header (parent);
		e->get_mounted_usage (parent);
	}
	return e;
}

/**
 * get_ext3
 */
ExtFsPtr
ExtFs::get_ext3 (ContainerPtr parent, unsigned char* buffer, int bufsize)
{
	bool b1 = (*(unsigned short int*) (buffer+0x438) == 0xEF53);	// Magic
	bool b2 = (*(unsigned int*) (buffer + 0x45C) & 0x0000004);	// Journal
	bool b3 = (*(unsigned int*) (buffer + 0x460) < 0x0000040);
	bool b4 = (*(unsigned int*) (buffer + 0x464) < 0x0000008);

	ExtFsPtr e;
	if (b1 && b2 && b3 && b4) {
		e = create();
		e->sub_type ("ext3");
		e->get_ext_header (parent);
		e->get_mounted_usage (parent);
	}
	return e;
}

/**
 * get_ext4
 */
ExtFsPtr
ExtFs::get_ext4 (ContainerPtr parent, unsigned char* buffer, int bufsize)
{
	bool b1 = (*(unsigned short int*) (buffer+0x438) == 0xEF53);	// Magic
	bool b2 = (*(unsigned int*) (buffer + 0x45C) & 0x0000004);	// Journal
	bool b3 = (*(unsigned int*) (buffer + 0x460) < 0x0000040);
	bool b4 = (*(unsigned int*) (buffer + 0x460) > 0x000003f);
	bool b5 = (*(unsigned int*) (buffer + 0x464) > 0x0000007);

	ExtFsPtr e;
	if (b1 && b2 && ((b3 && b5) || b4)) {
		e = create();
		e->sub_type ("ext4");
		e->get_ext_header (parent);
		e->get_mounted_usage (parent);
	}
	return e;
}


