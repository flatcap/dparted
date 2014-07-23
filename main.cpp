#include <deque>
#include <thread>
#include <vector>

#include "container.h"

std::mutex thread_mutex;
std::deque<std::thread> thread_queue;

std::vector<std::weak_ptr<Container>> all_children;
std::mutex mutex_children;

void
start_thread (std::function<void(void)> fn)
{
	std::lock_guard<std::mutex> lock (thread_mutex);
	thread_queue.push_back (
		std::thread ([fn]() {
			fn();
		})
	);
}

int
count_containers (const ContainerPtr& c)
{
	if (!c)
		return 0;

	int count = 1;
	for (const auto& i : c->children) {
		count += count_containers(i);
	}

	return count;
}

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
	std::lock_guard<std::mutex> lock (mutex_children);
	all_children.push_back(c);
	}
	std::string s = "name" + std::to_string (i) + " ";
	c->name = s;
	parent->add_child(c);
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
	srandom (time (nullptr));

	ContainerPtr c = Container::create();
	c->name = "top";
	all_children.push_back(c);

	for (int i = 0; i < 99; i++) {
		start_thread (std::bind (add_child, i));
	}

	wait_for_threads();
	printf ("%dC/%ldV children\n", count_containers(c), all_children.size());

	return 0;
}

