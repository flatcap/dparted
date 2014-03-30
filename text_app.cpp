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



#include "text_app.h"
#include "log_trace.h"
#ifdef DP_LIST
#include "list_visitor.h"
#endif
#ifdef DP_DOT
#include "dot_visitor.h"
#endif

TextAppPtr text_app;

TextApp::TextApp (void)
{
}

TextApp::~TextApp()
{
}


int
TextApp::run (int argc, char **argv)
{
	std::vector<std::string> disks;			// Mop up any remaining args
	for (; argc > 1; argc--, ++argv) {
		//disks.push_back (argv[1]);
	}

	top_level = main_app->scan (disks);

#ifdef DP_LIST
	ListVisitor lv;
	top_level->accept (lv);
	lv.list();
#endif

#ifdef DP_DOT
	DotVisitor dv;
	dv.resize = 60;
	top_level->accept (dv);
	dv.run_dotty();
#endif

#if 0
	ContainerPtr c1 = *top_level->get_children().begin();		// loop
	 //log_debug ("%s\n", c1->dump());
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

