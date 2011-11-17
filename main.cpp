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


#include <iostream>
#include <vector>
#include <glibmm/ustring.h>

#include "container.h"
#include "device.h"
#include "filesystem.h"
#include "lvm.h"
#include "partition.h"

#include "utils.h"

/**
 * run_command
 */
void run_command (void)
{
	Glib::ustring output;
	Glib::ustring error;

	if (execute_command("vgs --units=b --nosuffix --noheading -a", output, error))
		return;

	if (output.size() == 0)
		return;

	std::cout << output;
	/*
	std::vector<Glib::ustring> temp_arr;
	Utils::tokenize(output, temp_arr, "\n");
	for (unsigned int k = 0; k < temp_arr.size(); k++)
	{
		Glib::ustring temp = Utils::regexp_label(output, "^[^/]*(/dev/[^\t ]*)");
		if (temp.size() > 0)
			swraid_devices.push_back(temp);
	}
	*/
}

/**
 * main
 */
int main (int argc, char *argv[])
{
	return 0;
}
