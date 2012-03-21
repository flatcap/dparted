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


#include <stdlib.h>

#include <string>
#include <sstream>

#include "filesystem.h"
#include "partition.h"
#include "utils.h"
#include "identify.h"
#include "stringnum.h"
#include "log.h"

/**
 * Filesystem
 */
Filesystem::Filesystem (void)
{
	type.push_back ("filesystem");
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
	long block_size = 512;
	long long bfree = 0;

	if (parent_offset != 0) {
		//log_debug ("create loop device\n");
		build << "losetup /dev/loop16 " << device << " -o " << parent_offset;
		command = build.str();
		execute_command (command, output);
		//log_debug ("command = %s\n", command.c_str());
		//execute_command ("losetup /dev/loop16 ", output);
		dev = "/dev/loop16";
	}

	if (dev.empty())	//XXX shouldn't happen
		return 0;

	// do something
	command = "tune2fs -l " + dev;
	//log_debug ("command = %s\n", command.c_str());

	execute_command (command, output);
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
	}

	pos1 = output.find ("Filesystem UUID:");
	if (pos1 != std::string::npos) {
		pos1 = output.find_first_not_of (" ", pos1 + 16);
		pos2 = output.find_first_of ("\n\r", pos1);

		tmp = output.substr (pos1, pos2 - pos1);
		//log_debug ("\tuuid = '%s'\n", tmp.c_str());
	}

	pos1 = output.find ("Block size:");
	if (pos1 != std::string::npos) {
		pos1 = output.find_first_not_of (" ", pos1 + 11);
		pos2 = output.find_first_of ("\n\r", pos1);

		tmp = output.substr (pos1, pos2 - pos1);
		StringNum s (tmp.c_str());
		block_size = s;
		//log_debug ("\tblock size = %ld\n", block_size);
	}

	pos1 = output.find ("Block count:");
	if (pos1 != std::string::npos) {
		pos1 = output.find_first_not_of (" ", pos1 + 12);
		pos2 = output.find_first_of ("\n\r", pos1);

		tmp = output.substr (pos1, pos2 - pos1);
		//log_debug ("\tblock count = %s\n", tmp.c_str());
	}

	pos1 = output.find ("Free blocks:");
	if (pos1 != std::string::npos) {
		pos1 = output.find_first_not_of (" ", pos1 + 12);
		pos2 = output.find_first_of ("\n\r", pos1);

		tmp = output.substr (pos1, pos2 - pos1);
		StringNum s (tmp.c_str());
		bfree = (long long) s * block_size;
		//log_debug ("\tfree blocks = %s\n", tmp.c_str());
		//log_debug ("\tbytes free = %lld\n", bfree);
	}

	if (parent_offset != 0) {
		command = "losetup -d /dev/loop16";
		execute_command (command, output);
		//log_debug ("dismantle loop device\n");
		//log_debug ("command = %s\n", command.c_str());
		//log_debug ("\n");
	}

	return bfree;
}


/**
 * probe
 */
bool Filesystem::probe (DPContainer *parent, unsigned char *buffer, int bufsize)
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

	//log_debug ("NAME = %s\n", name.c_str());
	if (!name.empty()) {
		f = new Filesystem;
		f->name = name;
		//f->device = parent->device;
		f->parent_offset = 0;
		f->bytes_size = parent->bytes_size;
		if (name == "ext2") {
			long long bfree = f->ext2_get_usage();
			f->bytes_used = f->bytes_size - bfree;
		} else {
			f->bytes_used = parent->bytes_size;
		}
		//log_info ("%s: size = %lld, used = %lld\n", f->name.c_str(), f->bytes_size, f->bytes_used);

		parent->add_child (f);

		f->bytes_used = (rand()%90) * f->bytes_size / 100; //RAR
	}

	return (f != NULL);
}


/**
 * dump_csv
 */
void Filesystem::dump_csv (void)
{
	log_debug ("%s,%s,%s,%ld,%s,%s,%lld,%lld,%lld\n",
		"Filesystem",
		device.c_str(),
		name.c_str(),
		block_size,
		label.c_str(),
		uuid.c_str(),
		bytes_size,
		bytes_used,
		bytes_size - bytes_used);
	DPContainer::dump_csv();
}

/**
 * dump_dot
 */
std::string Filesystem::dump_dot (void)
{
	std::ostringstream output;

	output << dump_table_header ("Filesystem", "cyan");

	output << DPContainer::dump_dot();

	output << dump_row ("label", label);

	output << dump_table_footer();

	return output.str();
}
