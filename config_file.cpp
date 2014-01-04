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

#include <algorithm>
#include <vector>
#include <string>

#include <libconfig.h++>

#include "config_file.h"
#include "stringnum.h"

/**
 * ConfigFile
 */
ConfigFile::ConfigFile (void)
{
}

/**
 * ~ConfigFile
 */
ConfigFile::~ConfigFile()
{
}


/**
 * get_value
 */
std::string
get_value (const libconfig::Setting& s)
{
	libconfig::Setting::Type t = s.getType();

	switch (t) {
		case libconfig::Setting::TypeString:
			return s.operator std::string();
		case libconfig::Setting::TypeBoolean:
			if (bool(s))
				return "true";
			else
				return "false";
			return "something";
		case libconfig::Setting::TypeInt:
			return std::to_string (int (s));
		case libconfig::Setting::TypeInt64:
			return std::to_string (long (s));
		case libconfig::Setting::TypeFloat:
			return std::to_string (float (s));
		default:
			return "unknown";
	}
}

/**
 * parse_config
 */
void
parse_config (const libconfig::Setting& setting, std::map<std::string,std::string>& config)
{
	for (int i = 0; i < setting.getLength(); i++) {
		const libconfig::Setting& s = setting[i];

		if (s.isScalar()) {
			std::string path  = s.getPath();
			std::string value = get_value(s);

			config[path] = value;
		}

		if (s.isGroup() || s.isArray() || s.isList()) {
			parse_config (s, config);
		}
	}
}


/**
 * is_valid
 */
bool ConfigFile::is_valid (void)
{
	return valid;
}


/**
 * read_file
 */
ConfigFilePtr
ConfigFile::read_file (const std::string& filename)
{
	libconfig::Config cfg;

	try {
		cfg.readFile(filename.c_str());
	} catch(const libconfig::FileIOException& fioex) {
		std::cout << "I/O error while reading file." << std::endl;
		return false;
	} catch(const libconfig::ParseException& pex) {
		std::cout << "Parse error at " << pex.getFile() << ":" << pex.getLine() << " - " << pex.getError() << std::endl;
		return false;
	}

	const libconfig::Setting& root = cfg.getRoot();

	ConfigFilePtr cf (new ConfigFile());

	parse_config (root, cf->config);
	//cf->dump_config();

	return cf;
}

/**
 * dump_config
 */
void
ConfigFile::dump_config (void)
{
	std::cout << "Config:\n";
	for (auto c : config) {
		std::cout << "\t" << c.first << " = " << c.second << "\n";
	}
}


/**
 * exists
 */
bool ConfigFile::exists (const std::string& name)
{
	return (config.count (name) > 0);
}


/**
 * get_bool
 */
bool
ConfigFile::get_bool (const std::string& name)
{
	if (!exists (name))
		throw "config: " + name + "doesn't exist";

	StringNum value = config[name];
	std::transform (value.begin(), value.end(), value.begin(), ::tolower);

	return (value == "true");
}

/**
 * get_u32
 */
uint32_t
ConfigFile::get_u32 (const std::string& name)
{
	if (!exists (name))
		throw "config: " + name + "doesn't exist";

	StringNum value = config[name];

	return (uint32_t) value;
}

/**
 * get_s32
 */
int32_t
ConfigFile::get_s32 (const std::string& name)
{
	if (!exists (name))
		throw "config: " + name + "doesn't exist";

	StringNum value = config[name];

	return (int32_t) value;
}

/**
 * get_u64
 */
uint64_t
ConfigFile::get_u64 (const std::string& name)
{
	if (!exists (name))
		throw "config: " + name + "doesn't exist";

	StringNum value = config[name];

	return (uint64_t) value;
}

/**
 * get_s64
 */
int64_t
ConfigFile::get_s64 (const std::string& name)
{
	if (!exists (name))
		throw "config: " + name + "doesn't exist";

	StringNum value = config[name];

	return (int64_t) value;
}

/**
 * get_double
 */
double
ConfigFile::get_double (const std::string& name)
{
	if (!exists (name))
		throw "config: " + name + "doesn't exist";

	StringNum value = config[name];

	return (double) value;
}

/**
 * get_string
 */
std::string
ConfigFile::get_string (const std::string& name)
{
	if (!exists (name))
		throw "config: " + name + "doesn't exist";

	return config[name];
}

/**
 * get_strings
 */
std::vector<std::string>
ConfigFile::get_strings (const std::string& name)
{
	std::vector<std::string> vs;
	std::string array;

	for (int i = 0; i < 64; i++) {
		array = name + ".[" + std::to_string(i) + "]";
		if (!exists (array))
			break;

		vs.push_back (config[array]);
	}

	if (vs.empty()) {
		throw "config: " + array + "doesn't exist";
	}

	return vs;
}

