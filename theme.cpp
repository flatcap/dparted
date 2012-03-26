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


#include <gdkmm/pixbuf.h>
#include <gdkmm/rgba.h>

#include <map>

#include "log.h"
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
	// "yellow"		Colour name
	// "#F80"		Hex triplet: 1 digit  for Red, Green, Blue
	// "#FF8800"		Hex triplet: 2 digits for Red, Green, Blue
	// "#FFF888000"		Hex triplet: 3 digits for Red, Green, Blue
	// "#FFFF88880000"	Hex triplet: 4 digits for Red, Green, Blue
	// "rgb(255,128,0)	Decimal triplet: range 0-255 for Red, Green, Blue
	// "rgba(255,128,0,0.5)	Decimal triplet + alpha (range 0.0 - 1.0)

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

	colours["focus1"]	= Gdk::RGBA ("#000000");
	colours["focus2"]	= Gdk::RGBA ("#FFFFFF");
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
	Gdk::RGBA c ("rgba(255,0,255,0.3)");	//XXX change default to white

	std::map<std::string,Gdk::RGBA>::iterator it;

	it = colours.find (name);
	if (it == colours.end()) {
		c.set (name);
	} else {
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


#if 0
	// move to main window, add accessor function
	Glib::RefPtr<Gtk::StyleContext> style;
	style = get_style_context();

	Gdk::RGBA fg;
	Gdk::RGBA bg;

	fg = style->get_color (Gtk::STATE_FLAG_NORMAL);
	bg = style->get_background_color (Gtk::STATE_FLAG_NORMAL);

	std::cout << "fg = " << fg.get_red() << "," << fg.get_green() << "," << fg.get_blue() << "\n";
	std::cout << "bg = " << bg.get_red() << "," << bg.get_green() << "," << bg.get_blue() << "\n";
#endif

