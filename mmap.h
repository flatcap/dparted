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

#ifndef _MMAP_H_
#define _MMAP_H_

struct compare;
typedef std::tuple<long,long,void*> Mmap;	// offset, size, ptr
typedef std::shared_ptr<Mmap>       MmapPtr;	// Mmap smart pointer
typedef std::set<MmapPtr,compare>   MmapSet;	// sorted set of Mmaps

/**
 * functor compare
 */
struct compare
{
	bool operator() (const MmapPtr& a, const MmapPtr& b)
	{
		long ao, as, bo, bs;
		std::tie (ao, as, std::ignore) = *a;
		std::tie (bo, bs, std::ignore) = *b;

		if (ao == bo)
			return (as < bs);
		else
			return (ao < bo);
	}
};


#endif // _MMAP_H_

