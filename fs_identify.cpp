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

#include <cstring>

#include "endian.h"
#include "log.h"
#include "log_trace.h"

bool
identify_reiserfs (std::uint8_t* buffer, std::uint64_t UNUSED(bufsize))
{
	//XXX check bufsize.  all other functions too
	return (!strncmp ((char*) buffer+65588, "ReIsErFs",  8) ||
		!strncmp ((char*) buffer+65588, "ReIsEr2Fs", 8) ||
		!strncmp ((char*) buffer+65588, "ReIsEr3Fs", 9));
}

bool
identify_swap (std::uint8_t* buffer, std::uint64_t UNUSED(bufsize))
{
	return (!strncmp ((char*) buffer+4086, "SWAPSPACE2",           10) ||
		!strncmp ((char*) buffer+4086, "SWAP-SPACE",           10) ||
		!strncmp ((char*) buffer+4076, "SWAPSPACE2S1SUSPEND",  19) ||
		!strncmp ((char*) buffer+4076, "SWAPSPACE2LINHIB0001", 20));

	//XXX 65526	string	SWAPSPACE2	Linux/ppc swap file
	//XXX 16374	string	SWAPSPACE2	Linux/ia64 swap file
}

bool
identify_vfat (std::uint8_t* buffer, std::uint64_t UNUSED(bufsize))
{
	for (int i = 3; i < 11; ++i) {
		if ((buffer[i] > 0) && (buffer[i] < ' '))
			return 0;
		if (buffer[i] > 126)
			return 0;
	}

	return ((le16_to_cpup (buffer+510) == 0xAA55) &&
		(buffer[3] != 0) &&
		(le16_to_cpup (buffer+11) != 0) &&
		(buffer[13] != 0) &&
		(le16_to_cpup (buffer+14) != 0) &&
		((buffer[16] > 0) || (buffer[16] < 5)));
}

bool
identify_xfs (std::uint8_t* buffer, std::uint64_t UNUSED(bufsize))
{
	return (strncmp ((char*) buffer, "XFSB", 4) == 0);
}

bool
identify_lvm (std::uint8_t* buffer, std::uint64_t UNUSED(bufsize))
{
	return (strncmp ((char*) buffer+536, "LVM2 001", 8) == 0);
}

bool
identify_lvm_mlog (std::uint8_t* buffer, std::uint64_t UNUSED(bufsize))
{
	return (strncmp ((char*) buffer+0, "rRiM", 4) == 0);
}

