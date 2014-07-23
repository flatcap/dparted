#include <deque>
#include <thread>
#include <vector>

#include "container.h"

std::mutex thread_mutex;
std::deque<std::thread> thread_queue;

void
add_child (ContainerPtr& parent)
{
	ContainerPtr child = Container::create();
	parent->add_child(child);
}

int
main()
{
	ContainerPtr c = Container::create();

	for (int i = 0; i < 99; i++) {
		std::lock_guard<std::mutex> lock (thread_mutex);
		thread_queue.push_back (std::thread ([&c](){ add_child(c); }));
	}

	while (!thread_queue.empty()) {
		thread_queue.front().join();
		std::lock_guard<std::mutex> lock (thread_mutex);
		thread_queue.pop_front();
	}

	return 0;
}

