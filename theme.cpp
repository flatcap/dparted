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

#include <gdkmm/pixbuf.h>
#include <gdkmm/rgba.h>
#include <glibmm/fileutils.h>
#include <cairomm/cairomm.h>

#include <map>
#include <string>

#include "log.h"
#include "theme.h"
#include "log_trace.h"

Theme::Theme (void)
{
	init_icons();
}

Theme::~Theme()
{
}


bool
Theme::add_colour (const std::string& name, const std::string& colour)
{
	Gdk::RGBA& c = colours[name];

	if (c.set (colour))
		return true;

	return false;
}

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


void
Theme::init_icons (void)
{
	// We could load these on demand if it becomes slow.
	//XXX read these from theme file
	add_icon ("table", "icons/table.png");
	add_icon ("shield", "icons/shield.png");
	add_icon ("locked", "icons/locked.png");

	add_icon ("loop",   "/usr/share/icons/gnome/48x48/actions/gtk-refresh.png");
	add_icon ("disk",   "/usr/share/icons/gnome/48x48/devices/harddrive.png");
	add_icon ("file",   "/usr/share/icons/gnome/48x48/mimetypes/txt.png");
	add_icon ("network","/usr/share/icons/gnome/48x48/status/connect_established.png");

	add_icon ("warning", "/usr/share/icons/gnome/24x24/status/dialog-warning.png");
}

Glib::RefPtr<Gdk::Pixbuf>
Theme::add_icon (const std::string& name, const std::string& filename)
{
	Glib::RefPtr<Gdk::Pixbuf>& pb = icons[name];

	try {
		pb = Gdk::Pixbuf::create_from_file (filename);
	} catch (const Glib::FileError& fe) {
		log_debug ("file error: %s\n", fe.what().c_str());
	} catch (const Gdk::PixbufError& pbe) {
		log_debug ("pixbuf error: %s\n", pbe.what().c_str());
	}

	return pb;
}


Glib::RefPtr<Gdk::Pixbuf>
Theme::create_missing_pixbuf (int size)
{
	Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create (Gdk::COLORSPACE_RGB, true, 8, size, size);
	Cairo::RefPtr<Cairo::ImageSurface> surface = Cairo::ImageSurface::create (Cairo::FORMAT_ARGB32, size, size);
	Cairo::RefPtr<Cairo::Context> cr = Cairo::Context::create (surface);

	cr->save();
	cr->set_source_rgba (1, 1, 1, 1);
	cr->rectangle (0, 0, size, size);
	cr->fill();

	//XXX note the order: ImageSurface (RGBA) and Pixbuf (BGRA)
	cr->set_source_rgba (0, 0, 1, 1);
	cr->set_line_width(size/4);

	cr->move_to (0, 0);
	cr->rel_line_to (size, size);
	cr->stroke();

	cr->move_to (size, 0);
	cr->rel_line_to (-size, size);
	cr->stroke();

	cr->restore();

	memcpy (pixbuf->get_pixels(), surface->get_data(), size*size*4);	//XXX 4 hard-coded!

	return pixbuf;
}

Glib::RefPtr<Gdk::Pixbuf>
Theme::get_icon (const std::string& name)
{
	Glib::RefPtr<Gdk::Pixbuf>& pb = icons[name];

	if (!pb) {
		if (!missing_icon) {
			missing_icon = create_missing_pixbuf (24);
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
			//log_debug ("Search: %s\n", search.c_str());
			if (config_file->exists (search)) {
				return config_file->get_string (search);
			}
		}

		search = work_path + dot + attr;
		//log_debug ("Search: %s\n", search.c_str());
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


bool
Theme::is_valid (void)
{
	return true;
}

ThemePtr
Theme::read_file (const std::string& filename)
{
	ThemePtr tp (new Theme());

	ConfigFilePtr cf;

	cf = ConfigFile::read_file (filename);
	if (!cf) {
		//notify the user
		return nullptr;
	}

	if (tp->config_file) {
		//if modified ask user if they're sure
	}

	tp->config_file = cf;
	//cf->dump_config();

	return tp;
}

