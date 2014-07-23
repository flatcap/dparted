#include <deque>
#include <thread>
#include <vector>

#include "container.h"

std::mutex thread_mutex;
std::deque<std::thread> thread_queue;

std::vector<std::weak_ptr<Container>> all_children;
std::mutex children_mutex;

void
add_child (int i)
{
	int size = all_children.size();
	if (size == 0) {
		printf ("no children left\n");
		return;
	}

	int pick = rand() % size;
	ContainerPtr parent = all_children[pick].lock();
	if (!parent) {
		printf ("child %d is dead\n", pick);
		return;
	}

	ContainerPtr c = Container::create();
	{
	std::lock_guard<std::mutex> lock (children_mutex);
	all_children.push_back(c);
	}
	std::string s = "name" + std::to_string (i) + " ";
	c->name = s;
	parent->add_child(c);
}

int
main()
{
	srandom (time (nullptr));

	ContainerPtr c = Container::create();
	c->name = "top";
	all_children.push_back(c);

	for (int i = 0; i < 99; i++) {
		std::lock_guard<std::mutex> lock (thread_mutex);

		thread_queue.push_back (std::thread ([i](){ add_child(i); }));
	}

	while (!thread_queue.empty()) {
		thread_queue.front().join();
		std::lock_guard<std::mutex> lock (thread_mutex);
		thread_queue.pop_front();
	}

	printf ("%dC/%ldV children\n", c->count_children(), all_children.size());

	return 0;
}

