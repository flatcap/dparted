/* Copyright (c) 2012 Richard Russon (FlatCap)
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
#include <sstream>

#include "filesystem.h"
#include "partition.h"
#include "utils.h"
#include "identify.h"
#include "stringnum.h"

/**
 * Filesystem
 */
Filesystem::Filesystem (void)
{
	type = "filesystem";
}

/**
 * ~Filesystem
 */
Filesystem::~Filesystem()
{
}


/**
 * ext2_get_usage
 */
long long Filesystem::ext2_get_usage (void)
{
	std::ostringstream build;
	std::string dev = device;
	std::string command;
	std::string output;
	std::string error;
	long block_size = 512;
	long long bfree = 0;

	if (device_offset != 0) {
		//printf ("create loop device\n");
		build << "losetup /dev/loop0 " << device << " -o " << device_offset;
		command = build.str();
		execute_command (command, output, error);
		//printf ("command = %s\n", command.c_str());
		//execute_command ("losetup /dev/loop0 ", output, error);
		dev = "/dev/loop0";
	}

	// do something
	command = "tune2fs -l " + dev;
	//printf ("command = %s\n", command.c_str());

	execute_command (command, output, error);
	//printf ("result = \n%s\n", output.c_str());

	//interpret results
	std::string tmp;
	size_t pos1 = std::string::npos;
	size_t pos2 = std::string::npos;

	//printf ("volume:\n");

	pos1 = output.find ("Filesystem volume name:");
	if (pos1 != std::string::npos) {
		pos1 = output.find_first_not_of (" ", pos1 + 23);
		pos2 = output.find_first_of ("\n\r", pos1);

		tmp = output.substr (pos1, pos2 - pos1);
		//printf ("\tname = '%s'\n", tmp.c_str());
	}

	pos1 = output.find ("Filesystem UUID:");
	if (pos1 != std::string::npos) {
		pos1 = output.find_first_not_of (" ", pos1 + 16);
		pos2 = output.find_first_of ("\n\r", pos1);

		tmp = output.substr (pos1, pos2 - pos1);
		//printf ("\tuuid = '%s'\n", tmp.c_str());
	}

	pos1 = output.find ("Block size:");
	if (pos1 != std::string::npos) {
		pos1 = output.find_first_not_of (" ", pos1 + 11);
		pos2 = output.find_first_of ("\n\r", pos1);

		tmp = output.substr (pos1, pos2 - pos1);
		StringNum s (tmp.c_str());
		block_size = s;
		//printf ("\tblock size = %ld\n", block_size);
	}

	pos1 = output.find ("Block count:");
	if (pos1 != std::string::npos) {
		pos1 = output.find_first_not_of (" ", pos1 + 12);
		pos2 = output.find_first_of ("\n\r", pos1);

		tmp = output.substr (pos1, pos2 - pos1);
		//printf ("\tblock count = %s\n", tmp.c_str());
	}

	pos1 = output.find ("Free blocks:");
	if (pos1 != std::string::npos) {
		pos1 = output.find_first_not_of (" ", pos1 + 12);
		pos2 = output.find_first_of ("\n\r", pos1);

		tmp = output.substr (pos1, pos2 - pos1);
		StringNum s (tmp.c_str());
		bfree = (long long) s * block_size;
		//printf ("\tfree blocks = %s\n", tmp.c_str());
		//printf ("\tbytes free = %lld\n", bfree);
	}

	if (device_offset != 0) {
		command = "losetup -d /dev/loop0";
		execute_command (command, output, error);
		//printf ("dismantle loop device\n");
		//printf ("command = %s\n", command.c_str());
		//printf ("\n");
	}

	return bfree;
}


/**
 * probe
 */
Filesystem * Filesystem::probe (Container *parent, unsigned char *buffer, int bufsize)
{
	Filesystem *f = NULL;
	std::string name;

	if (identify_btrfs (buffer, bufsize)) {
		name = "btrfs";
	} else if (identify_ext2 (buffer, bufsize)) {
		name = "ext2";
	} else if (identify_ntfs (buffer, bufsize)) {
		name = "ntfs";
	} else if (identify_reiserfs (buffer, bufsize)) {
		name = "reiserfs";
	} else if (identify_swap (buffer, bufsize)) {
		name = "swap";
	} else if (identify_vfat (buffer, bufsize)) {
		name = "vfat";
	} else if (identify_xfs (buffer, bufsize)) {
		name = "xfs";
	}

	//printf ("NAME = %s\n", name.c_str());
	if (!name.empty()) {
		f = new Filesystem;
		f->name = name;
		f->device = parent->device;
		f->device_offset = parent->device_offset;
		f->bytes_size = parent->bytes_size;
		if (name == "ext2") {
			long long bfree = f->ext2_get_usage();
			f->bytes_used = f->bytes_size - bfree;
		} else {
			f->bytes_used = parent->bytes_size;
		}

		parent->add_child (f);
	}

	return f;
}


/**
 * dump
 */
void Filesystem::dump (int indent /* = 0 */)
{
	std::string command;
	std::string output;
	std::string error;
	unsigned int index = -1;
	//int result = -1;

	//fprintf (stderr, "%s: %s\n", __PRETTY_FUNCTION__, name.c_str());
	if ((name == "ext4") || (name == "ext3") || (name == "ext2")) {
		long long free = 0;

		command = "dumpe2fs -h " + parent->device;
		//command += '0' + part->num;
		//fprintf (stderr, "device = %s, command = %s\n", parent->device.c_str(), command.c_str());
		execute_command (command, output, error);

		//fprintf (stderr, "command = %s\n", command.c_str());
		//fprintf (stderr, "%s\n", output.c_str());

		index = output.find ("Filesystem UUID:") + 16;
		uuid = extract_bare_string (output, index);
		//fprintf (stderr, "uuid = %s\n", uuid.c_str());

		index = output.find ("Block size:");
		block_size = extract_number (output, index);

		index = output.find ("Block count:");
		bytes_size = block_size * extract_number (output, index);

		index = output.find ("Free blocks:");
		free = block_size * extract_number (output, index);
		bytes_used = bytes_size - free;

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
	} else if (name == "linux-swap(v1)") {
		// Block size 1024?

		command = "cat /proc/swaps";
		execute_command (command, output, error);

		index = output.find ("/dev/sdc1") + 10; //strlen ("/dev/sdc1")
		bytes_size = 1024 * extract_number (output, index);
		bytes_used = 1024 * extract_number (output, index);

	} else if (name == "ntfs") {
		//command = "ntfsresize -P -i -f -v " + part->device;
		command = "df -B1 " + parent->device;
		//RAR command += '0' + parent->num;
		execute_command (command, output, error);

		index = output.find ("/dev/sda7") + 10; //strlen ("/dev/sda7")
		bytes_size = extract_number (output, index);
		bytes_used = extract_number (output, index);

#if 0
		Device name        : /dev/sda7
		Cluster size       : 4096 bytes
		Current volume size: 193651012096 bytes (193652 MB)
		Current device size: 193651015680 bytes (193652 MB)
		Space in use       : 74 MB (0.0%)
		You might resize at 73482240 bytes or 74 MB (freeing 193578 MB).
#endif
	} else if (name == "fat32") {
		command = "dosfsck -n -v " + parent->device;
		//RAR command += '0' + parent->num;
		execute_command (command, output, error);

		index = output.find ("bytes per cluster") - 12;
		block_size = extract_number (output, index);

		index = output.find ("data clusters");
		bytes_size = extract_number (output, index);

		index = output.find ("files,");
		bytes_used = block_size * extract_number (output, index);

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

#if 0
	std::string size = get_size (bytes_size);
	std::string used = get_size (bytes_used);

	iprintf (indent,   "%s\n", name.c_str());
	iprintf (indent+8, "Size: %s\n",  size.c_str());
	iprintf (indent+8, "Used: %s\n",  used.c_str());
	//iprintf (indent+8, "Command: %s\n", command.c_str());

	//iprintf (indent+8, "Type: %s\n", type.c_str());
#endif

	Container::dump (indent);
}

/**
 * dump_csv
 */
void Filesystem::dump_csv (void)
{
	printf ("%s,%s,%s,%ld,%s,%s,%lld,%lld,%lld\n",
		"Filesystem",
		device.c_str(),
		name.c_str(),
		block_size,
		label.c_str(),
		uuid.c_str(),
		bytes_size,
		bytes_used,
		bytes_size - bytes_used);
	Container::dump_csv();
}

/**
 * dump_dot
 */
std::string Filesystem::dump_dot (void)
{
	std::ostringstream output;

	output << dump_table_header ("Filesystem", "cyan");

	output << Container::dump_dot();

	output << dump_row ("label", label);

	output << dump_table_footer();

	return output.str();
}
