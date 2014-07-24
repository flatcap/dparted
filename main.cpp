#include <deque>
#include <thread>
#include <vector>

#include "container.h"

std::mutex thread_mutex;
std::deque<std::thread> thread_queue;

void
add_child (ContainerPtr parent)
{
	int ch = parent->children.size();

	int pick = rand() % (ch+1);
	if (pick == ch) {
		parent->add_child(Container::create());
		return;
	}

	auto it = std::begin (parent->children);
	std::advance (it, pick);
	add_child (*it);
}

int
main()
{
	ContainerPtr c = Container::create();

	for (int i = 0; i < 3999; i++) {
		std::lock_guard<std::mutex> lock (thread_mutex);
		thread_queue.push_back (std::thread ([&c](){ add_child(c); }));
	}

	while (!thread_queue.empty()) {
		thread_queue.front().join();
		std::lock_guard<std::mutex> lock (thread_mutex);
		thread_queue.pop_front();
	}

	// c->dump();

	return 0;
}

