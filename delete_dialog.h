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

#ifndef _DELETE_DIALOG_H_
#define _DELETE_DIALOG_H_

#include <gtkmm/checkbutton.h>
#include <gtkmm/entry.h>
#include <gtkmm/grid.h>
#include <gtkmm/image.h>

#include "dialog.h"

typedef std::shared_ptr<class DeleteDialog> DeleteDialogPtr;

class DeleteDialog : public Dialog
{
public:
	virtual ~DeleteDialog();

	static DeleteDialogPtr create (QuestionPtr q);

	virtual int run (void);		// Hide Dialog::run

protected:
	DeleteDialog (QuestionPtr q);
	virtual void response (int button_id);

	Gtk::Grid grid;

	void control_toggled (Gtk::CheckButton* check, Option& o);
};

#endif // _DELETE_DIALOG_H_

