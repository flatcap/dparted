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

#ifndef _PROP_H_
#define _PROP_H_

#include <iostream>
#include <stdexcept>
#include <cstdint>
#include <string>
#include <memory>

class BaseProp;

typedef std::shared_ptr<BaseProp> PPtr;

class BaseProp
{
public:
	BaseProp (const char* owner, const char* name, const char* desc) :
		owner (owner),
		name  (name),
		desc  (desc)
	{
	}

	BaseProp (void)     = default;
	virtual ~BaseProp() = default;

	BaseProp (const BaseProp&  other) = default;
	BaseProp (BaseProp&& other)       = default;

	BaseProp& operator= (BaseProp&  other) = default;
	BaseProp& operator= (BaseProp&& other) = default;

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


template <typename T>
class Prop : public BaseProp
{
public:
	Prop (const char* owner, const char* name, T& v, const char* desc = "") :
		BaseProp (owner, name, desc),
		value(v)
	{
		set_type(v);
	}

	virtual ~Prop()
	{
	}

	Prop (void) = default;

	Prop (const Prop&  other) = default;
	Prop (Prop&& other)       = default;

	Prop& operator= (Prop&  other) = default;
	Prop& operator= (Prop&& other) = default;

	virtual operator T (void)
	{
		return value;
	}

protected:
	T& value;
};


#endif // _PROP_H_

