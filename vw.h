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

#ifndef _VW_H_
#define _VW_H_

#include <iostream>
#include <stdexcept>
#include <cstdint>
#include <string>

class BaseVariant
{
public:
	BaseVariant (void)     = default;
	virtual ~BaseVariant() = default;

	BaseVariant (std::string) : type (Tag::t_string) {}
	BaseVariant (const char*) : type (Tag::t_string) {}
	BaseVariant (double)      : type (Tag::t_double) {}
	BaseVariant (bool)        : type (Tag::t_bool)   {}
	BaseVariant (uint8_t)     : type (Tag::t_u8)     {}
	BaseVariant (int8_t)      : type (Tag::t_s8)     {}
	BaseVariant (uint16_t)    : type (Tag::t_u16)    {}
	BaseVariant (int16_t)     : type (Tag::t_s16)    {}
	BaseVariant (uint32_t)    : type (Tag::t_u32)    {}
	BaseVariant (int32_t)     : type (Tag::t_s32)    {}
	BaseVariant (uint64_t)    : type (Tag::t_u64)    {}
	BaseVariant (int64_t)     : type (Tag::t_s64)    {}

	virtual operator std::string (void);
	virtual operator double      (void);
	virtual operator bool        (void);
	virtual operator uint8_t     (void);
	virtual operator  int8_t     (void);
	virtual operator uint16_t    (void);
	virtual operator  int16_t    (void);
	virtual operator uint32_t    (void);
	virtual operator  int32_t    (void);
	virtual operator uint64_t    (void);
	virtual operator  int64_t    (void);

	enum class Tag {
		t_unset,	// Empty
		t_string,	// Text
		t_double,	// Floating point
		t_bool,		//  1 bit
		t_u8,		//  8 bit unsigned integer
		t_s8,		//          signed
		t_u16,		// 16 bit unsigned integer
		t_s16,		//          signed
		t_u32,		// 32 bit unsigned integer
		t_s32,		//          signed
		t_u64,		// 64 bit unsigned integer
		t_s64		//          signed
	} type = Tag::t_unset;
};

template <typename T>
class Variant : public BaseVariant
{
public:
	Variant (T& v) :
		BaseVariant(v),
		value(v)
	{
	}
	virtual ~Variant()
	{
	}

	virtual operator T (void)
	{
		//std::cout << "derived cast" << std::endl;
		return value;
	}

protected:
	T& value;
};

#endif // _VW_H_
