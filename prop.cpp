#include <string>
#include <stdexcept>

#include "prop.h"

BaseProp::operator std::string (void)
{
	switch (type) {
		case BaseProp::Tag::t_string:	return operator std::string();

		case BaseProp::Tag::t_double:	return std::to_string (operator double());
		case BaseProp::Tag::t_bool:		return std::to_string (operator bool());
		case BaseProp::Tag::t_u8:		return std::to_string (operator uint8_t());
		case BaseProp::Tag::t_s8:		return std::to_string (operator int8_t());
		case BaseProp::Tag::t_u16:		return std::to_string (operator uint16_t());
		case BaseProp::Tag::t_s16:		return std::to_string (operator int16_t());
		case BaseProp::Tag::t_u32:		return std::to_string (operator uint32_t());
		case BaseProp::Tag::t_s32:		return std::to_string (operator int32_t());
		case BaseProp::Tag::t_u64:		return std::to_string (operator uint64_t());
		case BaseProp::Tag::t_s64:		return std::to_string (operator int64_t());

		case BaseProp::Tag::t_unset:	throw std::runtime_error ("prop: not set");

		default:				throw std::runtime_error ("prop: unknown type");
	}
}

BaseProp::operator double (void)
{
	switch (type) {
		case BaseProp::Tag::t_double:	return operator double();

		case BaseProp::Tag::t_bool:		return operator bool();

		case BaseProp::Tag::t_string:
		case BaseProp::Tag::t_u8:
		case BaseProp::Tag::t_s8:
		case BaseProp::Tag::t_u16:
		case BaseProp::Tag::t_s16:
		case BaseProp::Tag::t_u32:
		case BaseProp::Tag::t_s32:
		case BaseProp::Tag::t_u64:
		case BaseProp::Tag::t_s64:		throw std::runtime_error ("prop: wrong type");

		case BaseProp::Tag::t_unset:	throw std::runtime_error ("prop: not set");

		default:				throw std::runtime_error ("prop: unknown type");
	}
}

BaseProp::operator bool (void)
{
	switch (type) {
		case BaseProp::Tag::t_string:	return (!operator std::string().empty());

		case BaseProp::Tag::t_double:	return (operator double()   != 0);
		case BaseProp::Tag::t_bool:		return (operator bool()     != 0);
		case BaseProp::Tag::t_u8:		return (operator uint8_t()  != 0);
		case BaseProp::Tag::t_s8:		return (operator int8_t()   != 0);
		case BaseProp::Tag::t_u16:		return (operator uint16_t() != 0);
		case BaseProp::Tag::t_s16:		return (operator int16_t()  != 0);
		case BaseProp::Tag::t_u32:		return (operator uint32_t() != 0);
		case BaseProp::Tag::t_s32:		return (operator int32_t()  != 0);
		case BaseProp::Tag::t_u64:		return (operator uint64_t() != 0);
		case BaseProp::Tag::t_s64:		return (operator int64_t()  != 0);

		case BaseProp::Tag::t_unset:	throw std::runtime_error ("prop: not set");

		default:				throw std::runtime_error ("prop: unknown type");
	}
}

BaseProp::operator uint8_t (void)
{
	switch (type) {
		case BaseProp::Tag::t_bool:		return operator bool();
		case BaseProp::Tag::t_u8:		return operator uint8_t();

		case BaseProp::Tag::t_s8:		throw std::runtime_error ("prop: wrong sign");

		case BaseProp::Tag::t_string:
		case BaseProp::Tag::t_double:	throw std::runtime_error ("prop: wrong type");

		case BaseProp::Tag::t_u16:
		case BaseProp::Tag::t_s16:
		case BaseProp::Tag::t_u32:
		case BaseProp::Tag::t_s32:
		case BaseProp::Tag::t_u64:
		case BaseProp::Tag::t_s64:		throw std::runtime_error ("prop: too big");

		case BaseProp::Tag::t_unset:	throw std::runtime_error ("prop: not set");

		default:				throw std::runtime_error ("prop: unknown type");
	}
}

BaseProp::operator int8_t (void)
{
	switch (type) {
		case BaseProp::Tag::t_bool:		return operator bool();
		case BaseProp::Tag::t_s8:		return operator int8_t();

		case BaseProp::Tag::t_u8:		throw std::runtime_error ("prop: wrong sign");

		case BaseProp::Tag::t_string:
		case BaseProp::Tag::t_double:	throw std::runtime_error ("prop: wrong type");

		case BaseProp::Tag::t_u16:
		case BaseProp::Tag::t_s16:
		case BaseProp::Tag::t_u32:
		case BaseProp::Tag::t_s32:
		case BaseProp::Tag::t_u64:
		case BaseProp::Tag::t_s64:		throw std::runtime_error ("prop: too big");

		case BaseProp::Tag::t_unset:	throw std::runtime_error ("prop: not set");

		default:				throw std::runtime_error ("prop: unknown type");
	}
}

BaseProp::operator uint16_t (void)
{
	switch (type) {
		case BaseProp::Tag::t_bool:		return operator bool();
		case BaseProp::Tag::t_u8:		return operator uint8_t();
		case BaseProp::Tag::t_u16:		return operator uint16_t();

		case BaseProp::Tag::t_s8:
		case BaseProp::Tag::t_s16:		throw std::runtime_error ("prop: wrong sign");

		case BaseProp::Tag::t_string:
		case BaseProp::Tag::t_double:	throw std::runtime_error ("prop: wrong type");

		case BaseProp::Tag::t_u32:
		case BaseProp::Tag::t_s32:
		case BaseProp::Tag::t_u64:
		case BaseProp::Tag::t_s64:		throw std::runtime_error ("prop: too big");

		case BaseProp::Tag::t_unset:	throw std::runtime_error ("prop: not set");

		default:				throw std::runtime_error ("prop: unknown type");
	}
}

BaseProp::operator int16_t (void)
{
	switch (type) {
		case BaseProp::Tag::t_bool:		return operator bool();
		case BaseProp::Tag::t_s8:		return operator int8_t();
		case BaseProp::Tag::t_s16:		return operator int16_t();

		case BaseProp::Tag::t_u8:
		case BaseProp::Tag::t_u16:		throw std::runtime_error ("prop: wrong sign");

		case BaseProp::Tag::t_string:
		case BaseProp::Tag::t_double:	throw std::runtime_error ("prop: wrong type");

		case BaseProp::Tag::t_u32:
		case BaseProp::Tag::t_s32:
		case BaseProp::Tag::t_u64:
		case BaseProp::Tag::t_s64:		throw std::runtime_error ("prop: too big");

		case BaseProp::Tag::t_unset:	throw std::runtime_error ("prop: not set");

		default:				throw std::runtime_error ("prop: unknown type");
	}
}

BaseProp::operator uint32_t (void)
{
	switch (type) {
		case BaseProp::Tag::t_bool:		return operator bool();
		case BaseProp::Tag::t_u8:		return operator uint8_t();
		case BaseProp::Tag::t_u16:		return operator uint16_t();
		case BaseProp::Tag::t_u32:		return operator uint32_t();

		case BaseProp::Tag::t_s8:
		case BaseProp::Tag::t_s16:
		case BaseProp::Tag::t_s32:		throw std::runtime_error ("prop: wrong sign");

		case BaseProp::Tag::t_string:
		case BaseProp::Tag::t_double:	throw std::runtime_error ("prop: wrong type");

		case BaseProp::Tag::t_u64:
		case BaseProp::Tag::t_s64:		throw std::runtime_error ("prop: too big");

		case BaseProp::Tag::t_unset:	throw std::runtime_error ("prop: not set");

		default:				throw std::runtime_error ("prop: unknown type");
	}
}

BaseProp::operator int32_t (void)
{
	switch (type) {
		case BaseProp::Tag::t_bool:		return operator bool();
		case BaseProp::Tag::t_s8:		return operator int8_t();
		case BaseProp::Tag::t_s16:		return operator int16_t();
		case BaseProp::Tag::t_s32:		return operator int32_t();

		case BaseProp::Tag::t_u8:
		case BaseProp::Tag::t_u16:
		case BaseProp::Tag::t_u32:		throw std::runtime_error ("prop: wrong sign");

		case BaseProp::Tag::t_string:
		case BaseProp::Tag::t_double:	throw std::runtime_error ("prop: wrong type");

		case BaseProp::Tag::t_u64:
		case BaseProp::Tag::t_s64:		throw std::runtime_error ("prop: too big");

		case BaseProp::Tag::t_unset:	throw std::runtime_error ("prop: not set");

		default:				throw std::runtime_error ("prop: unknown type");
	}
}

BaseProp::operator uint64_t (void)
{
	switch (type) {
		case BaseProp::Tag::t_bool:		return operator bool();
		case BaseProp::Tag::t_u8:		return operator uint8_t();
		case BaseProp::Tag::t_u16:		return operator uint16_t();
		case BaseProp::Tag::t_u32:		return operator uint32_t();
		case BaseProp::Tag::t_u64:		return operator uint64_t();

		case BaseProp::Tag::t_s8:
		case BaseProp::Tag::t_s16:
		case BaseProp::Tag::t_s32:
		case BaseProp::Tag::t_s64:		throw std::runtime_error ("prop: wrong sign");

		case BaseProp::Tag::t_string:
		case BaseProp::Tag::t_double:	throw std::runtime_error ("prop: wrong type");

		case BaseProp::Tag::t_unset:	throw std::runtime_error ("prop: not set");

		default:				throw std::runtime_error ("prop: unknown type");
	}
}

BaseProp::operator int64_t (void)
{
	switch (type) {
		case BaseProp::Tag::t_bool:		return operator bool();
		case BaseProp::Tag::t_s8:		return operator int8_t();
		case BaseProp::Tag::t_s16:		return operator int16_t();
		case BaseProp::Tag::t_s32:		return operator int32_t();
		case BaseProp::Tag::t_s64:		return operator int64_t();

		case BaseProp::Tag::t_u8:
		case BaseProp::Tag::t_u16:
		case BaseProp::Tag::t_u32:
		case BaseProp::Tag::t_u64:		throw std::runtime_error ("prop: wrong sign");

		case BaseProp::Tag::t_string:
		case BaseProp::Tag::t_double:	throw std::runtime_error ("prop: wrong type");

		case BaseProp::Tag::t_unset:	throw std::runtime_error ("prop: not set");

		default:				throw std::runtime_error ("prop: unknown type");
	}
}


