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

#ifndef _QUESTION_H_
#define _QUESTION_H_

#include <string>
#include <vector>
#include <memory>
#include <functional>

#include "container.h"

class Question;

typedef std::shared_ptr<Question> QuestionPtr;
typedef std::function<void(QuestionPtr)> question_cb_t;

class Question
{
public:
	virtual ~Question();
	static QuestionPtr create (question_cb_t fn = nullptr);

	std::map<std::string, std::string> input;
	std::map<std::string, std::string> output;
	std::vector<std::pair<std::string, int>> buttons;

	int result = -1;

	virtual void done (void);
	std::string get_input (const std::string& name);

	enum class Type {
		ChangePassword,
		Error,
		Information,
		Password,
		Question,
		Warning
	} type = Type::Information;

protected:
	Question (void);

	question_cb_t done_fn = nullptr;
	std::weak_ptr<Question> self;
};

#endif // _QUESTION_H_

