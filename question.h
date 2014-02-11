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
	static QuestionPtr create (ContainerPtr c, question_cb_t fn);

	std::string title;
	std::string question;
	std::string help_url;

	int result = -1;

	std::vector<std::string> answers;

	virtual void done (void);

	enum class Type {
		Question,
		Information,
		Error,
		Password
	} type = Type::Information;

protected:
	Question (void);

	ContainerPtr object;

	question_cb_t done_fn = nullptr;

	std::weak_ptr<Question> weak;
};

#endif // _QUESTION_H_

