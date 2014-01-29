/* Copyright (c) 2014 Richard Russon (FlatCap)
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

#include "property.h"

/**
 * string
 */
BaseProperty::operator std::string (void)
{
	switch (type) {
		case BaseProperty::Tag::t_string:	return operator std::string();

		case BaseProperty::Tag::t_double:	return std::to_string (operator double());
		case BaseProperty::Tag::t_bool:		return std::to_string (operator bool());
		case BaseProperty::Tag::t_u8:		return std::to_string (operator uint8_t());
		case BaseProperty::Tag::t_s8:		return std::to_string (operator int8_t());
		case BaseProperty::Tag::t_u16:		return std::to_string (operator uint16_t());
		case BaseProperty::Tag::t_s16:		return std::to_string (operator int16_t());
		case BaseProperty::Tag::t_u32:		return std::to_string (operator uint32_t());
		case BaseProperty::Tag::t_s32:		return std::to_string (operator int32_t());
		case BaseProperty::Tag::t_u64:		return std::to_string (operator uint64_t());
		case BaseProperty::Tag::t_s64:		return std::to_string (operator int64_t());

		case BaseProperty::Tag::t_unset:	throw std::runtime_error ("property: not set");

		default:				throw std::runtime_error ("property: unknown type");
	}
}

/**
 * double
 */
BaseProperty::operator double (void)
{
	switch (type) {
		case BaseProperty::Tag::t_double:	return operator double();

		case BaseProperty::Tag::t_bool:		return operator bool();

		case BaseProperty::Tag::t_string:
		case BaseProperty::Tag::t_u8:
		case BaseProperty::Tag::t_s8:
		case BaseProperty::Tag::t_u16:
		case BaseProperty::Tag::t_s16:
		case BaseProperty::Tag::t_u32:
		case BaseProperty::Tag::t_s32:
		case BaseProperty::Tag::t_u64:
		case BaseProperty::Tag::t_s64:		throw std::runtime_error ("property: wrong type");

		case BaseProperty::Tag::t_unset:	throw std::runtime_error ("property: not set");

		default:				throw std::runtime_error ("property: unknown type");
	}
}

/**
 * bool
 */
BaseProperty::operator bool (void)
{
	switch (type) {
		case BaseProperty::Tag::t_string:	return (!operator std::string().empty());

		case BaseProperty::Tag::t_double:	return (operator double()   != 0);
		case BaseProperty::Tag::t_bool:		return (operator bool()     != 0);
		case BaseProperty::Tag::t_u8:		return (operator uint8_t()  != 0);
		case BaseProperty::Tag::t_s8:		return (operator int8_t()   != 0);
		case BaseProperty::Tag::t_u16:		return (operator uint16_t() != 0);
		case BaseProperty::Tag::t_s16:		return (operator int16_t()  != 0);
		case BaseProperty::Tag::t_u32:		return (operator uint32_t() != 0);
		case BaseProperty::Tag::t_s32:		return (operator int32_t()  != 0);
		case BaseProperty::Tag::t_u64:		return (operator uint64_t() != 0);
		case BaseProperty::Tag::t_s64:		return (operator int64_t()  != 0);

		case BaseProperty::Tag::t_unset:	throw std::runtime_error ("property: not set");

		default:				throw std::runtime_error ("property: unknown type");
	}
}

/**
 * uint8_t
 */
BaseProperty::operator uint8_t (void)
{
	switch (type) {
		case BaseProperty::Tag::t_bool:		return operator bool();
		case BaseProperty::Tag::t_u8:		return operator uint8_t();

		case BaseProperty::Tag::t_s8:		throw std::runtime_error ("property: wrong sign");

		case BaseProperty::Tag::t_string:
		case BaseProperty::Tag::t_double:	throw std::runtime_error ("property: wrong type");

		case BaseProperty::Tag::t_u16:
		case BaseProperty::Tag::t_s16:
		case BaseProperty::Tag::t_u32:
		case BaseProperty::Tag::t_s32:
		case BaseProperty::Tag::t_u64:
		case BaseProperty::Tag::t_s64:		throw std::runtime_error ("property: too big");

		case BaseProperty::Tag::t_unset:	throw std::runtime_error ("property: not set");

		default:				throw std::runtime_error ("property: unknown type");
	}
}

/**
 * int8_t
 */
BaseProperty::operator int8_t (void)
{
	switch (type) {
		case BaseProperty::Tag::t_bool:		return operator bool();
		case BaseProperty::Tag::t_s8:		return operator int8_t();

		case BaseProperty::Tag::t_u8:		throw std::runtime_error ("property: wrong sign");

		case BaseProperty::Tag::t_string:
		case BaseProperty::Tag::t_double:	throw std::runtime_error ("property: wrong type");

		case BaseProperty::Tag::t_u16:
		case BaseProperty::Tag::t_s16:
		case BaseProperty::Tag::t_u32:
		case BaseProperty::Tag::t_s32:
		case BaseProperty::Tag::t_u64:
		case BaseProperty::Tag::t_s64:		throw std::runtime_error ("property: too big");

		case BaseProperty::Tag::t_unset:	throw std::runtime_error ("property: not set");

		default:				throw std::runtime_error ("property: unknown type");
	}
}

/**
 * uint16_t
 */
BaseProperty::operator uint16_t (void)
{
	switch (type) {
		case BaseProperty::Tag::t_bool:		return operator bool();
		case BaseProperty::Tag::t_u8:		return operator uint8_t();
		case BaseProperty::Tag::t_u16:		return operator uint16_t();

		case BaseProperty::Tag::t_s8:
		case BaseProperty::Tag::t_s16:		throw std::runtime_error ("property: wrong sign");

		case BaseProperty::Tag::t_string:
		case BaseProperty::Tag::t_double:	throw std::runtime_error ("property: wrong type");

		case BaseProperty::Tag::t_u32:
		case BaseProperty::Tag::t_s32:
		case BaseProperty::Tag::t_u64:
		case BaseProperty::Tag::t_s64:		throw std::runtime_error ("property: too big");

		case BaseProperty::Tag::t_unset:	throw std::runtime_error ("property: not set");

		default:				throw std::runtime_error ("property: unknown type");
	}
}

/**
 * int16_t
 */
BaseProperty::operator int16_t (void)
{
	switch (type) {
		case BaseProperty::Tag::t_bool:		return operator bool();
		case BaseProperty::Tag::t_s8:		return operator int8_t();
		case BaseProperty::Tag::t_s16:		return operator int16_t();

		case BaseProperty::Tag::t_u8:
		case BaseProperty::Tag::t_u16:		throw std::runtime_error ("property: wrong sign");

		case BaseProperty::Tag::t_string:
		case BaseProperty::Tag::t_double:	throw std::runtime_error ("property: wrong type");

		case BaseProperty::Tag::t_u32:
		case BaseProperty::Tag::t_s32:
		case BaseProperty::Tag::t_u64:
		case BaseProperty::Tag::t_s64:		throw std::runtime_error ("property: too big");

		case BaseProperty::Tag::t_unset:	throw std::runtime_error ("property: not set");

		default:				throw std::runtime_error ("property: unknown type");
	}
}

/**
 * uint32_t
 */
BaseProperty::operator uint32_t (void)
{
	switch (type) {
		case BaseProperty::Tag::t_bool:		return operator bool();
		case BaseProperty::Tag::t_u8:		return operator uint8_t();
		case BaseProperty::Tag::t_u16:		return operator uint16_t();
		case BaseProperty::Tag::t_u32:		return operator uint32_t();

		case BaseProperty::Tag::t_s8:
		case BaseProperty::Tag::t_s16:
		case BaseProperty::Tag::t_s32:		throw std::runtime_error ("property: wrong sign");

		case BaseProperty::Tag::t_string:
		case BaseProperty::Tag::t_double:	throw std::runtime_error ("property: wrong type");

		case BaseProperty::Tag::t_u64:
		case BaseProperty::Tag::t_s64:		throw std::runtime_error ("property: too big");

		case BaseProperty::Tag::t_unset:	throw std::runtime_error ("property: not set");

		default:				throw std::runtime_error ("property: unknown type");
	}
}

/**
 * int32_t
 */
BaseProperty::operator int32_t (void)
{
	switch (type) {
		case BaseProperty::Tag::t_bool:		return operator bool();
		case BaseProperty::Tag::t_s8:		return operator int8_t();
		case BaseProperty::Tag::t_s16:		return operator int16_t();
		case BaseProperty::Tag::t_s32:		return operator int32_t();

		case BaseProperty::Tag::t_u8:
		case BaseProperty::Tag::t_u16:
		case BaseProperty::Tag::t_u32:		throw std::runtime_error ("property: wrong sign");

		case BaseProperty::Tag::t_string:
		case BaseProperty::Tag::t_double:	throw std::runtime_error ("property: wrong type");

		case BaseProperty::Tag::t_u64:
		case BaseProperty::Tag::t_s64:		throw std::runtime_error ("property: too big");

		case BaseProperty::Tag::t_unset:	throw std::runtime_error ("property: not set");

		default:				throw std::runtime_error ("property: unknown type");
	}
}

/**
 * uint64_t
 */
BaseProperty::operator uint64_t (void)
{
	switch (type) {
		case BaseProperty::Tag::t_bool:		return operator bool();
		case BaseProperty::Tag::t_u8:		return operator uint8_t();
		case BaseProperty::Tag::t_u16:		return operator uint16_t();
		case BaseProperty::Tag::t_u32:		return operator uint32_t();
		case BaseProperty::Tag::t_u64:		return operator uint64_t();

		case BaseProperty::Tag::t_s8:
		case BaseProperty::Tag::t_s16:
		case BaseProperty::Tag::t_s32:
		case BaseProperty::Tag::t_s64:		throw std::runtime_error ("property: wrong sign");

		case BaseProperty::Tag::t_string:
		case BaseProperty::Tag::t_double:	throw std::runtime_error ("property: wrong type");

		case BaseProperty::Tag::t_unset:	throw std::runtime_error ("property: not set");

		default:				throw std::runtime_error ("property: unknown type");
	}
}

/**
 * int64_t
 */
BaseProperty::operator int64_t (void)
{
	switch (type) {
		case BaseProperty::Tag::t_bool:		return operator bool();
		case BaseProperty::Tag::t_s8:		return operator int8_t();
		case BaseProperty::Tag::t_s16:		return operator int16_t();
		case BaseProperty::Tag::t_s32:		return operator int32_t();
		case BaseProperty::Tag::t_s64:		return operator int64_t();

		case BaseProperty::Tag::t_u8:
		case BaseProperty::Tag::t_u16:
		case BaseProperty::Tag::t_u32:
		case BaseProperty::Tag::t_u64:		throw std::runtime_error ("property: wrong sign");

		case BaseProperty::Tag::t_string:
		case BaseProperty::Tag::t_double:	throw std::runtime_error ("property: wrong type");

		case BaseProperty::Tag::t_unset:	throw std::runtime_error ("property: not set");

		default:				throw std::runtime_error ("property: unknown type");
	}
}


