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


#include <map>
#include <string>

std::map<std::string,std::string>
get_default_theme (void)
{
	std::map<std::string,std::string> theme = {
		{ "Container.Filesystem.display",      "box" },
		{ "Container.Filesystem.colour",       "red" },
		{ "Container.Filesystem.background",   "white" },
		{ "Container.Filesystem.usage",        "true" },
		{ "Container.Filesystem.label",        "{device_short} - {bytes_size_human}\n{label}" },
		{ "Container.Filesystem.btrfs.colour", "#FF9955" },
		{ "Container.Filesystem.exfat.colour", "#2E8B57" },
		{ "Container.Filesystem.ext2.colour",  "#9DB8D2" },
	};

	//XXX convert this to ConfigPtr tree

	return theme;
}

