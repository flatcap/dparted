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

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <string>
#include <iostream>
#include <cstdint>
#include <memory>

//XXX static_assert : sizeof (d_value) == sizeof (l_value)

class Config;

typedef std::shared_ptr<Config> ConfigPtr;

class Config
{
public:
	Config (void)     = default;
	virtual ~Config() = default;

	Config (std::string value);		// typed constructors
	Config (const char* value);
	Config (double      value);
	Config (bool        value);
	Config (uint8_t     value);
	Config ( int8_t     value);
	Config (uint16_t    value);
	Config ( int16_t    value);
	Config (uint32_t    value);
	Config ( int32_t    value);
	Config (uint64_t    value);
	Config ( int64_t    value);

	Config (const Config&  v) = default;
	Config (      Config&& v) = default;

	Config& operator= (const Config&  v);
	Config& operator= (      Config&& v);

	Config& operator= (std::string);
	Config& operator= (double);
	Config& operator= (bool);
	Config& operator= (uint8_t);
	Config& operator= ( int8_t);
	Config& operator= (uint16_t);
	Config& operator= ( int16_t);
	Config& operator= (uint32_t);
	Config& operator= ( int32_t);
	Config& operator= (uint64_t);
	Config& operator= ( int64_t);

	operator std::string (void);		// cast Config to type
	operator double      (void);
	operator bool        (void);
	operator uint8_t     (void);
	operator  int8_t     (void);
	operator uint16_t    (void);
	operator  int16_t    (void);
	operator uint32_t    (void);
	operator  int32_t    (void);
	operator uint64_t    (void);
	operator  int64_t    (void);

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

protected:
	friend std::ostream& operator<< (std::ostream& os, const Config* v);
	friend std::ostream& operator<< (std::ostream& os, const Config& v);

	friend bool operator== (const Config& lhs, const Config& rhs);
	friend bool operator<  (const Config& lhs, const Config& rhs);

	inline friend bool operator!= (const Config& lhs, const Config& rhs) { return !operator== (lhs,rhs); }
	inline friend bool operator>  (const Config& lhs, const Config& rhs) { return  operator<  (rhs,lhs); }
	inline friend bool operator<= (const Config& lhs, const Config& rhs) { return !operator>  (lhs,rhs); }
	inline friend bool operator>= (const Config& lhs, const Config& rhs) { return !operator<  (lhs,rhs); }

	union {
		double   d_value;
		uint64_t l_value = 0;
	};
	std::string s_value;
};


#endif // _CONFIG_H_

