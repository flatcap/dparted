/* Copyright (c) 2011-2012 Richard Russon (FlatCap)
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


#ifndef _IDENTIFY_H_
#define _IDENTIFY_H_

int identify_btrfs    (char *buffer, int bufsize);
int identify_ext2     (char *buffer, int bufsize);
int identify_gpt      (char *buffer, int bufsize);
int identify_lvm      (char *buffer, int bufsize);
int identify_ntfs     (char *buffer, int bufsize);
int identify_reiserfs (char *buffer, int bufsize);
int identify_swap     (char *buffer, int bufsize);
int identify_vfat     (char *buffer, int bufsize);
int identify_xfs      (char *buffer, int bufsize);

#endif // _IDENTIFY_H_

