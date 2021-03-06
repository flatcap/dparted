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

#ifndef _FS_GET_H_
#define _FS_GET_H_

#include "filesystem.h"

FilesystemPtr get_reiserfs (ContainerPtr& parent, std::uint8_t* buffer, std::uint64_t bufsize);
FilesystemPtr get_swap     (ContainerPtr& parent, std::uint8_t* buffer, std::uint64_t bufsize);
FilesystemPtr get_vfat     (ContainerPtr& parent, std::uint8_t* buffer, std::uint64_t bufsize);
FilesystemPtr get_xfs      (ContainerPtr& parent, std::uint8_t* buffer, std::uint64_t bufsize);

#endif // _FS_GET_H_

