#ifndef _WORKER_H_
#define _WORKER_H_

#include <functional>
#include <mutex>
#include <string>
#include <thread>

typedef std::function<void(void)> notify_t;

class Worker
{
public:
	Worker (void)     = default;
	virtual ~Worker() = default;

	void do_work (notify_t callback);

	void get_data (double& fraction_done, std::string& message);
	void stop_work (void);
	bool has_stopped (void);

private:
	std::mutex m_Mutex;

	bool m_shall_stop  = false;
	bool m_has_stopped = false;

	// Shared data
	double m_fraction_done = 0.0;
	std::string m_message;
};

#endif // _WORKER_H_

