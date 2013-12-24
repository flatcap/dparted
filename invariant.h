/* Copyright (c) 2013 Richard Russon (FlatCap)
 *
 *  This program is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free Software
 *  Foundation; either version 2 of the License, or (at your option) any later
 *  version.
 *
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU Library General Public License for
 *  more details.
 *
 *  You should have received a copy of the GNU General Public License along with
 *  this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 *  Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <string>
#include <iostream>
#include <cstdint>

//XXX static_assert : sizeof(d_value) == sizeof(l_value)

/**
 * class Invariant
 */
class Invariant
{
public:
	Invariant (void)     = default;
	virtual ~Invariant() = default;

	Invariant (std::string value);		// typed constructors
	Invariant (const char* value);
	Invariant (double      value);
	Invariant (bool        value);
	Invariant (uint8_t     value);
	Invariant ( int8_t     value);
	Invariant (uint16_t    value);
	Invariant ( int16_t    value);
	Invariant (uint32_t    value);
	Invariant ( int32_t    value);
	Invariant (uint64_t    value);
	Invariant ( int64_t    value);

	Invariant (const Invariant&  v) = default;
	Invariant (      Invariant&& v) = default;

	Invariant& operator= (const Invariant&  v);
	Invariant& operator= (      Invariant&& v);

	Invariant& operator= (std::string);
	Invariant& operator= (double);
	Invariant& operator= (bool);
	Invariant& operator= (uint8_t);
	Invariant& operator= ( int8_t);
	Invariant& operator= (uint16_t);
	Invariant& operator= ( int16_t);
	Invariant& operator= (uint32_t);
	Invariant& operator= ( int32_t);
	Invariant& operator= (uint64_t);
	Invariant& operator= ( int64_t);

	operator std::string (void);		// cast Invariant to type
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
	friend std::ostream& operator<< (std::ostream& os, const Invariant* v);
	friend std::ostream& operator<< (std::ostream& os, const Invariant& v);

	friend bool operator== (const Invariant& lhs, const Invariant& rhs);
	friend bool operator<  (const Invariant& lhs, const Invariant& rhs);

	inline friend bool operator!= (const Invariant& lhs, const Invariant& rhs) { return !operator== (lhs,rhs); }
	inline friend bool operator>  (const Invariant& lhs, const Invariant& rhs) { return  operator<  (rhs,lhs); }
	inline friend bool operator<= (const Invariant& lhs, const Invariant& rhs) { return !operator>  (lhs,rhs); }
	inline friend bool operator>= (const Invariant& lhs, const Invariant& rhs) { return !operator<  (lhs,rhs); }

	union {
		double   d_value;
		uint64_t l_value = 0;
	};
	std::string s_value;
};


