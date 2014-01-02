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

#ifndef _CONFIG_FILE_H_
#define _CONFIG_FILE_H_

#include <string>
#include <memory>

#include <libconfig.h++>

#include "config.h"

class ConfigFile;

typedef std::shared_ptr<ConfigFile> ConfigFilePtr;

/**
 * class ConfigFile
 */
class ConfigFile
{
public:
	ConfigFile (void);
	virtual ~ConfigFile();

#if 0
	get_string
	get_bool
	get_u32
	get_u64
	get_double
#endif

protected:
	std::string filename;
	bool read_only = true;

	libconfig::Config cfg;
	ConfigPtr config;
};


#endif // _CONFIG_FILE_H_

