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

#include <memory>
#include <queue>

#include "question.h"
#include "message.h"
#include "config_file.h"
#include "container.h"
#include "thread.h"

class App;

typedef std::function<void(ContainerPtr)> scan_async_cb_t;

typedef std::shared_ptr<App> AppPtr;

extern AppPtr main_app;

class App
{
public:
	App (void);
	virtual ~App();

	virtual bool ask    (QuestionPtr q);
	virtual bool notify (Message& m);

	ConfigFilePtr get_config (void);
	bool set_config (const std::string& filename);

	ContainerPtr scan (std::vector<std::string>& devices);
	bool identify_device (ContainerPtr parent, std::string& device);
	bool process_queue_item (ContainerPtr item);

	template<class T>
	void queue_add_probe (std::shared_ptr<T>& item)
	{
		return_if_fail (item);
		ContainerPtr c (item);
		THREAD (std::bind (&App::process_queue_item, this, c)).detach();
	}

protected:
	ConfigFilePtr config_file;
};


#endif // _APP_H_

