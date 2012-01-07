/* Copyright (c) 2011 Richard Russon (FlatCap)
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


#include <string.h>

/**
 * identify_btrfs
 */
int identify_btrfs (char *buffer, int bufsize)
{
	return (strncmp (buffer+65600, "_BHRfS_M", 8) == 0);
}

/**
 * identify_ext2
 */
int identify_ext2 (char *buffer, int bufsize)
{
	return (*(unsigned short int *) (buffer+1080) == 0xEF53);
}

/**
 * identify_ntfs
 */
int identify_ntfs (char *buffer, int bufsize)
{
	return (strncmp (buffer+3, "NTFS    ", 8) == 0);
}

/**
 * identify_reiserfs
 */
int identify_reiserfs (char *buffer, int bufsize)
{
	return (!strncmp (buffer+65588, "ReIsErFs",  8) ||
		!strncmp (buffer+65588, "ReIsEr2Fs", 8) ||
		!strncmp (buffer+65588, "ReIsEr3Fs", 9));
}

/**
 * identify_swap
 */
int identify_swap (char *buffer, int bufsize)
{
	return (!strncmp (buffer+4086, "SWAPSPACE2",           10) ||
		!strncmp (buffer+4086, "SWAP-SPACE",           10) ||
		!strncmp (buffer+4076, "SWAPSPACE2S1SUSPEND",  19) ||
		!strncmp (buffer+4076, "SWAPSPACE2LINHIB0001", 20));
}

/**
 * identify_vfat
 */
int identify_vfat (char *buffer, int bufsize)
{
	return ((*(unsigned short int *) (buffer+510) == 0xAA55) &&
		(buffer[3] != 0) &&
		(*(unsigned short int *) (buffer+11) != 0) &&
		(buffer[13] != 0) &&
		(*(unsigned short int *) (buffer+14) != 0) &&
		((buffer[16] > 0) || (buffer[16] < 5)));
}

/**
 * identify_xfs
 */
int identify_xfs (char *buffer, int bufsize)
{
	return (strncmp (buffer, "XFSB", 4) == 0);
}

/**
 * identify_lvm
 */
int identify_lvm (char *buffer, int bufsize)
{
	return (strncmp (buffer+536, "LVM2 001", 8) == 0);
}


