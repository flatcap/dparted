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

#ifndef _GPT_H
#define _GPT_H

#include <memory>
#include <string>
#include <vector>

#include "table.h"

class Gpt;

typedef std::shared_ptr<Gpt> GptPtr;

class Gpt : public Table
{
public:
	static GptPtr create (void);
	virtual ~Gpt();
	virtual bool accept (Visitor& v);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

	static ContainerPtr probe (ContainerPtr& top_level, ContainerPtr& parent, unsigned char* buffer, int bufsize);

public:
	//properties

protected:
	Gpt (void);

private:

};

#endif // _GPT_H

