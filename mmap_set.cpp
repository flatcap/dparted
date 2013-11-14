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

#include <iostream>
#include <memory>
#include <set>
#include <tuple>

struct compare;

typedef std::tuple<long,long,void*> Mmap;	// offset, size, ptr
typedef std::shared_ptr<Mmap>       MmapPtr;	// Mmap smart pointer
typedef std::set<MmapPtr,compare>   MmapSet;	// sorted set of Mmaps

/**
 * deleter
 */
void deleter (Mmap *m)
{
	//std::cout << "deleter called on: " << m << std::endl;
	delete m;
}

/**
 * functor compare
 */
struct compare
{
	bool operator() (const MmapPtr &a, const MmapPtr &b)
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

/**
 * insert
 */
void
insert (MmapSet &s, long offset, long size, void *ptr)
{
	s.insert (MmapPtr(new Mmap (offset, size, ptr), deleter));
}


/**
 * main
 */
int
main (int, char *[])
{
	void *v = (void*) 0xdeadbeef;

	MmapSet s;

	insert (s, 1460, 500, v);
	insert (s, 2017, 500, v);
	insert (s, 5587, 500, v);
	insert (s, 6538, 500, v);
	insert (s, 2174, 200, v);
	insert (s, 3412, 200, v);
	insert (s, 3412, 300, v);
	insert (s, 3412, 100, v);

	auto it = s.begin();
	MmapPtr sp1 (*it++);
	MmapPtr sp2 (*it++);

	for (auto pm : s) {
		long a;
		long b;
		void *c;

		std::tie (a, b, c) = *pm;
		std::cout << a << " " << b << " " << c << " : RefCount = " << pm.use_count() << std::endl;
	}

	return 0;
}

