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

#include <cstring>

#include "log.h"
#include "log_trace.h"

/**
 * identify_btrfs
 */
bool
identify_btrfs (unsigned char* buffer, int bufsize)
{
	return (strncmp ((char*) buffer+65600, "_BHRfS_M", 8) == 0);
}

/**
 * identify_ext2
 */
bool
identify_ext2 (unsigned char* buffer, int bufsize)
{
	bool b1 = (*(unsigned short int*) (buffer+0x438) == 0xEF53);	// Magic
	bool b2 = !(*(unsigned int*) (buffer + 0x45C) & 0x0000004);	// Journal

	return (b1 && b2);
}

/**
 * identify_ext3
 */
bool
identify_ext3 (unsigned char* buffer, int bufsize)
{
	bool b1 = (*(unsigned short int*) (buffer+0x438) == 0xEF53);	// Magic
	bool b2 = (*(unsigned int*) (buffer + 0x45C) & 0x0000004);	// Journal
	bool b3 = (*(unsigned int*) (buffer + 0x460) < 0x0000040);
	bool b4 = (*(unsigned int*) (buffer + 0x464) < 0x0000008);

	return (b1 && b2 && b3 && b4);
}

/**
 * identify_ext4
 */
bool
identify_ext4 (unsigned char* buffer, int bufsize)
{
	bool b1 = (*(unsigned short int*) (buffer+0x438) == 0xEF53);	// Magic
	bool b2 = (*(unsigned int*) (buffer + 0x45C) & 0x0000004);	// Journal
	bool b3 = (*(unsigned int*) (buffer + 0x460) < 0x0000040);
	bool b4 = (*(unsigned int*) (buffer + 0x460) > 0x000003f);
	bool b5 = (*(unsigned int*) (buffer + 0x464) > 0x0000007);

	return (b1 && b2 && ((b3 && b5) || b4));
}

/**
 * identify_gpt
 */
bool
identify_gpt (unsigned char* buffer, int bufsize)
{
	return (strncmp ((char*) buffer+512, "EFI PART", 8) == 0);
}

/**
 * identify_msdos
 */
bool
identify_msdos (unsigned char* buffer, int bufsize)
{
	return (*(unsigned short int*) (buffer+510) == 0xAA55);
}

/**
 * identify_ntfs
 */
bool
identify_ntfs (unsigned char* buffer, int bufsize)
{
	return (strncmp ((char*) buffer+3, "NTFS    ", 8) == 0);
}

/**
 * identify_reiserfs
 */
bool
identify_reiserfs (unsigned char* buffer, int bufsize)
{
	return (!strncmp ((char*) buffer+65588, "ReIsErFs",  8) ||
		!strncmp ((char*) buffer+65588, "ReIsEr2Fs", 8) ||
		!strncmp ((char*) buffer+65588, "ReIsEr3Fs", 9));
}

/**
 * identify_swap
 */
bool
identify_swap (unsigned char* buffer, int bufsize)
{
	return (!strncmp ((char*) buffer+4086, "SWAPSPACE2",           10) ||
		!strncmp ((char*) buffer+4086, "SWAP-SPACE",           10) ||
		!strncmp ((char*) buffer+4076, "SWAPSPACE2S1SUSPEND",  19) ||
		!strncmp ((char*) buffer+4076, "SWAPSPACE2LINHIB0001", 20));
}

/**
 * identify_vfat
 */
bool
identify_vfat (unsigned char* buffer, int bufsize)
{
	for (int i = 3; i < 11; i++) {
		if ((buffer[i] > 0) && (buffer[i] < ' '))
			return 0;
		if (buffer[i] > 126)
			return 0;
	}

	return ((*(unsigned short int*) (buffer+510) == 0xAA55) &&
		(buffer[3] != 0) &&
		(*(unsigned short int*) (buffer+11) != 0) &&
		(buffer[13] != 0) &&
		(*(unsigned short int*) (buffer+14) != 0) &&
		((buffer[16] > 0) || (buffer[16] < 5)));
}

/**
 * identify_xfs
 */
bool
identify_xfs (unsigned char* buffer, int bufsize)
{
	return (strncmp ((char*) buffer, "XFSB", 4) == 0);
}

/**
 * identify_lvm
 */
bool
identify_lvm (unsigned char* buffer, int bufsize)
{
	return (strncmp ((char*) buffer+536, "LVM2 001", 8) == 0);
}

/**
 * identify_lvm_mlog
 */
bool
identify_lvm_mlog (unsigned char* buffer, int bufsize)
{
	return (strncmp ((char*) buffer+0, "rRiM", 4) == 0);
}

