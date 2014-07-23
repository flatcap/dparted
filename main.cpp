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
	int pick = rand() % all_children.size();
	ContainerPtr parent = all_children[pick].lock();
	if (!parent) {
		printf ("child is dead\n");
		return;
	}

	ContainerPtr c = Container::create();
	c->name = "name" + std::to_string (i) + " ";
	{
		std::lock_guard<std::mutex> lock (children_mutex);
		all_children.push_back(c);
	}
	parent->add_child(c);
}

int
main()
{
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

