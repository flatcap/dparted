/* Copyright (c) 2013 Richard Russon (FlatCap)
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

#include "variant.h"

/**
 * Variant (std::string)
 */
Variant::Variant (std::string value) :
	type (Tag::t_string)
{
	s_value = value;
}

/**
 * Variant (char*)
 */
Variant::Variant (const char* value) :
	type (Tag::t_string)
{
	s_value = value;
}

/**
 * Variant (double)
 */
Variant::Variant (double value) :
	type (Tag::t_double)
{
	d_value = value;
}

/**
 * Variant (bool)
 */
Variant::Variant (bool value) :
	type (Tag::t_bool)
{
	l_value = value;
}

/**
 * Variant (uint8_t)
 */
Variant::Variant (uint8_t value) :
	type (Tag::t_u8)
{
	l_value = value;
}

/**
 * Variant (int8_t)
 */
Variant::Variant (int8_t value) :
	type (Tag::t_s8)
{
	l_value = value;
}

/**
 * Variant (uint16_t)
 */
Variant::Variant (uint16_t value) :
	type (Tag::t_u16)
{
	l_value = value;
}

/**
 * Variant (int16_t)
 */
Variant::Variant (int16_t value) :
	type (Tag::t_s16)
{
	l_value = value;
}

/**
 * Variant (uint32_t)
 */
Variant::Variant (uint32_t value) :
	type (Tag::t_u32)
{
	l_value = value;
}

/**
 * Variant (int32_t)
 */
Variant::Variant (int32_t value) :
	type (Tag::t_s32)
{
	l_value = value;
}

/**
 * Variant (uint64_t)
 */
Variant::Variant (uint64_t value) :
	type (Tag::t_u64)
{
	l_value = value;
}

/**
 * Variant (int64_t)
 */
Variant::Variant (int64_t value) :
	type (Tag::t_s64)
{
	l_value = value;
}


/**
 * cast (string)
 */
Variant::operator std::string (void)
{
	switch (type) {
		case Variant::Tag::t_string:	return s_value;

		case Variant::Tag::t_double:	return std::to_string (d_value);

		case Variant::Tag::t_bool:	return std::to_string (         l_value);
		case Variant::Tag::t_u8:	return std::to_string (         l_value);
		case Variant::Tag::t_s8:	return std::to_string ((int64_t)l_value);
		case Variant::Tag::t_u16:	return std::to_string (         l_value);
		case Variant::Tag::t_s16:	return std::to_string ((int64_t)l_value);
		case Variant::Tag::t_u32:	return std::to_string (         l_value);
		case Variant::Tag::t_s32:	return std::to_string ((int64_t)l_value);
		case Variant::Tag::t_u64:	return std::to_string (         l_value);
		case Variant::Tag::t_s64:	return std::to_string ((int64_t)l_value);

		case Variant::Tag::t_unset:	return "";

		default:			throw std::runtime_error ("variant: unknown type");
	}
}

/**
 * cast (double)
 */
Variant::operator double()
{
	switch (type) {
		case Variant::Tag::t_double:	return d_value;

		case Variant::Tag::t_bool:	return l_value;

		case Variant::Tag::t_string:
		case Variant::Tag::t_u8:
		case Variant::Tag::t_s8:
		case Variant::Tag::t_u16:
		case Variant::Tag::t_s16:
		case Variant::Tag::t_u32:
		case Variant::Tag::t_s32:
		case Variant::Tag::t_u64:
		case Variant::Tag::t_s64:	throw std::runtime_error ("variant: wrong type");

		case Variant::Tag::t_unset:	return 0;

		default:			throw std::runtime_error ("variant: unknown type");
	}
}

/**
 * cast (bool)
 */
Variant::operator bool()
{
	switch (type) {
		case Variant::Tag::t_string:	return (!s_value.empty());

		case Variant::Tag::t_double:	return (d_value != 0);

		case Variant::Tag::t_bool:
		case Variant::Tag::t_u8:
		case Variant::Tag::t_s8:
		case Variant::Tag::t_u16:
		case Variant::Tag::t_s16:
		case Variant::Tag::t_u32:
		case Variant::Tag::t_s32:
		case Variant::Tag::t_u64:
		case Variant::Tag::t_s64:	return (l_value != 0);

		case Variant::Tag::t_unset:	return false;

		default:			throw std::runtime_error ("variant: unknown type");
	}
}

/**
 * cast (uint8_t)
 */
Variant::operator uint8_t()
{
	switch (type) {
		case Variant::Tag::t_bool:
		case Variant::Tag::t_u8:	return l_value;

		case Variant::Tag::t_s8:	throw std::runtime_error ("variant: wrong sign");

		case Variant::Tag::t_string:
		case Variant::Tag::t_double:	throw std::runtime_error ("variant: wrong type");

		case Variant::Tag::t_u16:
		case Variant::Tag::t_s16:
		case Variant::Tag::t_u32:
		case Variant::Tag::t_s32:
		case Variant::Tag::t_u64:
		case Variant::Tag::t_s64:	throw std::runtime_error ("variant: too big");

		case Variant::Tag::t_unset:	return 0;

		default:			throw std::runtime_error ("variant: unknown type");
	}
}

/**
 * cast (int8_t)
 */
Variant::operator int8_t()
{
	switch (type) {
		case Variant::Tag::t_bool:
		case Variant::Tag::t_s8:	return l_value;

		case Variant::Tag::t_u8:	throw std::runtime_error ("variant: wrong sign");

		case Variant::Tag::t_string:
		case Variant::Tag::t_double:	throw std::runtime_error ("variant: wrong type");

		case Variant::Tag::t_u16:
		case Variant::Tag::t_s16:
		case Variant::Tag::t_u32:
		case Variant::Tag::t_s32:
		case Variant::Tag::t_u64:
		case Variant::Tag::t_s64:	throw std::runtime_error ("variant: too big");

		case Variant::Tag::t_unset:	return 0;

		default:			throw std::runtime_error ("variant: unknown type");
	}
}

/**
 * cast (uint16_t)
 */
Variant::operator uint16_t()
{
	switch (type) {
		case Variant::Tag::t_bool:
		case Variant::Tag::t_u8:
		case Variant::Tag::t_u16:	return l_value;

		case Variant::Tag::t_s8:
		case Variant::Tag::t_s16:	throw std::runtime_error ("variant: wrong sign");

		case Variant::Tag::t_string:
		case Variant::Tag::t_double:	throw std::runtime_error ("variant: wrong type");

		case Variant::Tag::t_u32:
		case Variant::Tag::t_s32:
		case Variant::Tag::t_u64:
		case Variant::Tag::t_s64:	throw std::runtime_error ("variant: too big");

		case Variant::Tag::t_unset:	return 0;

		default:			throw std::runtime_error ("variant: unknown type");
	}
}

/**
 * cast (int16_t)
 */
Variant::operator int16_t()
{
	switch (type) {
		case Variant::Tag::t_bool:
		case Variant::Tag::t_s8:
		case Variant::Tag::t_s16:	return l_value;

		case Variant::Tag::t_u8:
		case Variant::Tag::t_u16:	throw std::runtime_error ("variant: wrong sign");

		case Variant::Tag::t_string:
		case Variant::Tag::t_double:	throw std::runtime_error ("variant: wrong type");

		case Variant::Tag::t_u32:
		case Variant::Tag::t_s32:
		case Variant::Tag::t_u64:
		case Variant::Tag::t_s64:	throw std::runtime_error ("variant: too big");

		case Variant::Tag::t_unset:	return 0;

		default:			throw std::runtime_error ("variant: unknown type");
	}
}

/**
 * cast (uint32_t)
 */
Variant::operator uint32_t()
{
	switch (type) {
		case Variant::Tag::t_bool:
		case Variant::Tag::t_u8:
		case Variant::Tag::t_u16:
		case Variant::Tag::t_u32:	return l_value;

		case Variant::Tag::t_s8:
		case Variant::Tag::t_s16:
		case Variant::Tag::t_s32:	throw std::runtime_error ("variant: wrong sign");

		case Variant::Tag::t_string:
		case Variant::Tag::t_double:	throw std::runtime_error ("variant: wrong type");

		case Variant::Tag::t_u64:
		case Variant::Tag::t_s64:	throw std::runtime_error ("variant: too big");

		case Variant::Tag::t_unset:	return 0;

		default:			throw std::runtime_error ("variant: unknown type");
	}
}

/**
 * cast (int32_t)
 */
Variant::operator int32_t()
{
	switch (type) {
		case Variant::Tag::t_bool:
		case Variant::Tag::t_s8:
		case Variant::Tag::t_s16:
		case Variant::Tag::t_s32:	return l_value;

		case Variant::Tag::t_u8:
		case Variant::Tag::t_u16:
		case Variant::Tag::t_u32:	throw std::runtime_error ("variant: wrong sign");

		case Variant::Tag::t_string:
		case Variant::Tag::t_double:	throw std::runtime_error ("variant: wrong type");

		case Variant::Tag::t_u64:
		case Variant::Tag::t_s64:	throw std::runtime_error ("variant: too big");

		case Variant::Tag::t_unset:	return 0;

		default:			throw std::runtime_error ("variant: unknown type");
	}
}

/**
 * cast (uint64_t)
 */
Variant::operator uint64_t()
{
	switch (type) {
		case Variant::Tag::t_bool:
		case Variant::Tag::t_u8:
		case Variant::Tag::t_u16:
		case Variant::Tag::t_u32:
		case Variant::Tag::t_u64:	return l_value;

		case Variant::Tag::t_s8:
		case Variant::Tag::t_s16:
		case Variant::Tag::t_s32:
		case Variant::Tag::t_s64:	throw std::runtime_error ("variant: wrong sign");

		case Variant::Tag::t_string:
		case Variant::Tag::t_double:	throw std::runtime_error ("variant: wrong type");

		case Variant::Tag::t_unset:	return 0;

		default:			throw std::runtime_error ("variant: unknown type");
	}
}

/**
 * cast (int64_t)
 */
Variant::operator int64_t()
{
	switch (type) {
		case Variant::Tag::t_bool:
		case Variant::Tag::t_s8:
		case Variant::Tag::t_s16:
		case Variant::Tag::t_s32:
		case Variant::Tag::t_s64:	return l_value;

		case Variant::Tag::t_u8:
		case Variant::Tag::t_u16:
		case Variant::Tag::t_u32:
		case Variant::Tag::t_u64:	throw std::runtime_error ("variant: wrong sign");

		case Variant::Tag::t_string:
		case Variant::Tag::t_double:	throw std::runtime_error ("variant: wrong type");

		case Variant::Tag::t_unset:	return 0;

		default:			throw std::runtime_error ("variant: unknown type");
	}
}


/**
 * operator<< (Variant*)
 */
std::ostream&
operator<< (std::ostream& os, const Variant* v)
{
	if (v)
		return operator<< (os, *v);
	else
		return os;
}

/**
 * operator<< (Variant&)
 */
std::ostream&
operator<< (std::ostream& os, const Variant& v)
{
	os << "V[";
	switch (v.type) {
		case Variant::Tag::t_unset:  os << "EMPTY";                              break;
		case Variant::Tag::t_string: os << "string," <<               v.s_value; break;
		case Variant::Tag::t_double: os << "double," <<               v.d_value; break;
		case Variant::Tag::t_bool:   os << "bool,"   <<               v.l_value; break;
		case Variant::Tag::t_u8:     os << "u8,"     <<               v.l_value; break;
		case Variant::Tag::t_s8:     os << "s8,"     << (signed long) v.l_value; break;
		case Variant::Tag::t_u16:    os << "u16,"    <<               v.l_value; break;
		case Variant::Tag::t_s16:    os << "s16,"    << (signed long) v.l_value; break;
		case Variant::Tag::t_u32:    os << "u32,"    <<               v.l_value; break;
		case Variant::Tag::t_s32:    os << "s32,"    << (signed long) v.l_value; break;
		case Variant::Tag::t_u64:    os << "u64,"    <<               v.l_value; break;
		case Variant::Tag::t_s64:    os << "s64,"    << (signed long) v.l_value; break;
	}
	os << "]";

	return os;
}


/**
 * bool operator==
 */
bool
operator== (const Variant& lhs, const Variant& rhs)
{
	if (lhs.type != rhs.type)
		return false;

	switch (lhs.type) {
		case Variant::Tag::t_string:	return (lhs.s_value == rhs.s_value);

		case Variant::Tag::t_double:	return (lhs.d_value == rhs.d_value);

		case Variant::Tag::t_bool:
		case Variant::Tag::t_u8:
		case Variant::Tag::t_s8:
		case Variant::Tag::t_u16:
		case Variant::Tag::t_s16:
		case Variant::Tag::t_u32:
		case Variant::Tag::t_s32:
		case Variant::Tag::t_u64:
		case Variant::Tag::t_s64:	return (lhs.l_value == rhs.l_value);

		case Variant::Tag::t_unset:	return true;

		default:			throw std::runtime_error ("variant: unknown type");
	}
}

/**
 * bool operator<
 */
bool
operator< (const Variant& lhs, const Variant& rhs)
{
	if (lhs.type != rhs.type)
		throw std::runtime_error ("variant: wrong type");

	switch (lhs.type) {
		case Variant::Tag::t_string:	return (lhs.s_value < rhs.s_value);

		case Variant::Tag::t_double:	return (lhs.d_value < rhs.d_value);

		case Variant::Tag::t_bool:
		case Variant::Tag::t_u8:
		case Variant::Tag::t_s8:
		case Variant::Tag::t_u16:
		case Variant::Tag::t_s16:
		case Variant::Tag::t_u32:
		case Variant::Tag::t_s32:
		case Variant::Tag::t_u64:
		case Variant::Tag::t_s64:	return (lhs.l_value < rhs.l_value);

		case Variant::Tag::t_unset:	return true;

		default:			throw std::runtime_error ("variant: unknown type");
	}
}


