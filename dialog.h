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

#ifndef _DIALOG_H_
#define _DIALOG_H_

#include <memory>

#include <gtkmm/messagedialog.h>
#include <gtkmm/image.h>

#include "log.h"
#include "question.h"

typedef std::shared_ptr<class Dialog> DialogPtr;

class Dialog : public Gtk::MessageDialog
{
public:
	Dialog (QuestionPtr q);
	virtual ~Dialog();

	bool ignore_escape = false;

	virtual int run (void);

protected:
	Gtk::Image image;
	Gtk::Button help;

	QuestionPtr question;

	void on_help (void);
	bool add_buttons (void);

	virtual void response (int button) = 0;
	virtual bool on_event (GdkEvent* event);
};

#endif // _DIALOG_H_

