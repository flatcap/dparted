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

#ifndef _VARIANT_H_
#define _VARIANT_H_

#include <iostream>
#include <stdexcept>
#include <cstdint>
#include <string>

/**
 * class BaseVariant
 */
class BaseVariant
{
public:
	BaseVariant (const char* owner, const char* name, const char* desc) :
		owner (owner),
		name  (name),
		desc  (desc)
	{
	}

	BaseVariant (void)     = default;
	virtual ~BaseVariant() = default;

	BaseVariant (const BaseVariant&  other) = default;
	BaseVariant (BaseVariant&& other)       = default;

	BaseVariant& operator= (BaseVariant&  other) = default;
	BaseVariant& operator= (BaseVariant&& other) = default;

	void set_type (std::string) { type = Tag::t_string; }
	void set_type (const char*) { type = Tag::t_string; }
	void set_type (double)      { type = Tag::t_double; }
	void set_type (bool)        { type = Tag::t_bool;   }
	void set_type (uint8_t)     { type = Tag::t_u8;     }
	void set_type (int8_t)      { type = Tag::t_s8;     }
	void set_type (uint16_t)    { type = Tag::t_u16;    }
	void set_type (int16_t)     { type = Tag::t_s16;    }
	void set_type (uint32_t)    { type = Tag::t_u32;    }
	void set_type (int32_t)     { type = Tag::t_s32;    }
	void set_type (uint64_t)    { type = Tag::t_u64;    }
	void set_type (int64_t)     { type = Tag::t_s64;    }

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

	std::string owner;
	std::string name;
	std::string desc;
};


/**
 * template class Variant
 */
template <typename T>
class Variant : public BaseVariant
{
public:
	Variant (const char* owner, const char* name, T& v, const char* desc = "") :
		BaseVariant(owner, name, desc),
		value(v)
	{
		set_type(v);
	}

	virtual ~Variant()
	{
	}

	Variant (void) = default;

	Variant (const Variant&  other) = default;
	Variant (Variant&& other)       = default;

	Variant& operator= (Variant&  other) = default;
	Variant& operator= (Variant&& other) = default;

	virtual operator T (void)
	{
		return value;
	}

protected:
	T& value;
};


#endif // _VARIANT_H_

