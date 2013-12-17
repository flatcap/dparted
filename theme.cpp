/* Copyright (c) 2013 Richard Russon (FlatCap)
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
#include <glibmm/fileutils.h>

#include <map>
#include <iostream>
#include <string>

#include <libconfig.h++>

#include "log.h"
#include "theme.h"
#include "log_trace.h"

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
void
Theme::init_colours (void)
{
	// "yellow"			Colour name
	// "#F80"			Hex triplet: 1 digit  for Red, Green, Blue
	// "#FF8800"			Hex triplet: 2 digits for Red, Green, Blue
	// "#FFF888000"			Hex triplet: 3 digits for Red, Green, Blue
	// "#FFFF88880000"		Hex triplet: 4 digits for Red, Green, Blue
	// "rgb(255,128,0)"		Decimal triplet: range 0-255 for Red, Green, Blue
	// "rgba(255,128,0,0.5)"	Decimal triplet + alpha (range 0.0 - 1.0)

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
Gdk::RGBA
Theme::add_colour (const std::string& name, const std::string& colour)
{
	Gdk::RGBA& c = colours[name];

	c.set (colour);

	return c;
}

/**
 * add_colour
 */
Gdk::RGBA
Theme::add_colour (const std::string& name, const Gdk::RGBA& colour)
{
	Gdk::RGBA& c = colours[name];

	c = colour;

	return c;
}

/**
 * get_colour
 */
Gdk::RGBA
Theme::get_colour (const std::string& name)
{
	Gdk::RGBA c ("rgba(255,0,255,0.3)");	//XXX change default to white

	std::map<std::string,Gdk::RGBA>::iterator it;

	it = colours.find (name);
	if (it == colours.end()) {
		c.set (name);
	} else {
		c = (*it).second;
	}

	//c.set_alpha (0.3);
	return c;
}


/**
 * init_icons
 */
void
Theme::init_icons (void)
{
	// We could load these on demand if it becomes slow.
	add_icon ("table", "icons/table.png");
	add_icon ("shield", "icons/shield.png");

	add_icon ("loop",   "/usr/share/icons/gnome/48x48/actions/gtk-refresh.png");
	add_icon ("disk",   "/usr/share/icons/gnome/48x48/devices/harddrive.png");
	add_icon ("file",   "/usr/share/icons/gnome/48x48/mimetypes/txt.png");
	add_icon ("network","/usr/share/icons/gnome/48x48/status/connect_established.png");

	add_icon ("warning", "/usr/share/icons/gnome/24x24/status/dialog-warning.png");

	add_icon ("margin_red",   "icons/margin_red.png");
	add_icon ("margin_black", "icons/margin_black.png");

	add_icon ("margin_red64",   "icons/margin_red64.png");
	add_icon ("margin_black64", "icons/margin_black64.png");
}

/**
 * add_icon
 */
Glib::RefPtr<Gdk::Pixbuf>
Theme::add_icon (const std::string& name, const std::string& filename)
{
	Glib::RefPtr<Gdk::Pixbuf>& pb = icons[name];

	try {
		pb = Gdk::Pixbuf::create_from_file (filename);
	} catch (const Glib::FileError& fe) {
		std::cout << "file error: " << fe.what() << std::endl;
	} catch (const Gdk::PixbufError& pbe) {
		std::cout << "pixbuf error: " << pbe.what() << std::endl;
	}

	return pb;
}


#include "missing.cpp"

/**
 * get_icon
 */
Glib::RefPtr<Gdk::Pixbuf>
Theme::get_icon (const std::string& name)
{
	Glib::RefPtr<Gdk::Pixbuf>& pb = icons[name];

	if (!pb) {
#if 0
		pb = icons["warning"];

		pb = Gdk::Pixbuf::create (Gdk::Colorspace::COLORSPACE_RGB, true, 8, 24, 24);
		pb->fill (0xFF0000FF);
#endif
		pb = Gdk::Pixbuf::create_from_inline (sizeof (myimage_inline), myimage_inline);
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

/**
 * get_value
 */
std::string
Theme::get_value (const libconfig::Setting& s)
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
Theme::parse_config (const libconfig::Setting& setting)
{
	for (int i = 0; i < setting.getLength(); i++) {
		const libconfig::Setting& s = setting[i];

		if (s.isScalar()) {
			std::string path  = s.getPath();
			std::string value = get_value(s);

			config[path] = value;
		}

		if (s.isGroup()) {
			parse_config (s);
		}
	}
}

/**
 * read_config
 */
bool
Theme::read_config (const char* filename)
{
	libconfig::Config cfg;

	try
	{
		cfg.readFile(filename);
	}
	catch(const libconfig::FileIOException& fioex)
	{
		std::cerr << "I/O error while reading file." << std::endl;
		return false;
	}
	catch(const libconfig::ParseException& pex)
	{
		std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine() << " - " << pex.getError() << std::endl;
		return false;
	}

	try
	{
		std::string name = cfg.lookup("name");
		//std::cout << "Theme: " << name << std::endl << std::endl;
	}
	catch(const libconfig::SettingNotFoundException& nfex)
	{
		std::cerr << "No 'name' setting in configuration file." << std::endl;
	}

	const libconfig::Setting& root = cfg.getRoot();

	parse_config (root);

#if 0
	log_info ("Theme:\n");
	for (auto c : config) {
		std::cout << '\t' << c.first << " = " << c.second << std::endl;
	}
	log_info ("\n");
#endif

	return true;
}

/**
 * get_config
 */
std::string
Theme::get_config (std::string path, const std::string& name, const std::string& attr)
{
	std::map<std::string,std::string>::iterator it;
	std::map<std::string,std::string>::iterator end = config.end();
	std::string pathname;
	std::string value;

	pathname = path;
	if (!pathname.empty())
		pathname += ".";
	pathname += name + "." + attr;
	//printf ("config: search:  %s\n", pathname.c_str());
	it = config.find (pathname);
	if (it != end) {
		return it->second;
	}

	for (auto i = 0; i < 20; i++) {
		pathname = path;
		if (!pathname.empty())
			pathname += ".";
		pathname += attr;

		//printf ("config: search:  %s\n", pathname.c_str());
		it = config.find (pathname);
		if (it != end) {
			value = it->second;
			break;
		}

		size_t pos = path.find_last_of(".");
		if (pos == std::string::npos) {
			break;
		}

		path.erase (pos);
	}

	if (value.empty()) {
		log_error ("config: missing: %s.%s.%s\n", path.c_str(), name.c_str(), attr.c_str());
	} else {
		//log_info ("config: found:   %s = %s\n", pathname.c_str(), value.c_str());
	}

	return value;
}


