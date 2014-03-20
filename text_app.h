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

#ifndef _TEXT_APP_H_
#define _TEXT_APP_H_

#include <memory>

#include "app.h"

class TextApp;

typedef std::shared_ptr<TextApp> TextAppPtr;

extern TextAppPtr text_app;

class TextApp : public App
{
public:
	TextApp (void);
	virtual ~TextApp();

	int run (int argc, char **argv);

protected:

};

#endif // _TEXT_APP_H_

