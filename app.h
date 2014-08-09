/* Copyright (c) 2014 Richard Russon (FlatCap)
 *
 * This file is part of DParted.
 *
 * DParted is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DParted is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DParted.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _APP_H_
#define _APP_H_

#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "question.h"
#include "config_file.h"
#include "container.h"
#include "timeline.h"

typedef std::function<void(ContainerPtr)> scan_async_cb_t;

typedef std::shared_ptr<class App> AppPtr;

extern AppPtr main_app;

class App
{
public:
	App (void);
	virtual ~App();

	virtual bool ask (QuestionPtr q);

	ConfigFilePtr get_config (void);
	bool set_config (const std::string& filename);

	ContainerPtr scan (std::vector<std::string>& devices, scan_async_cb_t fn);
	bool identify_device (ContainerPtr parent, std::string& device);
	bool process_queue_item (ContainerPtr item);
	void queue_add_probe (ContainerPtr& item);

	virtual bool open_uri (const std::string& uri);

	void test (void);
	void wait_for_threads (void);

	bool adjust_timeline (int amount);
	TimelinePtr get_timeline (void);

protected:
	ConfigFilePtr config_file;
	TimelinePtr timeline;

	void start_thread (std::function<void(void)> fn, const char* desc);

private:
#ifdef DP_THREADED
	std::mutex thread_mutex;
	std::deque<std::thread> thread_queue;
#endif
};


#endif // _APP_H_

