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

#ifndef _BASIC_H_
#define _BASIC_H_

#include <string>
#include <iostream>
#include <cstdint>

class Variant
{
public:
	Variant (void);
	virtual ~Variant();

	Variant (std::string value);
	Variant (double      value);
	Variant (bool        value);
	Variant (uint8_t     value);
	Variant (int8_t      value);
	Variant (uint16_t    value);
	Variant (int16_t     value);
	Variant (uint32_t    value);
	Variant (int32_t     value);
	Variant (uint64_t    value);
	Variant (int64_t     value);

	operator std::string (void);
	operator double      (void);
	operator bool        (void);
	operator uint8_t     (void);
	operator int8_t      (void);
	operator uint16_t    (void);
	operator int16_t     (void);
	operator uint32_t    (void);
	operator int32_t     (void);
	operator uint64_t    (void);
	operator int64_t     (void);

	enum class Tag {
		t_unset,	// Empty
		t_string,	// Text
		t_double,	// Floating point
		t_bool,		//  1 bit
		t_u8,		//  8 bits unsigned integer
		t_s8,		//         signed
		t_u16,		// 16 bits unsigned integer
		t_s16,		//         signed
		t_u32,		// 32 bits unsigned integer
		t_s32,		//         signed
		t_u64,		// 64 bits unsigned integer
		t_s64		//         signed
	} type;

protected:
	friend std::ostream & operator<< (std::ostream &os, const Variant *v);
	friend std::ostream & operator<< (std::ostream &os, const Variant &v);

	union {
		double   d_value;
		uint64_t l_value;
	};
	std::string s_value;
};

#endif // _BASIC_H_

