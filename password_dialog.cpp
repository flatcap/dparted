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

#include <glibmm.h>

#include "password_dialog.h"
#include "log.h"
#include "log_macro.h"

PasswordDialog::PasswordDialog (const Glib::ustring& message) :
	Gtk::MessageDialog (message)
{
	log_ctor ("ctor PasswordDialog\n");
}

PasswordDialog::~PasswordDialog()
{
	log_dtor ("dtor PasswordDialog\n");
}


PasswordDialogPtr
PasswordDialog::create (void)
{
	PasswordDialogPtr p (new PasswordDialog("hello"));
	p->self = p;

	return p;
}

