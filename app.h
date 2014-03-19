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

class App;

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

	void queue_add_probe (ContainerPtr& item);
	ContainerPtr scan (const std::vector<std::string>& files);
	ContainerPtr probe (ContainerPtr& parent, std::uint8_t* buffer, std::uint64_t bufsize);

	ContainerPtr get_top_level (void) { return top_level; } //XXX tmp

protected:
	ContainerPtr top_level;

	ConfigFilePtr config_file;

	std::queue<ContainerPtr> probe_queue;
};


#endif // _APP_H_

