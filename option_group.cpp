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
	Glib::OptionEntry entry01; entry01.set_short_name ('a'); entry01.set_long_name ("app");        entry01.set_description ("Run the graphical application (default)");          add_entry          (entry01, app);
	Glib::OptionEntry entry02; entry02.set_short_name ('l'); entry02.set_long_name ("list");       entry02.set_description ("List all the objects on the disks");                add_entry          (entry02, list);
	Glib::OptionEntry entry04; entry04.set_short_name ('p'); entry04.set_long_name ("properties"); entry04.set_description ("Show all the disks' metadata");                     add_entry          (entry04, properties);
	Glib::OptionEntry entry06; entry06.set_short_name ('q'); entry06.set_long_name ("quit");       entry06.set_description ("Kill the running instance of DParted");             add_entry          (entry06, quit);
	Glib::OptionEntry entry07; entry07.set_short_name ('c'); entry07.set_long_name ("config");     entry07.set_description ("Use this file to configure DParted");               add_entry_filename (entry07, config);
	Glib::OptionEntry entry08; entry08.set_short_name ('t'); entry08.set_long_name ("theme");      entry08.set_description ("Use this file to style the graphical application"); add_entry_filename (entry08, theme);
	Glib::OptionEntry entry09; entry09.set_short_name ('x'); entry09.set_long_name ("xcoord");     entry09.set_description ("X coordinate of the main window (app only)");       add_entry          (entry09, x);
	Glib::OptionEntry entry10; entry10.set_short_name ('y'); entry10.set_long_name ("ycoord");     entry10.set_description ("Y coordinate of the main window (app only)");       add_entry          (entry10, y);
	Glib::OptionEntry entry11; entry11.set_short_name ('w'); entry11.set_long_name ("width");      entry11.set_description ("Width of the main window (app only)");              add_entry          (entry11, w);
	Glib::OptionEntry entry12; entry12.set_short_name ('h'); entry12.set_long_name ("height");     entry12.set_description ("Height of the main window (app only)");             add_entry          (entry12, h);
	Glib::OptionEntry entry13; entry13.set_short_name ('H'); entry13.set_long_name ("hexdump");    entry13.set_description ("Hexdump of all the objects on the disks");          add_entry          (entry13, hex);
#ifdef DP_DOT
	Glib::OptionEntry entry90; entry90.set_short_name ('d'); entry90.set_long_name ("dot-display");  entry90.set_description ("Display disk hierarchy");      add_entry (entry90, dot_display);
	Glib::OptionEntry entry91;                               entry91.set_long_name ("dot-resize");   entry91.set_description ("Percentage scale of display"); add_entry (entry91, dot_resize);
	Glib::OptionEntry entry92;                               entry92.set_long_name ("dot-separate"); entry92.set_description ("Separate graphviz diagrams");  add_entry (entry92, dot_separate);
	Glib::OptionEntry entry93;                               entry93.set_long_name ("dot-save-gv");  entry93.set_description ("Save graphviz diagrams");      add_entry (entry93, dot_save_gv);
	Glib::OptionEntry entry94;                               entry94.set_long_name ("dot-save-png"); entry94.set_description ("Save png images");             add_entry (entry94, dot_save_png);
#endif
}


