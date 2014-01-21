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

#ifndef _QUESTION_H_
#define _QUESTION_H_

#include <string>
#include <vector>
#include <memory>
#include <functional>

#include "container.h"

class Question;

typedef std::shared_ptr<Question> QuestionPtr;

class Question
{
public:
	virtual ~Question() = default;
	static QuestionPtr create (ContainerPtr c);

	std::string title;
	std::string question;

	std::vector<std::string> answers;

	virtual void asked (void);

#if 0
	void reply (QuestionReply qr)
	{
		r = qr;
	}
#endif

protected:
	Question (void);

	ContainerPtr object;

#if 0
	QuestionReply r = nullptr;
#endif
};

#endif // _QUESTION_H_

