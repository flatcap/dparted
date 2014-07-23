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

class App;

typedef std::shared_ptr<App> AppPtr;

extern AppPtr app;

class App
{
public:
	App (void);
	virtual ~App();

	int run (void);

protected:
	void start_thread (std::function<void(void)> fn, const char* desc);

	std::mutex thread_mutex;
	std::deque<std::thread> thread_queue;
};


#endif // _APP_H_

