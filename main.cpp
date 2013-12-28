/* Copyright (c) 2013 Richard Russon (FlatCap)
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

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <gtkmm.h>
#include <gtkmm/main.h>

#include <iostream>
#include <cstdlib>
#include <queue>
#include <string>
#include <vector>

#include <linux/major.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>

#include "app.h"
#include "gui_app.h"
#include "container.h"
#include "file.h"
#include "filesystem.h"
#include "loop.h"
#include "misc.h"
#include "table.h"

#include "dot_visitor.h"
#include "dump_visitor.h"
#include "log.h"
#include "log_trace.h"
#include "pointers.h"
#include "utils.h"
#include "volume.h"
#include "invariant.h"

std::queue<ContainerPtr> probe_queue;

AppPtr main_app;

/**
 * queue_add_probe
 */
void
queue_add_probe (ContainerPtr& item)
{
	if (!item)
		return;

	probe_queue.push (item);
	std::string s = get_size (item->parent_offset);
	//log_info ("QUEUE: %s %s : %lld (%s)\n", item->name.c_str(), item->device.c_str(), item->parent_offset, s.c_str());
	//log_info ("QUEUE has %lu items\n", probe_queue.size());
}

#if 0
/**
 * mounts_get_list
 */
unsigned int
mounts_get_list (ContainerPtr& mounts)
{
	std::string command;
	std::vector<std::string> output;
	std::string error;

	command = "grep '^/dev' /proc/mounts";
	execute_command (command, output);

	for (unsigned int i = 0; i < output.size(); i++) {
		std::string line = output[i];
		log_info ("line%d:\n%s\n\n", i, line.c_str());
	}

	return mounts.children.size();
}

#endif

/**
 * probe
 */
ContainerPtr
probe (ContainerPtr& top_level, ContainerPtr& parent)
{
	//LOG_TRACE;

	if (!top_level || !parent)
		return nullptr;

	ContainerPtr item;

	if ((item = Filesystem::probe (top_level, parent))) {
		return item;
	}

	if ((item = Table::probe (top_level, parent))) {
		return item;
	}

	if ((item = Misc::probe (top_level, parent))) {
		return item;
	}

	return nullptr;
}

#if 0
/**
 * main
 */
int
main (int argc, char* argv[])
{
	//command line
	// -a	app
	// -l	list
	// -d	dot
	// -s	separate dot diagrams

	bool app      = true;
	bool dot      = false;
	bool list     = false;
	bool separate = false;

	if (argc > 1) {
		std::string arg = argv[1];
		if (arg[0] == '-') {
			for (auto c : arg) {
				switch (c) {
					case '-':                  break;
					case 'a': app      = true; break;
					case 'd': dot      = true; break;
					case 'l': list     = true; break;
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

	log_init ("/dev/stdout");

	ContainerPtr top_level = Container::create();
	top_level->name = "dummy";

	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			struct stat st;
			int res = -1;
			int fd = -1;

			fd = open (argv[i], O_RDONLY | O_CLOEXEC);
			if (fd < 0) {
				log_debug ("can't open file %s\n", argv[i]);
				continue;
			}

			res = fstat (fd, &st);
			if (res < 0) {
				log_debug ("stat on %s failed\n", argv[i]);
				close (fd);
				continue;
			}

			if (S_ISREG (st.st_mode) || S_ISDIR (st.st_mode)) {
				File::identify (top_level, argv[i], fd, st);
			} else if (S_ISBLK (st.st_mode)) {
				if (MAJOR (st.st_rdev) == LOOP_MAJOR) {
					Loop::identify (top_level, argv[i], fd, st);
				} else {
					//Gpt::identify (top_level, argv[i], fd, st);
				}
			} else {
				log_error ("can't probe something else\n");
			}
			close (fd);
		}
	} else {
		Loop::discover (top_level, probe_queue);
		//Gpt::discover (top_level, probe_queue);
	}

	// Process the probe_queue
	ContainerPtr item;
	//XXX deque?
	while (!probe_queue.empty()) {
		item = probe_queue.front();
		probe_queue.pop();

		//std::cout << "Item: " << item << "\n";

		ContainerPtr found = probe (top_level, item);
		if (found) {
			//item->add_child (found);
			//std::cout << "\tFound: " << found << "\n";
			//probe_queue.push (found);
		} else {
			//XXX LOG
			break;
		}
	}

	//LvmGroup::discover (top_level);
	//MdGroup::discover (top_level);

#if 1
	// Process the probe_queue
	//XXX deque?
	while (!probe_queue.empty()) {
		item = probe_queue.front();
		probe_queue.pop();

		//std::cout << "Item: " << item << "\n";

		ContainerPtr found = probe (top_level, item);
		if (found) {
			//item->add_child (found);
			//std::cout << "\tFound: " << found << "\n";
			//probe_queue.push (found);
		} else {
			//XXX LOG
			break;
		}
	}
#endif

	if (list) {
		log_info ("------------------------------------------------------------\n");
		DumpVisitor dv;
		top_level->accept (dv);
		dv.dump();
		log_info ("------------------------------------------------------------\n");
	}

	if (dot) {
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

	int retval = 0;

	if (app) {
                std::shared_ptr<GuiApp> gui (new GuiApp (top_level));

                main_app = gui;

                retval = gui->run (1, argv);           //XXX argc
	}

	log_close();
	return retval;
}

#endif

#include <cstdint>
#include "variant.h"
int main()
{
	ContainerPtr cp = Container::create();

	std::string a = "abc";
	double      b = 100;
#if 0
	bool        c = 101;
	uint8_t     d = 102;
	int8_t      e = 103;
	uint16_t    f = 104;
	int16_t     g = 105;
	uint32_t    h = 106;
	int32_t     i = 107;
	uint64_t    j = 108;
	int64_t     k = 109;
#endif

	cp->declare_prop ("main", "a", a, "apple");
	cp->declare_prop ("main", "b", b, "banana");

	VPtr v = cp->get_prop ("a");

	std::cout << (std::string) *v << std::endl;

#if 0
	Variant<std::string> va("main", "a", a, "param a");

	std::map<std::string,BaseVariant> props;

	props["xyz"] = va;

	Variant<std::string> vb = va;
	auto vc = vb;

	std::cout << "Props: " << props.size() << std::endl;
#endif

#if 0
	Variant<std::string> va("main", "a", a, "param a");
	Variant<double>      vb("main", "b", b, "param b");
	Variant<bool>        vc("main", "c", c, "param c");
	Variant<uint8_t>     vd("main", "d", d, "param d");
	Variant<int8_t>      ve("main", "e", e, "param e");
	Variant<uint16_t>    vf("main", "f", f, "param f");
	Variant<int16_t>     vg("main", "g", g, "param g");
	Variant<uint32_t>    vh("main", "h", h, "param h");
	Variant<int32_t>     vi("main", "i", i, "param i");
	Variant<uint64_t>    vj("main", "j", j, "param j");
	Variant<int64_t>     vk("main", "k", k, "param k");
#endif

#if 0
	std::cout << "uint16_t:" << std::endl;
	try { std::cout << "\tstd::string " << std::flush; std::cout << (uint16_t) va << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tdouble      " << std::flush; std::cout << (uint16_t) vb << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tbool        " << std::flush; std::cout << (uint16_t) vc << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint8_t     " << std::flush; std::cout << (uint16_t) vd << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint8_t      " << std::flush; std::cout << (uint16_t) ve << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint16_t    " << std::flush; std::cout << (uint16_t) vf << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint16_t     " << std::flush; std::cout << (uint16_t) vg << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint32_t    " << std::flush; std::cout << (uint16_t) vh << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint32_t     " << std::flush; std::cout << (uint16_t) vi << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint64_t    " << std::flush; std::cout << (uint16_t) vj << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint64_t     " << std::flush; std::cout << (uint16_t) vk << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	std::cout << std::endl;
#endif

}
