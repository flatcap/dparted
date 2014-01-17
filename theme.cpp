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

#include <gdkmm/pixbuf.h>
#include <gdkmm/rgba.h>
#include <glibmm/fileutils.h>

#include <map>
#include <iostream>
#include <string>

#include "log.h"
#include "theme.h"
#include "log_trace.h"
#include "missing_icon.h"

/**
 * Theme
 */
Theme::Theme()
{
	init_icons();
}

/**
 * ~Theme
 */
Theme::~Theme()
{
}


/**
 * add_colour
 */
bool
Theme::add_colour (const std::string& name, const std::string& colour)
{
	Gdk::RGBA& c = colours[name];

	if (c.set (colour))
		return true;

	return false;
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


/**
 * get_icon
 */
Glib::RefPtr<Gdk::Pixbuf>
Theme::get_icon (const std::string& name)
{
	Glib::RefPtr<Gdk::Pixbuf>& pb = icons[name];

	if (!pb) {
		if (!missing_icon) {
			missing_icon = Gdk::Pixbuf::create_from_inline (sizeof (missing_icon_bits), (const uint8_t*) missing_icon_bits, false);
		}

		pb = missing_icon;
	}

	return pb;
}


/**
 * get_config
 *
 * Search Order
 *	PATH1.PATH2.PATH3.NAME.ATTR
 *	PATH1.PATH2.PATH3.ATTR
 *
 *	PATH1.PATH2.NAME.ATTR
 *	PATH1.PATH2.ATTR
 *
 *	PATH1.NAME.ATTR
 *	PATH1.ATTR
 */
std::string
Theme::get_config (const std::string& path, const std::string& name, const std::string& attr)
{
	std::string work_path = path;
	std::string dot;
	std::string search;

	for (auto i = 0; i < 20; i++) {
		if (work_path.empty()) {
			dot.clear();
		} else {
			dot = ".";
		}

		if (!name.empty()) {
			search = work_path + dot + name + "." + attr;
			//std::cout << "Search: " << search << std::endl;
			if (config_file->exists (search)) {
				return config_file->get_string (search);
			}
		}

		search = work_path + dot + attr;
		//std::cout << "Search: " << search << std::endl;
		if (config_file->exists (search)) {
			return config_file->get_string (search);
		}

		size_t pos = work_path.find_last_of (".");
		if (pos == std::string::npos) {
			if (work_path.empty()) {
				break;
			}
			work_path.clear();
		} else {
			work_path.erase (pos);
		}
	}

	throw "can't find config: " + path + "/" + name + "/" + attr;
}


/**
 * is_valid
 */
bool
Theme::is_valid (void)
{
	return true;
}

/**
 * read_file
 */
ThemePtr
Theme::read_file (const std::string& filename)
{
	ThemePtr tp (new Theme());

	ConfigFilePtr cf;

	cf = ConfigFile::read_file (filename);
	if (!cf) {
		//notify the user
		return false;
	}

	if (tp->config_file) {
		//if modified ask user if they're sure
	}

	tp->config_file = cf;
	//cf->dump_config();

	return tp;
}

