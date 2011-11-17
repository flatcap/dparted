/* Copyright (c) 2011 Richard Russon (FlatCap)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <glibmm/ustring.h>
#include <glibmm/spawn.h>

#include "container.h"
#include "device.h"
#include "lvm.h"
#include "partition.h"

/**
 * execute_command
 */
int execute_command (const Glib::ustring &command, Glib::ustring &output, Glib::ustring &error)
{
	int exit_status = -1;
	std::string std_out;
	std::string std_error;

	try
	{
		std::vector<std::string>argv;
		argv.push_back ("sh");
		argv.push_back ("-c");
		argv.push_back ("sudo " + command);

		//Spawn command inheriting the parent's environment
		Glib::spawn_sync (".", argv, Glib::SPAWN_SEARCH_PATH, sigc::slot<void>(), &std_out, &std_error, &exit_status);
	}
	catch  (Glib::Exception & e)
	{
		 error = e.what();

		 return -1;
	}

	output = std_out;
	error = std_error;

	return exit_status;
}

/**
 * main
 */
int main (int argc, char *argv[])
{
	Glib::ustring output;
	Glib::ustring error;

	if (execute_command("vgs --units=b --nosuffix --noheading -a", output, error))
		return 1;

	if (output.size() == 0)
		return 1;

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

	return 0;
}
