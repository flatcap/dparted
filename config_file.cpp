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

#include <libconfig.h++>

#include "config_file.h"

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
	cf->dump_config();

	return nullptr;
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

