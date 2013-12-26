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

#include "vw.h"

/**
 * string
 */
BaseVariant::operator std::string (void)
{
	switch (type) {
		case BaseVariant::Tag::t_string:	return operator std::string();

		case BaseVariant::Tag::t_double:	return std::to_string (operator double());
		case BaseVariant::Tag::t_bool:		return std::to_string (operator bool());
		case BaseVariant::Tag::t_u8:		return std::to_string (operator uint8_t());
		case BaseVariant::Tag::t_s8:		return std::to_string (operator int8_t());
		case BaseVariant::Tag::t_u16:		return std::to_string (operator uint16_t());
		case BaseVariant::Tag::t_s16:		return std::to_string (operator int16_t());
		case BaseVariant::Tag::t_u32:		return std::to_string (operator uint32_t());
		case BaseVariant::Tag::t_s32:		return std::to_string (operator int32_t());
		case BaseVariant::Tag::t_u64:		return std::to_string (operator uint64_t());
		case BaseVariant::Tag::t_s64:		return std::to_string (operator int64_t());

		case BaseVariant::Tag::t_unset:		throw std::runtime_error ("variant: not set");

		default:				throw std::runtime_error ("variant: unknown type");
	}
}

/**
 * double
 */
BaseVariant::operator double (void)
{
	switch (type) {
		case BaseVariant::Tag::t_double:	return operator double();

		case BaseVariant::Tag::t_bool:		return operator bool();

		case BaseVariant::Tag::t_string:
		case BaseVariant::Tag::t_u8:
		case BaseVariant::Tag::t_s8:
		case BaseVariant::Tag::t_u16:
		case BaseVariant::Tag::t_s16:
		case BaseVariant::Tag::t_u32:
		case BaseVariant::Tag::t_s32:
		case BaseVariant::Tag::t_u64:
		case BaseVariant::Tag::t_s64:		throw std::runtime_error ("variant: wrong type");

		case BaseVariant::Tag::t_unset:		throw std::runtime_error ("variant: not set");

		default:				throw std::runtime_error ("variant: unknown type");
	}
}

/**
 * bool
 */
BaseVariant::operator bool (void)
{
	switch (type) {
		case BaseVariant::Tag::t_string:	return (!operator std::string().empty());

		case BaseVariant::Tag::t_double:	return (operator double()   != 0);
		case BaseVariant::Tag::t_bool:		return (operator bool()     != 0);
		case BaseVariant::Tag::t_u8:		return (operator uint8_t()  != 0);
		case BaseVariant::Tag::t_s8:		return (operator int8_t()   != 0);
		case BaseVariant::Tag::t_u16:		return (operator uint16_t() != 0);
		case BaseVariant::Tag::t_s16:		return (operator int16_t()  != 0);
		case BaseVariant::Tag::t_u32:		return (operator uint32_t() != 0);
		case BaseVariant::Tag::t_s32:		return (operator int32_t()  != 0);
		case BaseVariant::Tag::t_u64:		return (operator uint64_t() != 0);
		case BaseVariant::Tag::t_s64:		return (operator int64_t()  != 0);

		case BaseVariant::Tag::t_unset:		throw std::runtime_error ("variant: not set");

		default:				throw std::runtime_error ("variant: unknown type");
	}
}

/**
 * uint8_t
 */
BaseVariant::operator uint8_t (void)
{
	switch (type) {
		case BaseVariant::Tag::t_bool:		return operator bool();
		case BaseVariant::Tag::t_u8:		return operator uint8_t();

		case BaseVariant::Tag::t_s8:		throw std::runtime_error ("variant: wrong sign");

		case BaseVariant::Tag::t_string:
		case BaseVariant::Tag::t_double:	throw std::runtime_error ("variant: wrong type");

		case BaseVariant::Tag::t_u16:
		case BaseVariant::Tag::t_s16:
		case BaseVariant::Tag::t_u32:
		case BaseVariant::Tag::t_s32:
		case BaseVariant::Tag::t_u64:
		case BaseVariant::Tag::t_s64:		throw std::runtime_error ("variant: too big");

		case BaseVariant::Tag::t_unset:		throw std::runtime_error ("variant: not set");

		default:				throw std::runtime_error ("variant: unknown type");
	}
}

/**
 * int8_t
 */
BaseVariant::operator int8_t (void)
{
	switch (type) {
		case BaseVariant::Tag::t_bool:		return operator bool();
		case BaseVariant::Tag::t_s8:		return operator int8_t();

		case BaseVariant::Tag::t_u8:		throw std::runtime_error ("variant: wrong sign");

		case BaseVariant::Tag::t_string:
		case BaseVariant::Tag::t_double:	throw std::runtime_error ("variant: wrong type");

		case BaseVariant::Tag::t_u16:
		case BaseVariant::Tag::t_s16:
		case BaseVariant::Tag::t_u32:
		case BaseVariant::Tag::t_s32:
		case BaseVariant::Tag::t_u64:
		case BaseVariant::Tag::t_s64:		throw std::runtime_error ("variant: too big");

		case BaseVariant::Tag::t_unset:		throw std::runtime_error ("variant: not set");

		default:				throw std::runtime_error ("variant: unknown type");
	}
}

/**
 * uint16_t
 */
BaseVariant::operator uint16_t (void)
{
	switch (type) {
		case BaseVariant::Tag::t_bool:		return operator bool();
		case BaseVariant::Tag::t_u8:		return operator uint8_t();
		case BaseVariant::Tag::t_u16:		return operator uint16_t();

		case BaseVariant::Tag::t_s8:
		case BaseVariant::Tag::t_s16:		throw std::runtime_error ("variant: wrong sign");

		case BaseVariant::Tag::t_string:
		case BaseVariant::Tag::t_double:	throw std::runtime_error ("variant: wrong type");

		case BaseVariant::Tag::t_u32:
		case BaseVariant::Tag::t_s32:
		case BaseVariant::Tag::t_u64:
		case BaseVariant::Tag::t_s64:		throw std::runtime_error ("variant: too big");

		case BaseVariant::Tag::t_unset:		throw std::runtime_error ("variant: not set");

		default:				throw std::runtime_error ("variant: unknown type");
	}
}

/**
 * int16_t
 */
BaseVariant::operator int16_t (void)
{
	switch (type) {
		case BaseVariant::Tag::t_bool:		return operator bool();
		case BaseVariant::Tag::t_s8:		return operator int8_t();
		case BaseVariant::Tag::t_s16:		return operator int16_t();

		case BaseVariant::Tag::t_u8:
		case BaseVariant::Tag::t_u16:		throw std::runtime_error ("variant: wrong sign");

		case BaseVariant::Tag::t_string:
		case BaseVariant::Tag::t_double:	throw std::runtime_error ("variant: wrong type");

		case BaseVariant::Tag::t_u32:
		case BaseVariant::Tag::t_s32:
		case BaseVariant::Tag::t_u64:
		case BaseVariant::Tag::t_s64:		throw std::runtime_error ("variant: too big");

		case BaseVariant::Tag::t_unset:		throw std::runtime_error ("variant: not set");

		default:				throw std::runtime_error ("variant: unknown type");
	}
}

/**
 * uint32_t
 */
BaseVariant::operator uint32_t (void)
{
	switch (type) {
		case BaseVariant::Tag::t_bool:		return operator bool();
		case BaseVariant::Tag::t_u8:		return operator uint8_t();
		case BaseVariant::Tag::t_u16:		return operator uint16_t();
		case BaseVariant::Tag::t_u32:		return operator uint32_t();

		case BaseVariant::Tag::t_s8:
		case BaseVariant::Tag::t_s16:
		case BaseVariant::Tag::t_s32:		throw std::runtime_error ("variant: wrong sign");

		case BaseVariant::Tag::t_string:
		case BaseVariant::Tag::t_double:	throw std::runtime_error ("variant: wrong type");

		case BaseVariant::Tag::t_u64:
		case BaseVariant::Tag::t_s64:		throw std::runtime_error ("variant: too big");

		case BaseVariant::Tag::t_unset:		throw std::runtime_error ("variant: not set");

		default:				throw std::runtime_error ("variant: unknown type");
	}
}

/**
 * int32_t
 */
BaseVariant::operator int32_t (void)
{
	switch (type) {
		case BaseVariant::Tag::t_bool:		return operator bool();
		case BaseVariant::Tag::t_s8:		return operator int8_t();
		case BaseVariant::Tag::t_s16:		return operator int16_t();
		case BaseVariant::Tag::t_s32:		return operator int32_t();

		case BaseVariant::Tag::t_u8:
		case BaseVariant::Tag::t_u16:
		case BaseVariant::Tag::t_u32:		throw std::runtime_error ("variant: wrong sign");

		case BaseVariant::Tag::t_string:
		case BaseVariant::Tag::t_double:	throw std::runtime_error ("variant: wrong type");

		case BaseVariant::Tag::t_u64:
		case BaseVariant::Tag::t_s64:		throw std::runtime_error ("variant: too big");

		case BaseVariant::Tag::t_unset:		throw std::runtime_error ("variant: not set");

		default:				throw std::runtime_error ("variant: unknown type");
	}
}

/**
 * uint64_t
 */
BaseVariant::operator uint64_t (void)
{
	switch (type) {
		case BaseVariant::Tag::t_bool:		return operator bool();
		case BaseVariant::Tag::t_u8:		return operator uint8_t();
		case BaseVariant::Tag::t_u16:		return operator uint16_t();
		case BaseVariant::Tag::t_u32:		return operator uint32_t();
		case BaseVariant::Tag::t_u64:		return operator uint64_t();

		case BaseVariant::Tag::t_s8:
		case BaseVariant::Tag::t_s16:
		case BaseVariant::Tag::t_s32:
		case BaseVariant::Tag::t_s64:		throw std::runtime_error ("variant: wrong sign");

		case BaseVariant::Tag::t_string:
		case BaseVariant::Tag::t_double:	throw std::runtime_error ("variant: wrong type");

		case BaseVariant::Tag::t_unset:		throw std::runtime_error ("variant: not set");

		default:				throw std::runtime_error ("variant: unknown type");
	}
}

/**
 * int64_t
 */
BaseVariant::operator int64_t (void)
{
	switch (type) {
		case BaseVariant::Tag::t_bool:		return operator bool();
		case BaseVariant::Tag::t_s8:		return operator int8_t();
		case BaseVariant::Tag::t_s16:		return operator int16_t();
		case BaseVariant::Tag::t_s32:		return operator int32_t();
		case BaseVariant::Tag::t_s64:		return operator int64_t();

		case BaseVariant::Tag::t_u8:
		case BaseVariant::Tag::t_u16:
		case BaseVariant::Tag::t_u32:
		case BaseVariant::Tag::t_u64:		throw std::runtime_error ("variant: wrong sign");

		case BaseVariant::Tag::t_string:
		case BaseVariant::Tag::t_double:	throw std::runtime_error ("variant: wrong type");

		case BaseVariant::Tag::t_unset:		throw std::runtime_error ("variant: not set");

		default:				throw std::runtime_error ("variant: unknown type");
	}
}


