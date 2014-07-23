#include <string>
#include <sstream>
#include <ratio>
#include <chrono>
#include <thread>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/major.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>

#include "app.h"

AppPtr app;

App::App (void)
{
}

App::~App()
{
	for (auto& i : thread_queue) {
		i.join();	// Wait for things to finish
		// i.detach();	// Don't wait any longer
	}
	std::lock_guard<std::mutex> lock (thread_mutex);
	thread_queue.clear();
}


void
App::start_thread (std::function<void(void)> fn, const char* desc)
{
	std::lock_guard<std::mutex> lock (thread_mutex);
	thread_queue.push_back (
		std::thread ([fn, desc]() {
			printf ("thread started: %s\n", desc);
			fn();
			printf ("thread ended:   %s\n", desc);
		})
	);
}


int
App::run (void)
{
	return 0;
}

