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


#ifndef _THEME_H_
#define _THEME_H_

#include <gdkmm/pixbuf.h>
#include <gdkmm/rgba.h>

#include <map>

class Theme
{
public:
	Theme();
	virtual ~Theme();

	Gdk::RGBA add_colour (const std::string &name, const std::string &colour);
	Gdk::RGBA add_colour (const std::string &name, const Gdk::RGBA   &colour);
	Gdk::RGBA get_colour (const std::string &name);

	Glib::RefPtr<Gdk::Pixbuf> add_icon (const std::string &name, const std::string &filename);
	Glib::RefPtr<Gdk::Pixbuf> get_icon (const std::string &name);

protected:
	std::map<std::string,Gdk::RGBA> colours;
	std::map<std::string,Glib::RefPtr<Gdk::Pixbuf> > icons;

	void init_colours (void);
	void init_icons   (void);

private:

};

#endif /* _THEME_H_ */

