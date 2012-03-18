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
#include <gdkmm/pixbuf.h>

#include <map>

#include "theme.h"

/**
 * Theme
 */
Theme::Theme()
{
	init_colours();
	init_icons();
}

/**
 * ~Theme
 */
Theme::~Theme()
{
}


/**
 * init_colours
 */
void Theme::init_colours (void)
{
	colours["default"]	= Gdk::RGBA ("#000000");

	colours["btrfs"]	= Gdk::RGBA ("#FF9955");
	colours["exfat"]	= Gdk::RGBA ("#2E8B57");
	colours["ext2"]		= Gdk::RGBA ("#9DB8D2");
	colours["ext3"]		= Gdk::RGBA ("#7590AE");
	colours["ext4"]		= Gdk::RGBA ("#4B6983");
	colours["extended"]	= Gdk::RGBA ("#7DFCFE");
	colours["fat16"]	= Gdk::RGBA ("#00FF00");
	colours["fat32"]	= Gdk::RGBA ("#18D918");
	colours["hfs"]		= Gdk::RGBA ("#E0B6AF");
	colours["hfsplus"]	= Gdk::RGBA ("#C0A39E");
	colours["jfs"]		= Gdk::RGBA ("#E0C39E");
	colours["luks"]		= Gdk::RGBA ("#625B81");
	colours["lvm2_pv"]	= Gdk::RGBA ("#CC9966");
	colours["nilfs2"]	= Gdk::RGBA ("#826647");
	colours["ntfs"]		= Gdk::RGBA ("#42E5AC");
	colours["reiser4"]	= Gdk::RGBA ("#887FA3");
	colours["reiserfs"]	= Gdk::RGBA ("#ADA7C8");
	colours["swap"]		= Gdk::RGBA ("#C1665A");
	colours["table"]	= Gdk::RGBA ("#BEBEBE");
	colours["ufs"]		= Gdk::RGBA ("#D1940C");
	colours["unallocated"]	= Gdk::RGBA ("#A9A9A9");
	colours["unformatted"]	= Gdk::RGBA ("#000000");
	colours["unknown"]	= Gdk::RGBA ("#000000");
	colours["unused"]	= Gdk::RGBA ("#FFFFFF");
	colours["used"]		= Gdk::RGBA ("#F8F8BA");
	colours["xfs"]		= Gdk::RGBA ("#EED680");
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


/**
 * init_icons
 */
void Theme::init_icons (void)
{
	// We could load these on demand if it becomes slow.
	add_icon ("table", "icons/table.png");
	add_icon ("shield", "icons/shield.png");

	add_icon ("loop",   "/usr/share/icons/gnome/48x48/actions/gtk-refresh.png");
	add_icon ("disk",   "/usr/share/icons/gnome/48x48/devices/harddrive.png");
	add_icon ("file",   "/usr/share/icons/gnome/48x48/mimetypes/txt.png");
	add_icon ("network","/usr/share/icons/gnome/48x48/status/connect_established.png");

	add_icon ("warning", "/usr/share/icons/gnome/24x24/status/dialog-warning.png");
}

/**
 * add_icon
 */
Glib::RefPtr<Gdk::Pixbuf> Theme::add_icon (const std::string &name, const std::string &filename)
{
	Glib::RefPtr<Gdk::Pixbuf> &pb = icons[name];

	pb = Gdk::Pixbuf::create_from_file (filename);

	return pb;
}

/**
 * get_icon
 */
Glib::RefPtr<Gdk::Pixbuf> Theme::get_icon (const std::string &name)
{
	Glib::RefPtr<Gdk::Pixbuf> &pb = icons[name];

	if (!pb) {
		pb = icons["warning"];
	}

	return pb;
}
