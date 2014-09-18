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

#ifndef _ENDIAN_H_
#define _ENDIAN_H_

#include <cstdint>

#include <endian.h>

// Unsigned: From LE to CPU

#define le16_to_cpu(x)		(std::uint16_t) le16toh ((std::uint16_t)(x))
#define le32_to_cpu(x)		(std::uint32_t) le32toh ((std::uint32_t)(x))
#define le64_to_cpu(x)		(std::uint64_t) le64toh ((std::uint64_t)(x))

#define le16_to_cpup(x)		(std::uint16_t) le16toh (*(const std::uint16_t*)(x))
#define le32_to_cpup(x)		(std::uint32_t) le32toh (*(const std::uint32_t*)(x))
#define le64_to_cpup(x)		(std::uint64_t) le64toh (*(const std::uint64_t*)(x))

// Signed: From LE to CPU

#define sle16_to_cpu(x)		(std::int16_t) le16toh ((std::int16_t)(x))
#define sle32_to_cpu(x)		(std::int32_t) le32toh ((std::int32_t)(x))
#define sle64_to_cpu(x)		(std::int64_t) le64toh ((std::int64_t)(x))

#define sle16_to_cpup(x)	(std::int16_t) le16toh (*(const std::int16_t*)(x))
#define sle32_to_cpup(x)	(std::int32_t) le32toh (*(const std::int32_t*)(x))
#define sle64_to_cpup(x)	(std::int64_t) le64toh (*(const std::int64_t*)(x))

// Unsigned: From CPU to LE

#define cpu_to_le16(x)		(std::uint16_t) htole16 ((std::uint16_t)(x))
#define cpu_to_le32(x)		(std::uint32_t) htole32 ((std::uint32_t)(x))
#define cpu_to_le64(x)		(std::uint64_t) htole64 ((std::uint64_t)(x))

#define cpu_to_le16p(x)		(std::uint16_t) htole16 (*(const std::uint16_t*)(x))
#define cpu_to_le32p(x)		(std::uint32_t) htole32 (*(const std::uint32_t*)(x))
#define cpu_to_le64p(x)		(std::uint64_t) htole64 (*(const std::uint64_t*)(x))

// Signed: From CPU to LE

#define cpu_to_sle16(x)		(std::int16_t) htole16 ((std::int16_t)(x))
#define cpu_to_sle32(x)		(std::int32_t) htole32 ((std::int32_t)(x))
#define cpu_to_sle64(x)		(std::int64_t) htole64 ((std::int64_t)(x))

#define cpu_to_sle16p(x)	(std::int16_t) htole16 (*(const std::int16_t*)(x))
#define cpu_to_sle32p(x)	(std::int32_t) htole32 (*(const std::int32_t*)(x))
#define cpu_to_sle64p(x)	(std::int64_t) htole64 (*(const std::int64_t*)(x))

// Unsigned: From BE to CPU

#define be16_to_cpu(x)		(std::uint16_t) be16toh ((std::uint16_t)(x))
#define be32_to_cpu(x)		(std::uint32_t) be32toh ((std::uint32_t)(x))
#define be64_to_cpu(x)		(std::uint64_t) be64toh ((std::uint64_t)(x))

#define be16_to_cpup(x)		(std::uint16_t) be16toh (*(const std::uint16_t*)(x))
#define be32_to_cpup(x)		(std::uint32_t) be32toh (*(const std::uint32_t*)(x))
#define be64_to_cpup(x)		(std::uint64_t) be64toh (*(const std::uint64_t*)(x))

// Signed: From BE to CPU

#define sbe16_to_cpu(x)		(std::int16_t) be16toh ((std::int16_t)(x))
#define sbe32_to_cpu(x)		(std::int32_t) be32toh ((std::int32_t)(x))
#define sbe64_to_cpu(x)		(std::int64_t) be64toh ((std::int64_t)(x))

#define sbe16_to_cpup(x)	(std::int16_t) be16toh (*(const std::int16_t*)(x))
#define sbe32_to_cpup(x)	(std::int32_t) be32toh (*(const std::int32_t*)(x))
#define sbe64_to_cpup(x)	(std::int64_t) be64toh (*(const std::int64_t*)(x))

// Unsigned: From CPU to BE

#define cpu_to_be16(x)		(std::uint16_t) htobe16 ((std::uint16_t)(x))
#define cpu_to_be32(x)		(std::uint32_t) htobe32 ((std::uint32_t)(x))
#define cpu_to_be64(x)		(std::uint64_t) htobe64 ((std::uint64_t)(x))

#define cpu_to_be16p(x)		(std::uint16_t) htobe16 (*(const std::uint16_t*)(x))
#define cpu_to_be32p(x)		(std::uint32_t) htobe32 (*(const std::uint32_t*)(x))
#define cpu_to_be64p(x)		(std::uint64_t) htobe64 (*(const std::uint64_t*)(x))

// Signed: From CPU to BE

#define cpu_to_sbe16(x)		(std::int16_t) htobe16 ((std::int16_t)(x))
#define cpu_to_sbe32(x)		(std::int32_t) htobe32 ((std::int32_t)(x))
#define cpu_to_sbe64(x)		(std::int64_t) htobe64 ((std::int64_t)(x))

#define cpu_to_sbe16p(x)	(std::int16_t) htobe16 (*(const std::int16_t*)(x))
#define cpu_to_sbe32p(x)	(std::int32_t) htobe32 (*(const std::int32_t*)(x))
#define cpu_to_sbe64p(x)	(std::int64_t) htobe64 (*(const std::int64_t*)(x))

#endif // _ENDIAN_H_

