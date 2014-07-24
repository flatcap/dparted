#include <deque>
#include <thread>
#include <vector>

#include "container.h"

std::mutex thread_mutex;
std::deque<std::thread> thread_queue;

void
add_child (ContainerPtr parent)
{
	parent->add_child (Container::create());
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

	for (int i = 0; i < 3999; i++) {
		ContainerPtr parent = top->pick_container();

		std::lock_guard<std::mutex> lock (thread_mutex);
		thread_queue.push_back (std::thread ([parent](){ add_child (parent); }));
	}

#if 1
	wait_for_threads();

	for (int i = 0; i < 999; i++) {
		ContainerPtr child = top->pick_container();

		std::lock_guard<std::mutex> lock (thread_mutex);
		thread_queue.push_back (std::thread ([child](){ delete_child (child); }));
	}
#endif

	wait_for_threads();

	// top->dump();

	return 0;
}

