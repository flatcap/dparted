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


#ifndef _STRINGNUM_H
#define _STRINGNUM_H

#include <cstdlib>
#include <string>

class StringNum : public std::string
{
public:
	StringNum() : std::string()
	{
	}

	StringNum (const char *str) : std::string (str)
	{
	}

	const std::string operator= (const std::string &value)
	{
		return std::string::operator= (value);
	}

	operator long()
	{
		return strtol (c_str(), NULL, 10);
	}

	operator long long()
	{
		return strtoll (c_str(), NULL, 10);
	}
};

#endif /* _STRINGNUM_H */

