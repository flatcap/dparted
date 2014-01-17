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

#include <iostream>
#include <string>
#include <vector>

#include "app.h"
#include "gui_app.h"

#include "dot_visitor.h"
#include "dump_visitor.h"
#include "prop_visitor.h"
#include "log.h"

/**
 * main
 */
int
main (int argc, char* argv[])
{
	log_init ("/dev/stdout");

	//command line
	// -a	app
	// -l	list
	// -d	dot
	// -s	separate dot diagrams
	//
	//NOTIMPL
	// -c config_file
	//	[multiple = override]
	// -t theme_file
	//	[multiple = choice]

	bool app      = false;
	bool dot      = false;
	bool list     = false;
	bool separate = false;
	bool props    = false;

	if (argc > 1) {
		std::string arg = argv[1];
		if (arg[0] == '-') {
			for (auto c : arg) {
				switch (c) {
					case '-':                  break;
					case 'a': app      = true; break;
					case 'd': dot      = true; break;
					case 'l': list     = true; break;
					case 'p': props    = true; break;
					case 's': separate = true; break;
					default:
						  printf ("unknown option '%c'\n", c);
						  break;
				}
			}
			argc--;
			argv++;
		}
	}

	std::vector<std::string> files (argv + 1, argv + argc);

	ContainerPtr top_level;

	if (app) {
		gui_app = std::make_shared<GuiApp>();
		main_app = gui_app;

		gui_app->set_config ("config/dparted.conf");
		gui_app->set_theme  ("config/theme.conf");
	} else {
		main_app = std::make_shared<App>();
	}

	if (list || props || dot) {
		top_level = main_app->scan (files);
	}

	if (list && top_level) {
		log_info ("------------------------------------------------------------\n");
		DumpVisitor dv;
		top_level->accept (dv);
		dv.dump();
		log_info ("------------------------------------------------------------\n");
	}

	if (props && top_level) {
		log_info ("------------------------------------------------------------\n");
		PropVisitor pv;
		top_level->accept (pv);
		pv.dump();
		log_info ("------------------------------------------------------------\n");
	}

	if (dot && top_level) {
		if (separate) {
			for (auto c : top_level->get_children()) {
				DotVisitor dv;
				c->accept (dv);
				dv.run_dotty();
			}
		} else {
			DotVisitor dv;
			for (auto c : top_level->get_children()) {
				c->accept (dv);
			}
			dv.run_dotty();
		}
	}

	if (gui_app) {
#if 0
		if (!top_level)
			top_level = main_app->scan (files);
#endif
                gui_app->run (0, nullptr);
	}

	log_close();
	return 0;
}


