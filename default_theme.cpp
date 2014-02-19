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

#include <map>
#include <string>

#include "default_theme.h"

DefaultTheme::DefaultTheme (void)
{
#if 0
	config = {
		{ "Container.Filesystem.display",      "box" },
		{ "Container.Filesystem.colour",       "red" },
		{ "Container.Filesystem.background",   "white" },
		{ "Container.Filesystem.usage",        "true" },
		{ "Container.Filesystem.label",        "{device_short} - {bytes_size_human}\n{label}" },
		{ "Container.Filesystem.btrfs.colour", "#FF9955" },
		{ "Container.Filesystem.exfat.colour", "#2E8B57" },
		{ "Container.Filesystem.ext2.colour",  "#9DB8D2" },
	};
#endif
}

DefaultTheme::~DefaultTheme()
{
}


