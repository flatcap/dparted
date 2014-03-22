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

#ifndef _PROPERTY_H_
#define _PROPERTY_H_

#include <iostream>
#include <stdexcept>
#include <cstdint>
#include <string>
#include <memory>
#include <vector>

class BaseProperty;

typedef std::shared_ptr<BaseProperty> PPtr;

typedef std::function<std::string              (void)> get_string_t;
typedef std::function<std::vector<std::string> (void)> get_list_t;
typedef std::function<double                   (void)> get_double_t;
typedef std::function<bool                     (void)> get_bool_t;
typedef std::function<uint8_t                  (void)> get_uint8_t;
typedef std::function< int8_t                  (void)> get_int8_t;
typedef std::function<uint16_t                 (void)> get_uint16_t;
typedef std::function< int16_t                 (void)> get_int16_t;
typedef std::function<uint32_t                 (void)> get_uint32_t;
typedef std::function< int32_t                 (void)> get_int32_t;
typedef std::function<uint64_t                 (void)> get_uint64_t;
typedef std::function< int64_t                 (void)> get_int64_t;

class BaseProperty
{
public:
	BaseProperty (const char* owner, const char* name, const char* desc, int flags) :
		owner (owner),
		name  (name),
		desc  (desc),
		flags (flags)
	{
	}

	BaseProperty (void)     = default;
	virtual ~BaseProperty() = default;

	BaseProperty (const BaseProperty&  other) = default;
	BaseProperty (      BaseProperty&& other) = default;

	BaseProperty& operator= (BaseProperty&  other) = default;
	BaseProperty& operator= (BaseProperty&& other) = default;

	void set_type (std::string)              { type = Tag::t_string; }
	void set_type (std::vector<std::string>) { type = Tag::t_list; }
	void set_type (const char*)              { type = Tag::t_string; }
	void set_type (double)                   { type = Tag::t_double; }
	void set_type (bool)                     { type = Tag::t_bool;   }
	void set_type (uint8_t)                  { type = Tag::t_u8;     }
	void set_type (int8_t)                   { type = Tag::t_s8;     }
	void set_type (uint16_t)                 { type = Tag::t_u16;    }
	void set_type (int16_t)                  { type = Tag::t_s16;    }
	void set_type (uint32_t)                 { type = Tag::t_u32;    }
	void set_type (int32_t)                  { type = Tag::t_s32;    }
	void set_type (uint64_t)                 { type = Tag::t_u64;    }
	void set_type (int64_t)                  { type = Tag::t_s64;    }

	virtual operator std::string              (void);
	virtual operator std::vector<std::string> (void);
	virtual operator double                   (void);
	virtual operator bool                     (void);
	virtual operator uint8_t                  (void);
	virtual operator  int8_t                  (void);
	virtual operator uint16_t                 (void);
	virtual operator  int16_t                 (void);
	virtual operator uint32_t                 (void);
	virtual operator  int32_t                 (void);
	virtual operator uint64_t                 (void);
	virtual operator  int64_t                 (void);

	std::string get_type_name (void);

	enum class Tag {
		t_unset,	// Empty
		t_string,	// Text
		t_list,		// A set of text strings
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

	enum Flags {
		Hide    = 1 << 0,	// Should not be displayed in properties dialog
		Dot     = 1 << 1,	// Should be displayed in graphviz diagram
		Size    = 1 << 2,	// This number can be abbreviated, e.g. MiB
		Percent = 1 << 3,	// Display the ratio of two numbers as a percentage
		Debug   = 1 << 4
	};

	std::string owner;
	std::string name;
	std::string desc;
	long flags = 0;
};

template <typename T>
class PropVar : public BaseProperty
{
public:
	PropVar (const char* owner, const char* name, T& v, const char* desc, int flags) :
		BaseProperty (owner, name, desc, flags),
		value(v)
	{
		set_type(v);
	}

	virtual ~PropVar()
	{
	}

	PropVar (void) = default;

	PropVar (const PropVar&  other) = default;
	PropVar (      PropVar&& other) = default;

	PropVar& operator= (PropVar&  other) = default;
	PropVar& operator= (PropVar&& other) = default;

	virtual operator T (void)
	{
		return value;
	}

protected:
	T& value;
};

template <typename T>
class PropFn : public BaseProperty
{
public:
	PropFn (const char* owner, const char* name, std::function<T(void)> fn, const char* desc, int flags) :
		BaseProperty (owner, name, desc, flags),
		fn(fn)
	{
		T dummy = {};
		set_type(dummy);
	}

	virtual ~PropFn()
	{
	}

	PropFn (void) = default;

	PropFn (const PropFn&  other) = default;
	PropFn (      PropFn&& other) = default;

	PropFn& operator= (PropFn&  other) = default;
	PropFn& operator= (PropFn&& other) = default;

	virtual operator T (void)
	{
		return fn();
	}

protected:
	std::function<T(void)> fn = nullptr;
};

class PropPercent : public BaseProperty
{
public:
	PropPercent (const char* owner, const char* name, PPtr& numerator, PPtr& denominator, const char* desc, int flags) :
		BaseProperty (owner, name, desc, flags),
		num(numerator),
		denom(denominator)
	{
		type = Tag::t_u8;	// Pretend to be a std::uint8_t
	}

	virtual ~PropPercent()
	{
	}

	PropPercent (void) = default;

	PropPercent (const PropPercent&  other) = default;
	PropPercent (      PropPercent&& other) = default;

	PropPercent& operator= (PropPercent&  other) = default;
	PropPercent& operator= (PropPercent&& other) = default;

	virtual operator std::uint8_t (void)
	{
		//XXX check num & denom
		double dn = (std::uint64_t) *num;
		double dd = (std::uint64_t) *denom;

		return (std::uint8_t) ((dn/dd*100) + 0.5);
	}

protected:
	PPtr num;
	PPtr denom;
};

class PropArray : public BaseProperty
{
public:
	PropArray (const char* owner, const char* name, std::vector<std::string>& vec, unsigned int index, const char* desc, int flags) :
		BaseProperty (owner, name, desc, flags),
		v(vec),
		index(index)
	{
		type = Tag::t_string;		// Pretend to be a std::string
	}

	virtual ~PropArray()
	{
	}

	PropArray (void) = default;

	PropArray (const PropArray&  other) = default;
	PropArray (      PropArray&& other) = default;

	PropArray& operator= (PropArray&  other) = default;
	PropArray& operator= (PropArray&& other) = default;

	virtual operator std::string (void)
	{
		if (index >= v.size())
			return "";

		return v[index];
	}

protected:
	std::vector<std::string>& v;
	unsigned int index = 0;
};


#endif // _PROPERTY_H_

