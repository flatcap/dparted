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

#ifndef _OPTIONGROUP_H_
#define _OPTIONGROUP_H_

#include <vector>
#include <string>

#include <glibmm/optiongroup.h>

class OptionGroup : public Glib::OptionGroup
{
public:
	OptionGroup();

	bool quit = false;
	int x = -1;	// Window coordinates
	int y = -1;
	int w = -1;	// Window size
	int h = -1;

	std::vector<std::string> config;
	std::vector<std::string> theme;
};


#endif // _OPTIONGROUP_H_

