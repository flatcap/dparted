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
#include <string>

#include "app.h"

/**
 * App
 */
App::App (void)
{
}

/**
 * ~App
 */
App::~App()
{
}


/**
 * ask
 */
bool
App::ask (Question& q)
{
	std::cout << q.title << std::endl;
	std::cout << q.question << std::endl;
	std::cout << '\t';
	for (auto a : q.answers) {
		std::cout << a << " ";
	}
	std::cout << '\n';
	return false;
}

/**
 * notify
 */
bool
App::notify (Message& m)
{
	// Might need to queue these until we're ready to confront the user
	return false;
}


/**
 * add_config
 */
bool
App::add_config (const std::string& filename)
{
	ConfigFilePtr cf;

	cf = ConfigFile::read_file (filename);
	if (!cf) {
		//notify the user
		return false;
	}

	if (config_file) {
		//if modified ask user if they're sure
	}

	config_file = cf;
	//cf->dump_config();

	return true;
}

