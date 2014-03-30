/* Copyright (c) 2014 Richard Russon (FlatCap)
 *
 * This file is part of DParted.
 *
 * DParted is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DParted is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DParted.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _CONFIG_FILE_H_
#define _CONFIG_FILE_H_

#include <string>
#include <memory>
#include <map>
#include <vector>

#include "config.h"

class ConfigFile;

typedef std::shared_ptr<ConfigFile> ConfigFilePtr;

/**
 * class ConfigFile - Manages a bunch of nested strings
 */
class ConfigFile
{
public:
	ConfigFile (void);
	virtual ~ConfigFile();
	static ConfigFilePtr create (void);

	static ConfigFilePtr read_file (const std::string& filename);

	bool exists (const std::string& name);

	bool                     get_bool    (const std::string& name);
	uint32_t                 get_u32     (const std::string& name);
	int32_t                  get_s32     (const std::string& name);
	uint64_t                 get_u64     (const std::string& name);
	int64_t                  get_s64     (const std::string& name);
	double                   get_double  (const std::string& name);
	std::string              get_string  (const std::string& name);
	std::vector<std::string> get_strings (const std::string& name);

	bool is_valid (void);

	void dump_config (void);

protected:
	std::string filename;
	bool read_only = true;
	bool valid = false;

	std::map<std::string,std::string> config;

private:
	std::weak_ptr<ConfigFile> self;
};


#endif // _CONFIG_FILE_H_

