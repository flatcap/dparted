#include <iostream>
#include <memory>
#include <set>
#include <tuple>

typedef std::tuple<long,long,void*> Mmap;	// offset, size, ptr
typedef std::shared_ptr<Mmap> MmapPtr;

/**
 * lambda deleter
 */
auto
deleter = [] (Mmap *mm)
{
	//std::cout << "deleter called on: " << mm. << std::endl;
	delete mm;
};

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


typedef std::set<MmapPtr,compare> MmapSet;

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

