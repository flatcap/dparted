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

#include "password_dialog.h"

PasswordDialog::PasswordDialog (const Glib::ustring& message) :
	Gtk::MessageDialog (message)
{
}

PasswordDialog::~PasswordDialog()
{
}


PasswordDialogPtr
PasswordDialog::create (void)
{
	PasswordDialogPtr p (new PasswordDialog("hello"));
	p->weak = p;

	return p;
}

