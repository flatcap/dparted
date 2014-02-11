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

#ifndef _ICON_MANAGER_H_
#define _ICON_MANAGER_H_

#include <map>
#include <string>

#include <glibmm/refptr.h>
#include <gdkmm/pixbuf.h>

class IconManager
{
public:
	IconManager (void);
	virtual ~IconManager();

	Glib::RefPtr<Gdk::Pixbuf> get_icon (const std::string& name, int w, int h);

protected:
	std::map <std::string, Glib::RefPtr<Gdk::Pixbuf>> icons;

};

#endif // _ICON_MANAGER_H_

