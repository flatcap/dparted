/* Copyright (c) 2014 Richard Russon (FlatCap)
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.	See the GNU Library General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <string>
#include <stdexcept>
#include <cstdint>

#include "config.h"

/**
 * Config (std::string)
 */
Config::Config (std::string value) :
	type (Tag::t_string)
{
	s_value = value;
}

/**
 * Config (char*)
 */
Config::Config (const char* value) :
	type (Tag::t_string)
{
	s_value = value;
}

/**
 * Config (double)
 */
Config::Config (double value) :
	type (Tag::t_double)
{
	d_value = value;
}

/**
 * Config (bool)
 */
Config::Config (bool value) :
	type (Tag::t_bool)
{
	l_value = value;
}

/**
 * Config (uint8_t)
 */
Config::Config (uint8_t value) :
	type (Tag::t_u8)
{
	l_value = value;
}

/**
 * Config (int8_t)
 */
Config::Config (int8_t value) :
	type (Tag::t_s8)
{
	l_value = value;
}

/**
 * Config (uint16_t)
 */
Config::Config (uint16_t value) :
	type (Tag::t_u16)
{
	l_value = value;
}

/**
 * Config (int16_t)
 */
Config::Config (int16_t value) :
	type (Tag::t_s16)
{
	l_value = value;
}

/**
 * Config (uint32_t)
 */
Config::Config (uint32_t value) :
	type (Tag::t_u32)
{
	l_value = value;
}

/**
 * Config (int32_t)
 */
Config::Config (int32_t value) :
	type (Tag::t_s32)
{
	l_value = value;
}

/**
 * Config (uint64_t)
 */
Config::Config (uint64_t value) :
	type (Tag::t_u64)
{
	l_value = value;
}

/**
 * Config (int64_t)
 */
Config::Config (int64_t value) :
	type (Tag::t_s64)
{
	l_value = value;
}


/**
 * operator= (reference)
 */
Config& Config::operator= (const Config& v)
{
	if ((type == v.type) || (type == Tag::t_unset)) {
		type    = v.type;
		l_value = v.l_value;
		s_value = v.s_value;
	} else {
		throw "different type";
	}

	return *this;
}

/**
 * operator= (move)
 */
Config& Config::operator= (Config&& v)
{
	if ((type == v.type) || (type == Tag::t_unset)) {
		std::swap (type,    v.type);
		std::swap (l_value, v.l_value);
		std::swap (s_value, v.s_value);
	} else {
		throw "different type";
	}

	return *this;
}


/**
 * operator= (std::string)
 */
Config& Config::operator= (std::string value)
{
	switch (type) {
		case Config::Tag::t_unset:
			type = Config::Tag::t_string;
		case Config::Tag::t_string:
			s_value = value;
			break;
		default:
			throw std::runtime_error ("config: wrong type");
	}

	return *this;
}

/**
 * operator= (double)
 */
Config& Config::operator= (double value)
{
	switch (type) {
		case Config::Tag::t_unset:
			type = Config::Tag::t_double;
		case Config::Tag::t_double:
			d_value = value;
			break;
		default:
			throw std::runtime_error ("config: wrong type");
	}

	return *this;
}

/**
 * operator= (bool)
 */
Config& Config::operator= (bool value)
{
	switch (type) {
		case Config::Tag::t_unset:
			type = Config::Tag::t_bool;
		case Config::Tag::t_bool:
			l_value = value;
			break;
		default:
			throw std::runtime_error ("config: wrong type");
	}

	return *this;
}

/**
 * operator= (uint8_t)
 */
Config& Config::operator= (uint8_t value)
{
	switch (type) {
		case Config::Tag::t_unset:
			type = Config::Tag::t_u8;
		case Config::Tag::t_u8:
			l_value = value;
			break;
		default:
			throw std::runtime_error ("config: wrong type");
	}

	return *this;
}

/**
 * operator= (int8_t)
 */
Config& Config::operator= (int8_t value)
{
	switch (type) {
		case Config::Tag::t_unset:
			type = Config::Tag::t_s8;
		case Config::Tag::t_s8:
			l_value = value;
			break;
		default:
			throw std::runtime_error ("config: wrong type");
	}

	return *this;
}

/**
 * operator= (uint16_t)
 */
Config& Config::operator= (uint16_t value)
{
	switch (type) {
		case Config::Tag::t_unset:
			type = Config::Tag::t_u16;
		case Config::Tag::t_u16:
			l_value = value;
			break;
		default:
			throw std::runtime_error ("config: wrong type");
	}

	return *this;
}

/**
 * operator= (int16_t)
 */
Config& Config::operator= (int16_t value)
{
	switch (type) {
		case Config::Tag::t_unset:
			type = Config::Tag::t_s16;
		case Config::Tag::t_s16:
			l_value = value;
			break;
		default:
			throw std::runtime_error ("config: wrong type");
	}

	return *this;
}

/**
 * operator= (uint32_t)
 */
Config& Config::operator= (uint32_t value)
{
	switch (type) {
		case Config::Tag::t_unset:
			type = Config::Tag::t_u32;
		case Config::Tag::t_u32:
			l_value = value;
			break;
		default:
			throw std::runtime_error ("config: wrong type");
	}

	return *this;
}

/**
 * operator= (int32_t)
 */
Config& Config::operator= (int32_t value)
{
	switch (type) {
		case Config::Tag::t_unset:
			type = Config::Tag::t_s32;
		case Config::Tag::t_s32:
			l_value = value;
			break;
		default:
			throw std::runtime_error ("config: wrong type");
	}

	return *this;
}

/**
 * operator= (uint64_t)
 */
Config& Config::operator= (uint64_t value)
{
	switch (type) {
		case Config::Tag::t_unset:
			type = Config::Tag::t_u64;
		case Config::Tag::t_u64:
			l_value = value;
			break;
		default:
			throw std::runtime_error ("config: wrong type");
	}

	return *this;
}

/**
 * operator= (int64_t)
 */
Config& Config::operator= (int64_t value)
{
	switch (type) {
		case Config::Tag::t_unset:
			type = Config::Tag::t_s64;
		case Config::Tag::t_s64:
			l_value = value;
			break;
		default:
			throw std::runtime_error ("config: wrong type");
	}

	return *this;
}


/**
 * cast (string)
 */
Config::operator std::string (void)
{
	switch (type) {
		case Config::Tag::t_string:	return s_value;

		case Config::Tag::t_double:	return std::to_string (d_value);

		case Config::Tag::t_bool:	return std::to_string (          l_value);
		case Config::Tag::t_u8:		return std::to_string (          l_value);
		case Config::Tag::t_s8:		return std::to_string ((int64_t) l_value);
		case Config::Tag::t_u16:	return std::to_string (          l_value);
		case Config::Tag::t_s16:	return std::to_string ((int64_t) l_value);
		case Config::Tag::t_u32:	return std::to_string (          l_value);
		case Config::Tag::t_s32:	return std::to_string ((int64_t) l_value);
		case Config::Tag::t_u64:	return std::to_string (          l_value);
		case Config::Tag::t_s64:	return std::to_string ((int64_t) l_value);

		case Config::Tag::t_unset:	return "";

		default:			throw std::runtime_error ("config: unknown type");
	}
}

/**
 * cast (double)
 */
Config::operator double()
{
	switch (type) {
		case Config::Tag::t_double:	return d_value;

		case Config::Tag::t_bool:	return l_value;

		case Config::Tag::t_string:
		case Config::Tag::t_u8:
		case Config::Tag::t_s8:
		case Config::Tag::t_u16:
		case Config::Tag::t_s16:
		case Config::Tag::t_u32:
		case Config::Tag::t_s32:
		case Config::Tag::t_u64:
		case Config::Tag::t_s64:	throw std::runtime_error ("config: wrong type");

		case Config::Tag::t_unset:	return 0;

		default:			throw std::runtime_error ("config: unknown type");
	}
}

/**
 * cast (bool)
 */
Config::operator bool()
{
	switch (type) {
		case Config::Tag::t_string:	return (!s_value.empty());

		case Config::Tag::t_double:	return (d_value != 0);

		case Config::Tag::t_bool:
		case Config::Tag::t_u8:
		case Config::Tag::t_s8:
		case Config::Tag::t_u16:
		case Config::Tag::t_s16:
		case Config::Tag::t_u32:
		case Config::Tag::t_s32:
		case Config::Tag::t_u64:
		case Config::Tag::t_s64:	return (l_value != 0);

		case Config::Tag::t_unset:	return false;

		default:			throw std::runtime_error ("config: unknown type");
	}
}

/**
 * cast (uint8_t)
 */
Config::operator uint8_t()
{
	switch (type) {
		case Config::Tag::t_bool:
		case Config::Tag::t_u8:	return l_value;

		case Config::Tag::t_s8:	throw std::runtime_error ("config: wrong sign");

		case Config::Tag::t_string:
		case Config::Tag::t_double:	throw std::runtime_error ("config: wrong type");

		case Config::Tag::t_u16:
		case Config::Tag::t_s16:
		case Config::Tag::t_u32:
		case Config::Tag::t_s32:
		case Config::Tag::t_u64:
		case Config::Tag::t_s64:	throw std::runtime_error ("config: too big");

		case Config::Tag::t_unset:	return 0;

		default:			throw std::runtime_error ("config: unknown type");
	}
}

/**
 * cast (int8_t)
 */
Config::operator int8_t()
{
	switch (type) {
		case Config::Tag::t_bool:
		case Config::Tag::t_s8:	return l_value;

		case Config::Tag::t_u8:	throw std::runtime_error ("config: wrong sign");

		case Config::Tag::t_string:
		case Config::Tag::t_double:	throw std::runtime_error ("config: wrong type");

		case Config::Tag::t_u16:
		case Config::Tag::t_s16:
		case Config::Tag::t_u32:
		case Config::Tag::t_s32:
		case Config::Tag::t_u64:
		case Config::Tag::t_s64:	throw std::runtime_error ("config: too big");

		case Config::Tag::t_unset:	return 0;

		default:			throw std::runtime_error ("config: unknown type");
	}
}

/**
 * cast (uint16_t)
 */
Config::operator uint16_t()
{
	switch (type) {
		case Config::Tag::t_bool:
		case Config::Tag::t_u8:
		case Config::Tag::t_u16:	return l_value;

		case Config::Tag::t_s8:
		case Config::Tag::t_s16:	throw std::runtime_error ("config: wrong sign");

		case Config::Tag::t_string:
		case Config::Tag::t_double:	throw std::runtime_error ("config: wrong type");

		case Config::Tag::t_u32:
		case Config::Tag::t_s32:
		case Config::Tag::t_u64:
		case Config::Tag::t_s64:	throw std::runtime_error ("config: too big");

		case Config::Tag::t_unset:	return 0;

		default:			throw std::runtime_error ("config: unknown type");
	}
}

/**
 * cast (int16_t)
 */
Config::operator int16_t()
{
	switch (type) {
		case Config::Tag::t_bool:
		case Config::Tag::t_s8:
		case Config::Tag::t_s16:	return l_value;

		case Config::Tag::t_u8:
		case Config::Tag::t_u16:	throw std::runtime_error ("config: wrong sign");

		case Config::Tag::t_string:
		case Config::Tag::t_double:	throw std::runtime_error ("config: wrong type");

		case Config::Tag::t_u32:
		case Config::Tag::t_s32:
		case Config::Tag::t_u64:
		case Config::Tag::t_s64:	throw std::runtime_error ("config: too big");

		case Config::Tag::t_unset:	return 0;

		default:			throw std::runtime_error ("config: unknown type");
	}
}

/**
 * cast (uint32_t)
 */
Config::operator uint32_t()
{
	switch (type) {
		case Config::Tag::t_bool:
		case Config::Tag::t_u8:
		case Config::Tag::t_u16:
		case Config::Tag::t_u32:	return l_value;

		case Config::Tag::t_s8:
		case Config::Tag::t_s16:
		case Config::Tag::t_s32:	throw std::runtime_error ("config: wrong sign");

		case Config::Tag::t_string:
		case Config::Tag::t_double:	throw std::runtime_error ("config: wrong type");

		case Config::Tag::t_u64:
		case Config::Tag::t_s64:	throw std::runtime_error ("config: too big");

		case Config::Tag::t_unset:	return 0;

		default:			throw std::runtime_error ("config: unknown type");
	}
}

/**
 * cast (int32_t)
 */
Config::operator int32_t()
{
	switch (type) {
		case Config::Tag::t_bool:
		case Config::Tag::t_s8:
		case Config::Tag::t_s16:
		case Config::Tag::t_s32:	return l_value;

		case Config::Tag::t_u8:
		case Config::Tag::t_u16:
		case Config::Tag::t_u32:	throw std::runtime_error ("config: wrong sign");

		case Config::Tag::t_string:
		case Config::Tag::t_double:	throw std::runtime_error ("config: wrong type");

		case Config::Tag::t_u64:
		case Config::Tag::t_s64:	throw std::runtime_error ("config: too big");

		case Config::Tag::t_unset:	return 0;

		default:			throw std::runtime_error ("config: unknown type");
	}
}

/**
 * cast (uint64_t)
 */
Config::operator uint64_t()
{
	switch (type) {
		case Config::Tag::t_bool:
		case Config::Tag::t_u8:
		case Config::Tag::t_u16:
		case Config::Tag::t_u32:
		case Config::Tag::t_u64:	return l_value;

		case Config::Tag::t_s8:
		case Config::Tag::t_s16:
		case Config::Tag::t_s32:
		case Config::Tag::t_s64:	throw std::runtime_error ("config: wrong sign");

		case Config::Tag::t_string:
		case Config::Tag::t_double:	throw std::runtime_error ("config: wrong type");

		case Config::Tag::t_unset:	return 0;

		default:			throw std::runtime_error ("config: unknown type");
	}
}

/**
 * cast (int64_t)
 */
Config::operator int64_t()
{
	switch (type) {
		case Config::Tag::t_bool:
		case Config::Tag::t_s8:
		case Config::Tag::t_s16:
		case Config::Tag::t_s32:
		case Config::Tag::t_s64:	return l_value;

		case Config::Tag::t_u8:
		case Config::Tag::t_u16:
		case Config::Tag::t_u32:
		case Config::Tag::t_u64:	throw std::runtime_error ("config: wrong sign");

		case Config::Tag::t_string:
		case Config::Tag::t_double:	throw std::runtime_error ("config: wrong type");

		case Config::Tag::t_unset:	return 0;

		default:			throw std::runtime_error ("config: unknown type");
	}
}


/**
 * operator<< (Config*)
 */
std::ostream&
operator<< (std::ostream& os, const Config* v)
{
	if (v)
		return operator<< (os, *v);
	else
		return os;
}

/**
 * operator<< (Config&)
 */
std::ostream&
operator<< (std::ostream& os, const Config& v)
{
	os << "V[";
	switch (v.type) {
		case Config::Tag::t_unset:  os << "EMPTY";                              break;
		case Config::Tag::t_string: os << "string," <<               v.s_value; break;
		case Config::Tag::t_double: os << "double," <<               v.d_value; break;
		case Config::Tag::t_bool:   os << "bool,"   <<               v.l_value; break;
		case Config::Tag::t_u8:     os << "u8,"     <<               v.l_value; break;
		case Config::Tag::t_s8:     os << "s8,"     << (signed long) v.l_value; break;
		case Config::Tag::t_u16:    os << "u16,"    <<               v.l_value; break;
		case Config::Tag::t_s16:    os << "s16,"    << (signed long) v.l_value; break;
		case Config::Tag::t_u32:    os << "u32,"    <<               v.l_value; break;
		case Config::Tag::t_s32:    os << "s32,"    << (signed long) v.l_value; break;
		case Config::Tag::t_u64:    os << "u64,"    <<               v.l_value; break;
		case Config::Tag::t_s64:    os << "s64,"    << (signed long) v.l_value; break;
	}
	os << "]";

	return os;
}


/**
 * bool operator==
 */
bool
operator== (const Config& lhs, const Config& rhs)
{
	if (lhs.type != rhs.type)
		return false;

	switch (lhs.type) {
		case Config::Tag::t_string:	return (lhs.s_value == rhs.s_value);

		case Config::Tag::t_double:	return (lhs.d_value == rhs.d_value);

		case Config::Tag::t_bool:
		case Config::Tag::t_u8:
		case Config::Tag::t_s8:
		case Config::Tag::t_u16:
		case Config::Tag::t_s16:
		case Config::Tag::t_u32:
		case Config::Tag::t_s32:
		case Config::Tag::t_u64:
		case Config::Tag::t_s64:	return (lhs.l_value == rhs.l_value);

		case Config::Tag::t_unset:	return true;

		default:			throw std::runtime_error ("config: unknown type");
	}
}

/**
 * bool operator<
 */
bool
operator< (const Config& lhs, const Config& rhs)
{
	if (lhs.type != rhs.type)
		throw std::runtime_error ("config: wrong type");

	switch (lhs.type) {
		case Config::Tag::t_string:	return (lhs.s_value < rhs.s_value);

		case Config::Tag::t_double:	return (lhs.d_value < rhs.d_value);

		case Config::Tag::t_bool:
		case Config::Tag::t_u8:
		case Config::Tag::t_s8:
		case Config::Tag::t_u16:
		case Config::Tag::t_s16:
		case Config::Tag::t_u32:
		case Config::Tag::t_s32:
		case Config::Tag::t_u64:
		case Config::Tag::t_s64:	return (lhs.l_value < rhs.l_value);

		case Config::Tag::t_unset:	return true;

		default:			throw std::runtime_error ("config: unknown type");
	}
}


