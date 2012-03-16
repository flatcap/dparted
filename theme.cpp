/* Copyright (c) 2012 Richard Russon (FlatCap)
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


#include <gdkmm/rgba.h>

#include <map>

#include "theme.h"

/**
 * Theme
 */
Theme::Theme()
{
	set_colours();
}

/**
 * ~Theme
 */
Theme::~Theme()
{
}


/**
 * set_colours
 */
void Theme::set_colours (void)
{
	colours["unallocated"]	= Gdk::RGBA ("#a9a9a9");
	colours["unknown"]	= Gdk::RGBA ("#000000");
	colours["unformatted"]	= Gdk::RGBA ("#000000");
	colours["extended"]	= Gdk::RGBA ("#7dfcfe");
	colours["btrfs"]	= Gdk::RGBA ("#ff9955");
	colours["ext2"]		= Gdk::RGBA ("#9db8d2");
	colours["ext3"]		= Gdk::RGBA ("#7590ae");
	colours["ext4"]		= Gdk::RGBA ("#4b6983");
	colours["linux_swap"]	= Gdk::RGBA ("#c1665a");
	colours["fat16"]	= Gdk::RGBA ("#00ff00");
	colours["fat32"]	= Gdk::RGBA ("#18d918");
	colours["exfat"]	= Gdk::RGBA ("#2e8b57");
	colours["nilfs2"]	= Gdk::RGBA ("#826647");
	colours["ntfs"]		= Gdk::RGBA ("#42e5ac");
	colours["reiserfs"]	= Gdk::RGBA ("#ada7c8");
	colours["reiser4"]	= Gdk::RGBA ("#887fa3");
	colours["xfs"]		= Gdk::RGBA ("#eed680");
	colours["jfs"]		= Gdk::RGBA ("#e0c39e");
	colours["hfs"]		= Gdk::RGBA ("#e0b6af");
	colours["hfsplus"]	= Gdk::RGBA ("#c0a39e");
	colours["ufs"]		= Gdk::RGBA ("#d1940c");
	colours["used"]		= Gdk::RGBA ("#f8f8ba");
	colours["unused"]	= Gdk::RGBA ("#ffffff");
	colours["lvm2_pv"]	= Gdk::RGBA ("#cc9966");
	colours["luks"]		= Gdk::RGBA ("#625b81");
	colours["default"]	= Gdk::RGBA ("#000000");
}

/**
 * add_colour
 */
Gdk::RGBA Theme::add_colour (const std::string &name, const std::string &colour)
{
	Gdk::RGBA &c = colours[name];

	c.set (colour);

	return c;
}

/**
 * add_colour
 */
Gdk::RGBA Theme::add_colour (const std::string &name, const Gdk::RGBA &colour)
{
	Gdk::RGBA &c = colours[name];

	c = colour;

	return c;
}

/**
 * get_colour
 */
Gdk::RGBA Theme::get_colour (const std::string &name)
{
	Gdk::RGBA c ("black");

	std::map<std::string,Gdk::RGBA>::iterator it;

	it = colours.find (name);
	if (it == colours.end()) {
		it = colours.find ("default");
	}

	if (it != colours.end()) {
		c = (*it).second;
	}

	return c;
}

