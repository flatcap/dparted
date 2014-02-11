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

	StringNum (const char* str) : std::string (str)
	{
	}

	StringNum (const std::string& str) : std::string (str)
	{
	}

	virtual ~StringNum()
	{
	}

	const std::string operator= (const std::string& value)
	{
		return std::string::operator= (value);
	}

	operator bool()
	{
		return (strtol (c_str(), nullptr, 10) != 0);
	}

	operator uint32_t()
	{
		return strtoimax (c_str(), nullptr, 10);
	}

	operator int32_t()
	{
		return strtoumax (c_str(), nullptr, 10);
	}

	operator uint64_t()
	{
		return strtoul (c_str(), nullptr, 10);
	}

	operator int64_t()
	{
		return strtol (c_str(), nullptr, 10);
	}

	operator double()
	{
		return strtod (c_str(), nullptr);
	}
};


#endif // _STRINGNUM_H

