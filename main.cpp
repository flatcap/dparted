#include <memory>
#include <ctime>
#include <functional>
#include <deque>
#include <thread>
#include <algorithm>

#include "container.h"

std::mutex thread_mutex;
std::deque<std::thread> thread_queue;

std::vector<std::weak_ptr<Container>> all_children;
std::mutex mutex_children;

void
start_thread (std::function<void(void)> fn, const char* desc)
{
	std::lock_guard<std::mutex> lock (thread_mutex);
	thread_queue.push_back (
		std::thread ([fn, desc]() {
			// printf ("thread started: %s\n", desc);
			fn();
			// printf ("thread ended:   %s\n", desc);
		})
	);
}

int
count_containers (const ContainerPtr& c)
{
	if (!c)
		return 0;

	const auto& children = c->get_children();

	int count = 1;
	for (const auto& i : children) {
		count += count_containers(i);
	}

	return count;
}

void
dump_children (void)
{
	for (auto c : all_children) {
		if (c.expired())
			printf ("X");
		else
			printf ("O");
	}
}

void
tidy_children (void)
{
	// printf ("TIDY ---------------------------------------------------------------------------------------------------------------------\n");
	// printf ("\t%ld children\n", all_children.size());
	int live = 0;
	int dead = 0;
	for (auto c : all_children) {
		if (c.expired())
			dead++;
		else
			live++;
	}
	// printf ("\t\t%d live\n", live);
	// printf ("\t\t%d dead\n", dead);

	// printf ("REMOVE_IF\n");
	// printf ("\t"); dump_children(); printf ("\n");
	auto new_end = std::remove_if (std::begin(all_children), std::end(all_children), [](std::weak_ptr<Container>& c) { return c.expired(); });
	// printf ("\t"); dump_children(); printf ("\n");

	// printf ("\tcheck start\n");
	for (auto it = begin(all_children); it != new_end; ++it) {
		if ((*it).expired())
			printf ("\tdead link missed\n");
	}

#if 0
	for (auto it = new_end; it != end(all_children); ++it) {
		if (!(*it).expired())
			printf ("\t\tlive link deleted\n");
	}
#endif
	// printf ("\tcheck end\n");

	int before = all_children.size();
	all_children.erase (new_end, std::end(all_children));
	int after  = all_children.size();
	if (before != after) {
		printf ("\ttidied %d children\n", (before-after));
	}

	live = 0;
	dead = 0;
	for (auto c : all_children) {
		if (c.expired())
			dead++;
		else
			live++;
	}
	if (dead != 0) {
		printf ("\t%ld children (after)\n", all_children.size());
		printf ("\t\t%d live\n", live);
		printf ("\t\t%d dead\n", dead);
	}

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
alter_child (void)
{
	int size = all_children.size();
	if (size == 0) {
		printf ("no children left\n");
		return;
	}

	int pick = rand() % size;
	ContainerPtr child = all_children[pick].lock();
	if (!child) {
		printf ("child %d is dead\n", pick);
		return;
	}

	child->name += 'M';
}

void
delete_child (void)
{
	int size = all_children.size();
	if (size == 0) {
		printf ("no children left\n");
		return;
	}

	int pick = (rand() % (size-1)) + 1;	// Skip 0

	ContainerPtr child = all_children[pick].lock();
	if (!child) {
		printf ("child %d is dead\n", pick);
		return;
	}

	ContainerPtr parent = child->get_parent();
	if (!parent) {
		printf ("can't delete top-level\n");
		return;
	}

	{
	std::lock_guard<std::mutex> lock (mutex_children);
	printf ("\tdeleting: %d children\n", count_containers(child));
	printf ("%dC/%ldV children\n", count_containers(all_children[0].lock()), all_children.size());
	parent->delete_child (child);
	all_children.erase (std::begin(all_children)+pick);
	tidy_children();
	printf ("%dC/%ldV children\n", count_containers(all_children[0].lock()), all_children.size());
	}
}

void
wait_for_threads (void)
{
	// printf ("Waiting for threads to finish\n");
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
#if 1
		start_thread (std::bind (add_child, i), "new");
#else
		add_child(i);
#endif
	}

	wait_for_threads();
	// printf ("START: %d\n", count_containers(c));

#if 0
	for (int i = 0; i < 100; i++) {
		start_thread (std::bind (alter_child), "alter");
	}
#endif

#if 0
	for (int i = 0; i < 3; i++) {
		// start_thread (std::bind (delete_child), "delete");
		delete_child();
	}
#endif

	// wait_for_threads();

	// printf ("Threads have finished\n");
	// c->dump();
#if 1
	// tidy_children();
	printf ("%dC/%ldV children\n", count_containers(c), all_children.size());
#endif

	for (auto& i : thread_queue) {
		i.join();	// Wait for things to finish
		// i.detach();	// Don't wait any longer
	}
	std::lock_guard<std::mutex> lock (thread_mutex);
	thread_queue.clear();

	return 0;
}

