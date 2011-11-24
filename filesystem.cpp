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

/**
 * Filesystem
 */
Filesystem::Filesystem (void)
{
	size = -1;
	used = -1;
}

/**
 * ~Filesystem
 */
Filesystem::~Filesystem()
{
}

/**
 * Dump
 */
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
#if 0
		Filesystem volume name:   fedora-16
		Last mounted on:          /
		Filesystem UUID:          17d021dd-64dd-4ff6-ab40-97d24407f27e
		Inode count:              1310720
		Block count:              5242880
		Reserved block count:     0
		Free blocks:              2294731
		Free inodes:              1076315
		First block:              0
		Block size:               4096
		Fragment size:            4096
		Journal size:             128M

		2294731*4096

		/dev/sda1      21137846272 8467374080 12670472192  41% /

		fs total size		21474836480
		df total size		21137846272 (128M journal + 60M inodes + 133M other)

		used			8467374080
		free			12670472192
		total			21137846272
#endif
	} else if (type == "linux-swap(v1)") {
		command = "cat /proc/meminfo";
		execute_command (command, output, error);
		unsigned int index = output.find ("SwapTotal:");
		unsigned int end   = output.find ("\n", index);
		//printf ("index = %d\n", index);
		//printf ("end = %d\n", end);
		std::string line = output.substr (index, end - index);
		unsigned int off_num = line.find_first_of ("0123456789");
		//printf ("off_num = %u\n", off_num);
		//printf ("rest = %s\n", line.substr (off_num).c_str());
		//iprintf (indent+8, "line = %s\n", line.c_str());
		line = line.substr (off_num);
		sscanf (line.c_str(), "%Ld kB", &size);
		size *= 1024;

		long long free = -1;
		index = output.find ("SwapFree:");
		end   = output.find ("\n", index);
		//printf ("index = %d\n", index);
		//printf ("end = %d\n", end);
		line = output.substr (index, end - index);
		off_num = line.find_first_of ("0123456789");
		//printf ("off_num = %u\n", off_num);
		//printf ("rest = %s\n", line.substr (off_num).c_str());
		//iprintf (indent+8, "line = %s\n", line.c_str());
		line = line.substr (off_num);
		sscanf (line.c_str(), "%Ld kB", &free);
		free *= 1024;
		used = size - free;

		// WRONG WRONG WRONG - could have multiple swap devices
#if 0
		SwapCached:        20472 kB
		SwapTotal:       7164952 kB
		SwapFree:        6972292 kB


		Filename  Type      Size    Used Priority
		/dev/sdc1 partition 7164952 440  -1
#endif
	} else if (type == "ntfs") {
		command = "ntfsresize -P -i -f -v " + part->device;
		command += '0' + part->num;
#if 0
		Device name        : /dev/sda7
		Cluster size       : 4096 bytes
		Current volume size: 193651012096 bytes (193652 MB)
		Current device size: 193651015680 bytes (193652 MB)
		Space in use       : 74 MB (0.0%)
		You might resize at 73482240 bytes or 74 MB (freeing 193578 MB).
#endif
	} else if (type == "fat32") {
		command = "dosfsck -n -v " + part->device;
		command += '0' + part->num;
#if 0
		512 bytes per logical sector
		16384 bytes per cluster
			32 reserved sectors
		First FAT starts at byte 16384 (sector 32)
			2 FATs, 32 bit entries
		34504704 bytes per FAT (= 67392 sectors)
		Root directory start at cluster 2 (arbitrary size)
		Data area starts at byte 69025792 (sector 134816)
		8625355 data clusters (141317816320 bytes)
		63 sectors/track, 255 heads
		276146176 sectors total
		/dev/sda6: 1 files, 262145/8625355 clusters
#endif
	}

#if 0
	df -B1 /dev/sda7
	Filesystem        1B-blocks      Used    Available Use% Mounted on
	/dev/sda7      193651011584 479731712 193171279872   1% /mnt/vg/junk/robert

#endif

	//result = execute_command (command, output, error);

	iprintf (indent,   "\e[33m%s\e[0m\n", type.c_str());
	iprintf (indent+8, "Size: %lld\n",  size);
	iprintf (indent+8, "Used: %lld\n",  used);
	iprintf (indent+8, "Command: %s\n", command.c_str());

	//iprintf (indent+8, "Type: %s\n", type.c_str());

	Container::Dump (indent);
}

