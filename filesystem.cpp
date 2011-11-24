/* Copyright (c) 2011 Richard Russon (FlatCap)
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


#include <stdio.h>
#include <string>

#include "filesystem.h"
#include "partition.h"
#include "utils.h"

Filesystem::Filesystem (void)
{
	size = -1;
	used = -1;
}

Filesystem::~Filesystem()
{
}

void Filesystem::Dump (int indent /* = 0 */)
{
	std::string space = "                                                                ";

	space = space.substr (0, indent);

	std::string command;
	std::string output;
	std::string error;
	//int result = -1;

	if ((type == "ext4") || (type == "ext3") || (type == "ext2")) {
		command = "dump2efs -h " + part->device;
		command += '0' + part->num;
	} else if (type == "linux-swap(v1)") {
		command = "cat /proc/meminfo";
	} else if (type == "ntfs") {
		command = "ntfsresize -P -i -f -v " + part->device;
		command += '0' + part->num;
	} else if (type == "fat32") {
		command = "dosfsck -n -v " + part->device;
		command += '0' + part->num;
	}

	//result = execute_command (command, output, error);

	iprintf (indent,   "\e[33m%s\e[0m\n", type.c_str());
	iprintf (indent+8, "Size: %lld\n",  size);
	iprintf (indent+8, "Used: %lld\n",  used);
	iprintf (indent+8, "Command: %s\n", command.c_str());

	//iprintf (indent+8, "Type: %s\n", type.c_str());

	Container::Dump (indent);
}

