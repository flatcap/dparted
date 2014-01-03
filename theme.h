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

#ifndef _THEME_H_
#define _THEME_H_

#include <map>
#include <memory>

#include <gdkmm/pixbuf.h>
#include <gdkmm/rgba.h>

#include <libconfig.h++>

class Theme;

typedef std::shared_ptr<Theme> ThemePtr;

/**
 * class Theme
 */
class Theme
{
public:
	Theme();
	virtual ~Theme();

	Gdk::RGBA add_colour (const std::string& name, const std::string& colour);
	Gdk::RGBA add_colour (const std::string& name, const Gdk::RGBA&   colour);
	Gdk::RGBA get_colour (const std::string& name);

	Glib::RefPtr<Gdk::Pixbuf> add_icon (const std::string& name, const std::string& filename);
	Glib::RefPtr<Gdk::Pixbuf> get_icon (const std::string& name);

	bool read_config (const char* filename);
	std::string get_config (std::string path, const std::string& name, const std::string& attr);

	bool is_valid (void);

protected:
	std::map<std::string,Gdk::RGBA> colours;
	std::map<std::string,Glib::RefPtr<Gdk::Pixbuf> > icons;

	void init_colours (void);
	void init_icons   (void);

	void parse_config (const libconfig::Setting& setting);
	std::string get_value (const libconfig::Setting& s);

	std::map<std::string,std::string> config;

private:
	int seqnum = 0;

};


#endif // _THEME_H_


