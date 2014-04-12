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

#include <cstring>

#include "text_app.h"
#ifdef DP_LIST
#include "list_visitor.h"
#endif
#ifdef DP_DOT
#include "dot_visitor.h"
#endif
#ifdef DP_PROP
#include "prop_visitor.h"
#endif
#ifdef DP_HEX
#include "hex_visitor.h"
#endif
#include "log_trace.h"
#include "stringnum.h"

TextAppPtr text_app;

TextApp::TextApp (void)
{
}

TextApp::~TextApp()
{
}


void usage (void)
{
	log_info ("Usage: dparted [options] [device]\n");
	log_info ("\t-?  help\n");
#ifdef DP_DOT
	log_info ("\t-d  dotty\n");
	log_info ("\t\t--dot-resize NUM      Percentage scale of display\n");
	log_info ("\t\t--dot-separate        Separate graphviz diagrams\n");
	log_info ("\t\t--dot-save-gv         Save graphviz diagrams\n");
	log_info ("\t\t--dot-save-png        Save png images\n");
#endif
#ifdef DP_HEX
	log_info ("\t-h  hex dump\n");
	log_info ("\t\t--hex-abbreviate NUM  Show leading/trailing bytes\n");
#endif
#ifdef DP_LIST
	log_info ("\t-l  list\n");
#endif
#ifdef DP_PROP
	log_info ("\t-p  properties\n");
#endif
}

int
TextApp::run (int argc, char **argv)
{
#ifdef DP_DOT
	bool dot          = false;
	int  dot_resize   = 100;
	bool dot_separate = false;
	bool dot_save_gv  = false;
	bool dot_save_png = false;
#endif
#ifdef DP_HEX
	bool hex            = false;
	int  hex_abbreviate = 256;
#endif
#ifdef DP_LIST
	bool list = false;
#endif
#ifdef DP_PROP
	bool prop = false;
#endif
	bool error = false;

	std::vector<std::string> disks;			// Mop up any remaining args
	for (; argc > 1; --argc, ++argv) {
		if (argv[1][0] == '-') {
			int len = strlen (argv[1]);
			if (len == 1) {
				log_error ("no option given\n");
				error = true;
				continue;
			}
			for (int i = 1; i < len; ++i) {
				if (strncmp (argv[1], "--", 2) == 0) {
					if (false) {
#ifdef DP_HEX
					} else if (strcmp (argv[1], "--hex-abbreviate") == 0) {
						if (argc < 3) {
							log_error ("No argument for --hex-abbreviate\n");
							error = true;
							break;
						}
						StringNum s (argv[2]);
						hex_abbreviate = (int) s;
						--argc;
						++argv;
#endif
#ifdef DP_DOT
					} else if (strcmp (argv[1], "--dot-resize") == 0) {
						if (argc < 3) {
							log_error ("No argument for --dot-resize\n");
							error = true;
							break;
						}
						StringNum s (argv[2]);
						dot_resize = (int) s;
						--argc;
						++argv;
					} else if (strcmp (argv[1], "--dot-separate") == 0) {
						dot_separate = true;
					} else if (strcmp (argv[1], "--dot-save-gv") == 0) {
						dot_save_gv = true;
					} else if (strcmp (argv[1], "--dot-save-png") == 0) {
						dot_save_png = true;
#endif
					} else {
						log_error ("Unknown option %s\n", argv[1]);
						error = true;
					}
					break;
				}

				switch (argv[1][i]) {
					case '?': error = true; break;
#ifdef DP_DOT
					case 'd': dot   = true; break;
#endif
#ifdef DP_HEX
					case 'h': hex   = true; break;
#endif
#ifdef DP_LIST
					case 'l': list  = true; break;
#endif
#ifdef DP_PROP
					case 'p': prop  = true; break;
#endif
					default:
						log_error ("Unknown option: -%c\n", argv[1][i]);
						error = true;
				}
			}
			continue;
		}
		disks.push_back (argv[1]);
	}

	if (error) {
		usage();
		exit(1);
	}

	top_level = main_app->scan (disks);

#ifdef DP_DOT
	if (dot) {
		if (dot_separate) {
			for (auto c : top_level->get_children()) {
				DotVisitor dv;
				dv.display  = !(dot_save_gv || dot_save_png);
				dv.resize   = dot_resize;
				dv.save_gv  = dot_save_gv;
				dv.save_png = dot_save_png;
				c->accept (dv);
				dv.run_dotty();
			}
		} else {
			DotVisitor dv;
			dv.display  = !(dot_save_gv || dot_save_png);
			dv.resize   = dot_resize;
			dv.save_gv  = dot_save_gv;
			dv.save_png = dot_save_png;
			top_level->accept (dv);
			dv.run_dotty();
		}
	}
#endif
#ifdef DP_HEX
	if (hex) {
		HexVisitor hv;
		hv.abbreviate = hex_abbreviate;
		top_level->accept (hv);
	}
#endif
#ifdef DP_LIST
	if (list) run_list (top_level);
#endif
#ifdef DP_PROP
	if (prop) run_prop (top_level);
#endif

#if 0
	ContainerPtr c1 = *top_level->get_children().begin();		// loop
	log_debug ("%s\n", c1->dump());
	ContainerPtr c2 = *c1->get_children().begin();			// gpt
	log_debug ("%s %ld\n", c2->dump(), c2->get_children().size());
#if 0
	for (auto i : c2->get_children()) {
		log_debug ("\t%s\n", i->dump());
	}
#endif
	auto it = c2->get_children().begin();
	std::advance (it, 15);
	ContainerPtr c3 = *it;
	ContainerPtr c4 = *c3->get_children().begin();

	log_debug ("%s\n", c4->dump());
	for (auto p : c4->get_all_props (true)) {
		std::string s { *p };
		log_debug ("%s\t%s\n", p->name.c_str(), s.c_str());
	}
#endif

	return 0;
}

