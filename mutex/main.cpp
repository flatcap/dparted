#include <deque>
#include <thread>
#include <vector>

#include "container.h"

std::mutex thread_mutex;
std::deque<std::thread> thread_queue;

void
add_child (ContainerPtr parent, int i)
{
	ContainerPtr child = Container::create();
	std::string s = "c" + std::to_string (i) + " ";
	child->set_name(s);
	parent->add_child (child);
}

void
delete_child (ContainerPtr child)
{
	ContainerPtr parent = child->get_parent();
	if (!parent) {
		return;
	}

	parent->delete_child (child);
}

void
alter_child (ContainerPtr child)
{
	child->alter ("M");
}

void
wait_for_threads (void)
{
	while (!thread_queue.empty()) {
		thread_queue.front().join();
		std::lock_guard<std::mutex> lock (thread_mutex);
		thread_queue.pop_front();
	}
}

int
main()
{
	ContainerPtr top = Container::create();
	for (int i = 0; i < 999; i++) {
		ContainerPtr parent = top->pick_container();
		add_child (parent, i);
	}

	int add = 1000;
	int mod = 0;
	int del = 0;

	for (int i = 0; i < 10000; i++) {
		ContainerPtr c = top->pick_container();

		std::lock_guard<std::mutex> lock (thread_mutex);

		int pick = rand() % 100;

		if (pick < 60) {
			thread_queue.push_back (std::thread ([c,i](){ add_child (c, i); }));
			add++;
		} else if (pick < 90) {
			thread_queue.push_back (std::thread ([c](){ alter_child (c); }));
			mod++;
		} else {
			thread_queue.push_back (std::thread ([c](){ delete_child (c); }));
			del++;
		}
	}

	wait_for_threads();

	// top->dump();
	printf ("%d children (A:%d, M:%d, D:%d)\n", top->count_children(), add, mod, del);

	return 0;
}

