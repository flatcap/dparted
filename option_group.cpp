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

#include <glibmm/optionentry.h>

#include "option_group.h"

OptionGroup::OptionGroup (void) :
	Glib::OptionGroup ("dparted", "Options for DParted", "Options to configure DParted")
{
	Glib::OptionEntry entry01; entry01.set_short_name ('q'); entry01.set_long_name ("quit");       entry01.set_description ("Kill the running instance of DParted");             add_entry          (entry01, quit);
	Glib::OptionEntry entry02; entry02.set_short_name ('c'); entry02.set_long_name ("config");     entry02.set_description ("Use this file to configure DParted");               add_entry_filename (entry02, config);
	Glib::OptionEntry entry03; entry03.set_short_name ('t'); entry03.set_long_name ("theme");      entry03.set_description ("Use this file to style the graphical application"); add_entry_filename (entry03, theme);
	Glib::OptionEntry entry04; entry04.set_short_name ('x'); entry04.set_long_name ("xcoord");     entry04.set_description ("X coordinate of the main window");                  add_entry          (entry04, x);
	Glib::OptionEntry entry05; entry05.set_short_name ('y'); entry05.set_long_name ("ycoord");     entry05.set_description ("Y coordinate of the main window");                  add_entry          (entry05, y);
	Glib::OptionEntry entry06; entry06.set_short_name ('w'); entry06.set_long_name ("width");      entry06.set_description ("Width of the main window");                         add_entry          (entry06, w);
	Glib::OptionEntry entry07; entry07.set_short_name ('h'); entry07.set_long_name ("height");     entry07.set_description ("Height of the main window");                        add_entry          (entry07, h);
#ifdef DP_TEST
	Glib::OptionEntry entry08; entry08.set_short_name ('T'); entry08.set_long_name ("test");       entry08.set_description ("Test number");                                      add_entry          (entry08, test);
#endif
}


