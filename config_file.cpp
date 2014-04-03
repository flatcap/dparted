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

#include <algorithm>
#include <vector>
#include <string>

#include <libconfig.h++>

#include "config_file.h"
#include "stringnum.h"
#include "log.h"

ConfigFile::ConfigFile (void)
{
}

ConfigFile::~ConfigFile()
{
}

ConfigFilePtr
ConfigFile::create (void)
{
	ConfigFilePtr c (new ConfigFile());
	c->self = c;

	return c;
}


std::string
get_value (const libconfig::Setting& s)
{
	libconfig::Setting::Type t = s.getType();

	switch (t) {
		case libconfig::Setting::TypeString:
			return s.operator std::string();
		case libconfig::Setting::TypeBoolean:
			if (bool(s)) {
				return "true";
			} else {
				return "false";
			}
		case libconfig::Setting::TypeInt:
			return std::to_string (int(s));
		case libconfig::Setting::TypeInt64:
			return std::to_string (long(s));
		case libconfig::Setting::TypeFloat:
			return std::to_string (float(s));
		default:
			return "unknown";
	}
}

void
parse_config (const libconfig::Setting& setting, std::map<std::string,std::string>& config)
{
	if (setting.getLength() == 0) {
		config[setting.getPath()] = "{empty}";
		return;
	}

	for (int i = 0; i < setting.getLength(); ++i) {
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


bool
ConfigFile::is_valid (void)
{
	return valid;
}


ConfigFilePtr
ConfigFile::read_file (const std::string& filename)
{
	libconfig::Config cfg;

	try {
		cfg.readFile (filename.c_str());
	} catch (const libconfig::FileIOException& fioex) {
		log_debug ("I/O error while reading file.\n");
		return false;
	} catch (const libconfig::ParseException& pex) {
		log_debug ("Parse error at %s:%d - %s\n", pex.getFile(), pex.getLine(), pex.getError());
		return false;
	}

	const libconfig::Setting& root = cfg.getRoot();

	ConfigFilePtr cf = ConfigFile::create();

	parse_config (root, cf->config);
	//cf->dump_config();

	return cf;
}

void
ConfigFile::dump_config (void)
{
	log_debug ("Config:\n");
	for (auto c : config) {
		log_debug ("\t%s = %s\n", c.first.c_str(), c.second.c_str());
	}
}


bool
ConfigFile::exists (const std::string& name)
{
	return (config.count (name) > 0);
}


bool
ConfigFile::get_bool (const std::string& name)
{
	if (!exists (name))
		throw "config: " + name + "doesn't exist";

	StringNum value = config[name];
	std::transform (value.begin(), value.end(), value.begin(), ::tolower);

	return (value == "true");
}

uint32_t
ConfigFile::get_u32 (const std::string& name)
{
	if (!exists (name))
		throw "config: " + name + "doesn't exist";

	StringNum value = config[name];

	return (uint32_t) value;
}

int32_t
ConfigFile::get_s32 (const std::string& name)
{
	if (!exists (name))
		throw "config: " + name + "doesn't exist";

	StringNum value = config[name];

	return (int32_t) value;
}

uint64_t
ConfigFile::get_u64 (const std::string& name)
{
	if (!exists (name))
		throw "config: " + name + "doesn't exist";

	StringNum value = config[name];

	return (uint64_t) value;
}

int64_t
ConfigFile::get_s64 (const std::string& name)
{
	if (!exists (name))
		throw "config: " + name + "doesn't exist";

	StringNum value = config[name];

	return (int64_t) value;
}

double
ConfigFile::get_double (const std::string& name)
{
	if (!exists (name))
		throw "config: " + name + "doesn't exist";

	StringNum value = config[name];

	return (double) value;
}

std::string
ConfigFile::get_string (const std::string& name)
{
	if (!exists (name))
		throw "config: " + name + "doesn't exist";

	return config[name];
}

std::vector<std::string>
ConfigFile::get_strings (const std::string& name)
{
	std::vector<std::string> vs;
	std::string array;

	for (int i = 0; i < 64; ++i) {
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

std::vector<std::string>
ConfigFile::get_children (const std::string& name)
{
	std::vector<std::string> children;

	int len = name.length();
	for (auto it : config) {
		bool match = (it.first.compare (0, len, name) == 0);
		if (match) {
			std::string s = it.first.substr (len+1);
			size_t pos = s.find_first_of ('.');
			if (pos != std::string::npos) {
				s = s.substr (0, pos);
			}

			if (children.empty() || (children.size() && (s != children.back()))) {
				children.push_back (s);
			}
		} else {
			if (children.size())
				break;
		}
	}

	return children;
}

