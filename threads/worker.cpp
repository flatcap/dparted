#include <chrono>
#include <functional>
#include <mutex>
#include <string>
#include <thread>

#include "worker.h"

void Worker::get_data (double& fraction_done, std::string& message)
{
	std::lock_guard<std::mutex> lock (m_Mutex);
	fraction_done = m_fraction_done;
	message = m_message;
}

void Worker::stop_work (void)
{
	std::lock_guard<std::mutex> lock (m_Mutex);
	m_shall_stop = true;
}

bool Worker::has_stopped (void)
{
	std::lock_guard<std::mutex> lock (m_Mutex);
	return m_has_stopped;
}

void Worker::do_work (notify_t callback)
{
	m_Mutex.lock();
	m_has_stopped = false;
	m_fraction_done = 0.0;
	m_message.clear();
	m_Mutex.unlock();

	for (int i = 0; ; ++i) { // do until break
		std::this_thread::sleep_for (std::chrono::milliseconds (100)); // Simulate a long calculation.

		{// Scope lock
			std::lock_guard<std::mutex> lock (m_Mutex);

			m_fraction_done += 0.01;

			if ((i % 4) == 3) {
				m_message = std::to_string ((int) (m_fraction_done * 100.0)) + "% done";
			}

			if (m_fraction_done >= 1.0) {
				m_message = "Finished";
				break;
			}

			if (m_shall_stop) {
				m_message = "Stopped";
				break;
			}
		} // End of scope
		callback();
	}

	m_Mutex.lock();
	m_shall_stop = false;
	m_has_stopped = true;
	m_Mutex.unlock();
	callback();
}

