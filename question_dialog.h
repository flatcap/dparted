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

#ifndef _QUESTION_DIALOG_H_
#define _QUESTION_DIALOG_H_

#include <memory>

#include <gtkmm/messagedialog.h>
#include <gtkmm/dialog.h>
#include <gtkmm/window.h>
#include <gtkmm/entry.h>
#include <gtkmm/image.h>
#include <gtkmm/button.h>

class QuestionDialog;

typedef std::shared_ptr<QuestionDialog> QuestionDialogPtr;

class QuestionDialog : public Gtk::MessageDialog
{
public:
	virtual ~QuestionDialog();

	static QuestionDialogPtr create (void);

	std::string title;
	std::string primary;
	std::string secondary;
	std::string help_url;

	std::vector<std::pair<std::string,int>> buttons;

	int run (void);		// Hide Dialog::run

protected:
	QuestionDialog (void);
	void on_dialog_response (int button_id);
	virtual bool on_key_press_event (GdkEventKey* event);
	virtual bool on_event (GdkEvent * event);

	void on_help (void);
};

#endif // _QUESTION_DIALOG_H_

