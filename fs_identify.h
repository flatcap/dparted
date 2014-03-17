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

#ifndef _IDENTIFY_H_
#define _IDENTIFY_H_

bool identify_gpt      (std::uint8_t* buffer, int bufsize);
bool identify_lvm      (std::uint8_t* buffer, int bufsize);
bool identify_lvm_mlog (std::uint8_t* buffer, int bufsize);
bool identify_msdos    (std::uint8_t* buffer, int bufsize);
bool identify_reiserfs (std::uint8_t* buffer, int bufsize);
bool identify_swap     (std::uint8_t* buffer, int bufsize);
bool identify_vfat     (std::uint8_t* buffer, int bufsize);
bool identify_xfs      (std::uint8_t* buffer, int bufsize);

#endif // _IDENTIFY_H_

