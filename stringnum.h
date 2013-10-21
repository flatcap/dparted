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

#ifndef _STRINGNUM_H
#define _STRINGNUM_H

#include <cstdlib>
#include <string>
#include <inttypes.h>

/**
 * class StringNum
 */
class StringNum : public std::string
{
public:
	//XXX do I need c'tor, d'tor, operator= ?
	StringNum() : std::string()
	{
	}

	StringNum (const char *str) : std::string (str)
	{
	}

	StringNum (const std::string &str) : std::string (str)
	{
	}

	virtual ~StringNum()
	{
	}

	const std::string operator= (const std::string &value)
	{
		return std::string::operator= (value);
	}

	operator bool()
	{
		return (strtol (c_str(), NULL, 10) != 0);
	}

	operator int()
	{
		return strtoimax (c_str(), NULL, 10);
	}

	operator long()
	{
		return strtol (c_str(), NULL, 10);
	}
};


#endif // _STRINGNUM_H

