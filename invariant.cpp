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

#include "invariant.h"

/**
 * Invariant (std::string)
 */
Invariant::Invariant (std::string value) :
	type (Tag::t_string)
{
	s_value = value;
}

/**
 * Invariant (char*)
 */
Invariant::Invariant (const char* value) :
	type (Tag::t_string)
{
	s_value = value;
}

/**
 * Invariant (double)
 */
Invariant::Invariant (double value) :
	type (Tag::t_double)
{
	d_value = value;
}

/**
 * Invariant (bool)
 */
Invariant::Invariant (bool value) :
	type (Tag::t_bool)
{
	l_value = value;
}

/**
 * Invariant (uint8_t)
 */
Invariant::Invariant (uint8_t value) :
	type (Tag::t_u8)
{
	l_value = value;
}

/**
 * Invariant (int8_t)
 */
Invariant::Invariant (int8_t value) :
	type (Tag::t_s8)
{
	l_value = value;
}

/**
 * Invariant (uint16_t)
 */
Invariant::Invariant (uint16_t value) :
	type (Tag::t_u16)
{
	l_value = value;
}

/**
 * Invariant (int16_t)
 */
Invariant::Invariant (int16_t value) :
	type (Tag::t_s16)
{
	l_value = value;
}

/**
 * Invariant (uint32_t)
 */
Invariant::Invariant (uint32_t value) :
	type (Tag::t_u32)
{
	l_value = value;
}

/**
 * Invariant (int32_t)
 */
Invariant::Invariant (int32_t value) :
	type (Tag::t_s32)
{
	l_value = value;
}

/**
 * Invariant (uint64_t)
 */
Invariant::Invariant (uint64_t value) :
	type (Tag::t_u64)
{
	l_value = value;
}

/**
 * Invariant (int64_t)
 */
Invariant::Invariant (int64_t value) :
	type (Tag::t_s64)
{
	l_value = value;
}


/**
 * operator= (reference)
 */
Invariant& Invariant::operator= (const Invariant& v)
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
Invariant& Invariant::operator= (Invariant&& v)
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
Invariant& Invariant::operator= (std::string value)
{
	switch (type) {
		case Invariant::Tag::t_unset:
			type = Invariant::Tag::t_string;
		case Invariant::Tag::t_string:
			s_value = value;
			break;
		default:
			throw std::runtime_error ("variant: wrong type");
	}

	return *this;
}

/**
 * operator= (double)
 */
Invariant& Invariant::operator= (double value)
{
	switch (type) {
		case Invariant::Tag::t_unset:
			type = Invariant::Tag::t_double;
		case Invariant::Tag::t_double:
			d_value = value;
			break;
		default:
			throw std::runtime_error ("variant: wrong type");
	}

	return *this;
}

/**
 * operator= (bool)
 */
Invariant& Invariant::operator= (bool value)
{
	switch (type) {
		case Invariant::Tag::t_unset:
			type = Invariant::Tag::t_bool;
		case Invariant::Tag::t_bool:
			l_value = value;
			break;
		default:
			throw std::runtime_error ("variant: wrong type");
	}

	return *this;
}

/**
 * operator= (uint8_t)
 */
Invariant& Invariant::operator= (uint8_t value)
{
	switch (type) {
		case Invariant::Tag::t_unset:
			type = Invariant::Tag::t_u8;
		case Invariant::Tag::t_u8:
			l_value = value;
			break;
		default:
			throw std::runtime_error ("variant: wrong type");
	}

	return *this;
}

/**
 * operator= (int8_t)
 */
Invariant& Invariant::operator= (int8_t value)
{
	switch (type) {
		case Invariant::Tag::t_unset:
			type = Invariant::Tag::t_s8;
		case Invariant::Tag::t_s8:
			l_value = value;
			break;
		default:
			throw std::runtime_error ("variant: wrong type");
	}

	return *this;
}

/**
 * operator= (uint16_t)
 */
Invariant& Invariant::operator= (uint16_t value)
{
	switch (type) {
		case Invariant::Tag::t_unset:
			type = Invariant::Tag::t_u16;
		case Invariant::Tag::t_u16:
			l_value = value;
			break;
		default:
			throw std::runtime_error ("variant: wrong type");
	}

	return *this;
}

/**
 * operator= (int16_t)
 */
Invariant& Invariant::operator= (int16_t value)
{
	switch (type) {
		case Invariant::Tag::t_unset:
			type = Invariant::Tag::t_s16;
		case Invariant::Tag::t_s16:
			l_value = value;
			break;
		default:
			throw std::runtime_error ("variant: wrong type");
	}

	return *this;
}

/**
 * operator= (uint32_t)
 */
Invariant& Invariant::operator= (uint32_t value)
{
	switch (type) {
		case Invariant::Tag::t_unset:
			type = Invariant::Tag::t_u32;
		case Invariant::Tag::t_u32:
			l_value = value;
			break;
		default:
			throw std::runtime_error ("variant: wrong type");
	}

	return *this;
}

/**
 * operator= (int32_t)
 */
Invariant& Invariant::operator= (int32_t value)
{
	switch (type) {
		case Invariant::Tag::t_unset:
			type = Invariant::Tag::t_s32;
		case Invariant::Tag::t_s32:
			l_value = value;
			break;
		default:
			throw std::runtime_error ("variant: wrong type");
	}

	return *this;
}

/**
 * operator= (uint64_t)
 */
Invariant& Invariant::operator= (uint64_t value)
{
	switch (type) {
		case Invariant::Tag::t_unset:
			type = Invariant::Tag::t_u64;
		case Invariant::Tag::t_u64:
			l_value = value;
			break;
		default:
			throw std::runtime_error ("variant: wrong type");
	}

	return *this;
}

/**
 * operator= (int64_t)
 */
Invariant& Invariant::operator= (int64_t value)
{
	switch (type) {
		case Invariant::Tag::t_unset:
			type = Invariant::Tag::t_s64;
		case Invariant::Tag::t_s64:
			l_value = value;
			break;
		default:
			throw std::runtime_error ("variant: wrong type");
	}

	return *this;
}


/**
 * cast (string)
 */
Invariant::operator std::string (void)
{
	switch (type) {
		case Invariant::Tag::t_string:	return s_value;

		case Invariant::Tag::t_double:	return std::to_string (d_value);

		case Invariant::Tag::t_bool:	return std::to_string (         l_value);
		case Invariant::Tag::t_u8:	return std::to_string (         l_value);
		case Invariant::Tag::t_s8:	return std::to_string ((int64_t)l_value);
		case Invariant::Tag::t_u16:	return std::to_string (         l_value);
		case Invariant::Tag::t_s16:	return std::to_string ((int64_t)l_value);
		case Invariant::Tag::t_u32:	return std::to_string (         l_value);
		case Invariant::Tag::t_s32:	return std::to_string ((int64_t)l_value);
		case Invariant::Tag::t_u64:	return std::to_string (         l_value);
		case Invariant::Tag::t_s64:	return std::to_string ((int64_t)l_value);

		case Invariant::Tag::t_unset:	return "";

		default:			throw std::runtime_error ("variant: unknown type");
	}
}

/**
 * cast (double)
 */
Invariant::operator double()
{
	switch (type) {
		case Invariant::Tag::t_double:	return d_value;

		case Invariant::Tag::t_bool:	return l_value;

		case Invariant::Tag::t_string:
		case Invariant::Tag::t_u8:
		case Invariant::Tag::t_s8:
		case Invariant::Tag::t_u16:
		case Invariant::Tag::t_s16:
		case Invariant::Tag::t_u32:
		case Invariant::Tag::t_s32:
		case Invariant::Tag::t_u64:
		case Invariant::Tag::t_s64:	throw std::runtime_error ("variant: wrong type");

		case Invariant::Tag::t_unset:	return 0;

		default:			throw std::runtime_error ("variant: unknown type");
	}
}

/**
 * cast (bool)
 */
Invariant::operator bool()
{
	switch (type) {
		case Invariant::Tag::t_string:	return (!s_value.empty());

		case Invariant::Tag::t_double:	return (d_value != 0);

		case Invariant::Tag::t_bool:
		case Invariant::Tag::t_u8:
		case Invariant::Tag::t_s8:
		case Invariant::Tag::t_u16:
		case Invariant::Tag::t_s16:
		case Invariant::Tag::t_u32:
		case Invariant::Tag::t_s32:
		case Invariant::Tag::t_u64:
		case Invariant::Tag::t_s64:	return (l_value != 0);

		case Invariant::Tag::t_unset:	return false;

		default:			throw std::runtime_error ("variant: unknown type");
	}
}

/**
 * cast (uint8_t)
 */
Invariant::operator uint8_t()
{
	switch (type) {
		case Invariant::Tag::t_bool:
		case Invariant::Tag::t_u8:	return l_value;

		case Invariant::Tag::t_s8:	throw std::runtime_error ("variant: wrong sign");

		case Invariant::Tag::t_string:
		case Invariant::Tag::t_double:	throw std::runtime_error ("variant: wrong type");

		case Invariant::Tag::t_u16:
		case Invariant::Tag::t_s16:
		case Invariant::Tag::t_u32:
		case Invariant::Tag::t_s32:
		case Invariant::Tag::t_u64:
		case Invariant::Tag::t_s64:	throw std::runtime_error ("variant: too big");

		case Invariant::Tag::t_unset:	return 0;

		default:			throw std::runtime_error ("variant: unknown type");
	}
}

/**
 * cast (int8_t)
 */
Invariant::operator int8_t()
{
	switch (type) {
		case Invariant::Tag::t_bool:
		case Invariant::Tag::t_s8:	return l_value;

		case Invariant::Tag::t_u8:	throw std::runtime_error ("variant: wrong sign");

		case Invariant::Tag::t_string:
		case Invariant::Tag::t_double:	throw std::runtime_error ("variant: wrong type");

		case Invariant::Tag::t_u16:
		case Invariant::Tag::t_s16:
		case Invariant::Tag::t_u32:
		case Invariant::Tag::t_s32:
		case Invariant::Tag::t_u64:
		case Invariant::Tag::t_s64:	throw std::runtime_error ("variant: too big");

		case Invariant::Tag::t_unset:	return 0;

		default:			throw std::runtime_error ("variant: unknown type");
	}
}

/**
 * cast (uint16_t)
 */
Invariant::operator uint16_t()
{
	switch (type) {
		case Invariant::Tag::t_bool:
		case Invariant::Tag::t_u8:
		case Invariant::Tag::t_u16:	return l_value;

		case Invariant::Tag::t_s8:
		case Invariant::Tag::t_s16:	throw std::runtime_error ("variant: wrong sign");

		case Invariant::Tag::t_string:
		case Invariant::Tag::t_double:	throw std::runtime_error ("variant: wrong type");

		case Invariant::Tag::t_u32:
		case Invariant::Tag::t_s32:
		case Invariant::Tag::t_u64:
		case Invariant::Tag::t_s64:	throw std::runtime_error ("variant: too big");

		case Invariant::Tag::t_unset:	return 0;

		default:			throw std::runtime_error ("variant: unknown type");
	}
}

/**
 * cast (int16_t)
 */
Invariant::operator int16_t()
{
	switch (type) {
		case Invariant::Tag::t_bool:
		case Invariant::Tag::t_s8:
		case Invariant::Tag::t_s16:	return l_value;

		case Invariant::Tag::t_u8:
		case Invariant::Tag::t_u16:	throw std::runtime_error ("variant: wrong sign");

		case Invariant::Tag::t_string:
		case Invariant::Tag::t_double:	throw std::runtime_error ("variant: wrong type");

		case Invariant::Tag::t_u32:
		case Invariant::Tag::t_s32:
		case Invariant::Tag::t_u64:
		case Invariant::Tag::t_s64:	throw std::runtime_error ("variant: too big");

		case Invariant::Tag::t_unset:	return 0;

		default:			throw std::runtime_error ("variant: unknown type");
	}
}

/**
 * cast (uint32_t)
 */
Invariant::operator uint32_t()
{
	switch (type) {
		case Invariant::Tag::t_bool:
		case Invariant::Tag::t_u8:
		case Invariant::Tag::t_u16:
		case Invariant::Tag::t_u32:	return l_value;

		case Invariant::Tag::t_s8:
		case Invariant::Tag::t_s16:
		case Invariant::Tag::t_s32:	throw std::runtime_error ("variant: wrong sign");

		case Invariant::Tag::t_string:
		case Invariant::Tag::t_double:	throw std::runtime_error ("variant: wrong type");

		case Invariant::Tag::t_u64:
		case Invariant::Tag::t_s64:	throw std::runtime_error ("variant: too big");

		case Invariant::Tag::t_unset:	return 0;

		default:			throw std::runtime_error ("variant: unknown type");
	}
}

/**
 * cast (int32_t)
 */
Invariant::operator int32_t()
{
	switch (type) {
		case Invariant::Tag::t_bool:
		case Invariant::Tag::t_s8:
		case Invariant::Tag::t_s16:
		case Invariant::Tag::t_s32:	return l_value;

		case Invariant::Tag::t_u8:
		case Invariant::Tag::t_u16:
		case Invariant::Tag::t_u32:	throw std::runtime_error ("variant: wrong sign");

		case Invariant::Tag::t_string:
		case Invariant::Tag::t_double:	throw std::runtime_error ("variant: wrong type");

		case Invariant::Tag::t_u64:
		case Invariant::Tag::t_s64:	throw std::runtime_error ("variant: too big");

		case Invariant::Tag::t_unset:	return 0;

		default:			throw std::runtime_error ("variant: unknown type");
	}
}

/**
 * cast (uint64_t)
 */
Invariant::operator uint64_t()
{
	switch (type) {
		case Invariant::Tag::t_bool:
		case Invariant::Tag::t_u8:
		case Invariant::Tag::t_u16:
		case Invariant::Tag::t_u32:
		case Invariant::Tag::t_u64:	return l_value;

		case Invariant::Tag::t_s8:
		case Invariant::Tag::t_s16:
		case Invariant::Tag::t_s32:
		case Invariant::Tag::t_s64:	throw std::runtime_error ("variant: wrong sign");

		case Invariant::Tag::t_string:
		case Invariant::Tag::t_double:	throw std::runtime_error ("variant: wrong type");

		case Invariant::Tag::t_unset:	return 0;

		default:			throw std::runtime_error ("variant: unknown type");
	}
}

/**
 * cast (int64_t)
 */
Invariant::operator int64_t()
{
	switch (type) {
		case Invariant::Tag::t_bool:
		case Invariant::Tag::t_s8:
		case Invariant::Tag::t_s16:
		case Invariant::Tag::t_s32:
		case Invariant::Tag::t_s64:	return l_value;

		case Invariant::Tag::t_u8:
		case Invariant::Tag::t_u16:
		case Invariant::Tag::t_u32:
		case Invariant::Tag::t_u64:	throw std::runtime_error ("variant: wrong sign");

		case Invariant::Tag::t_string:
		case Invariant::Tag::t_double:	throw std::runtime_error ("variant: wrong type");

		case Invariant::Tag::t_unset:	return 0;

		default:			throw std::runtime_error ("variant: unknown type");
	}
}


/**
 * operator<< (Invariant*)
 */
std::ostream&
operator<< (std::ostream& os, const Invariant* v)
{
	if (v)
		return operator<< (os, *v);
	else
		return os;
}

/**
 * operator<< (Invariant&)
 */
std::ostream&
operator<< (std::ostream& os, const Invariant& v)
{
	os << "V[";
	switch (v.type) {
		case Invariant::Tag::t_unset:  os << "EMPTY";                              break;
		case Invariant::Tag::t_string: os << "string," <<               v.s_value; break;
		case Invariant::Tag::t_double: os << "double," <<               v.d_value; break;
		case Invariant::Tag::t_bool:   os << "bool,"   <<               v.l_value; break;
		case Invariant::Tag::t_u8:     os << "u8,"     <<               v.l_value; break;
		case Invariant::Tag::t_s8:     os << "s8,"     << (signed long) v.l_value; break;
		case Invariant::Tag::t_u16:    os << "u16,"    <<               v.l_value; break;
		case Invariant::Tag::t_s16:    os << "s16,"    << (signed long) v.l_value; break;
		case Invariant::Tag::t_u32:    os << "u32,"    <<               v.l_value; break;
		case Invariant::Tag::t_s32:    os << "s32,"    << (signed long) v.l_value; break;
		case Invariant::Tag::t_u64:    os << "u64,"    <<               v.l_value; break;
		case Invariant::Tag::t_s64:    os << "s64,"    << (signed long) v.l_value; break;
	}
	os << "]";

	return os;
}


/**
 * bool operator==
 */
bool
operator== (const Invariant& lhs, const Invariant& rhs)
{
	if (lhs.type != rhs.type)
		return false;

	switch (lhs.type) {
		case Invariant::Tag::t_string:	return (lhs.s_value == rhs.s_value);

		case Invariant::Tag::t_double:	return (lhs.d_value == rhs.d_value);

		case Invariant::Tag::t_bool:
		case Invariant::Tag::t_u8:
		case Invariant::Tag::t_s8:
		case Invariant::Tag::t_u16:
		case Invariant::Tag::t_s16:
		case Invariant::Tag::t_u32:
		case Invariant::Tag::t_s32:
		case Invariant::Tag::t_u64:
		case Invariant::Tag::t_s64:	return (lhs.l_value == rhs.l_value);

		case Invariant::Tag::t_unset:	return true;

		default:			throw std::runtime_error ("variant: unknown type");
	}
}

/**
 * bool operator<
 */
bool
operator< (const Invariant& lhs, const Invariant& rhs)
{
	if (lhs.type != rhs.type)
		throw std::runtime_error ("variant: wrong type");

	switch (lhs.type) {
		case Invariant::Tag::t_string:	return (lhs.s_value < rhs.s_value);

		case Invariant::Tag::t_double:	return (lhs.d_value < rhs.d_value);

		case Invariant::Tag::t_bool:
		case Invariant::Tag::t_u8:
		case Invariant::Tag::t_s8:
		case Invariant::Tag::t_u16:
		case Invariant::Tag::t_s16:
		case Invariant::Tag::t_u32:
		case Invariant::Tag::t_s32:
		case Invariant::Tag::t_u64:
		case Invariant::Tag::t_s64:	return (lhs.l_value < rhs.l_value);

		case Invariant::Tag::t_unset:	return true;

		default:			throw std::runtime_error ("variant: unknown type");
	}
}


