#ifndef _APP_H_
#define _APP_H_

#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

class App;

typedef std::shared_ptr<App> AppPtr;

extern AppPtr app;

class App
{
public:
	App (void);
	virtual ~App();

	void test (void);
	void wait_for_threads (void);

protected:
	void start_thread (std::function<void(void)> fn, const char* desc);

	std::mutex thread_mutex;
	std::deque<std::thread> thread_queue;
};


#endif // _APP_H_

