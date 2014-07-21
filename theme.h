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

#ifndef _THEME_H_
#define _THEME_H_

#include <map>
#include <memory>

#include <gdkmm/pixbuf.h>
#include <gdkmm/rgba.h>
#include <glibmm/refptr.h>

#include "config_file.h"
#include "theme_listener.h"

class Theme;

typedef std::shared_ptr<Theme> ThemePtr;

class Theme
{
public:
	Theme();
	virtual ~Theme();

	bool add_colour (const std::string& name, const std::string& colour);
	Gdk::RGBA get_colour (const std::string& name);

	Glib::RefPtr<Gdk::Pixbuf> add_icon (const std::string& name, const std::string& filename);
	Glib::RefPtr<Gdk::Pixbuf> get_icon (const std::string& name);

	std::string get_config (const std::string& path, const std::string& name, const std::string& attr, bool recurse = true);
	//XXX get_config (just_a_path_name);
	std::vector<std::string> get_children (const std::string& name);

	bool is_valid (void);

	static ThemePtr read_file (const std::string& filename);

	void add_listener (const ThemeListenerPtr& tl);

protected:
	ConfigFilePtr config_file;

	std::map<std::string, Gdk::RGBA> colours;
	std::map<std::string, Glib::RefPtr<Gdk::Pixbuf>> icons;

	Glib::RefPtr<Gdk::Pixbuf> missing_icon;
	Glib::RefPtr<Gdk::Pixbuf> create_missing_pixbuf (int size);

	void init_icons (void);

private:
	//int seqnum = 0;

	std::vector<ThemeListenerWeak> theme_listeners;
};


#endif // _THEME_H_


