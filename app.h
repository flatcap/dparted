/* Copyright (c) 2014 Richard Russon (FlatCap)
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Library General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _APP_H_
#define _APP_H_

#include <memory>

#include "question.h"
#include "message.h"
#include "config_manager.h"

class App;

typedef std::shared_ptr<App> AppPtr;

/**
 * class App
 */
class App
{
public:
	App (void);
	virtual ~App();

	virtual bool ask (Question& q);
	virtual bool notify (Message& m);

	ConfigManagerPtr get_config_manager (void);

	bool add_config (const std::string& filename);

protected:
	ConfigManagerPtr config_manager;
};


#endif // _APP_H_

