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

#include <string>
#include <vector>

#include "app.h"
#include "gpt.h"
#include "gpt_partition.h"
#ifdef DP_LIST
#include "list_visitor.h"
#endif
#include "log.h"
#include "loop.h"
#include "partition.h"
#include "test.h"
#include "utils.h"

const int one_gig = 1073741824;
typedef std::vector<std::tuple<int,bool,std::uint64_t,std::uint64_t>> disk_def;

void
create_disks (ContainerPtr parent, const disk_def& dd, std::string name)
{
	return_if_fail (parent);

	name = "Test case: " + name;
	ContainerPtr new_parent = Container::start_transaction (parent, name);
	if (!new_parent)
		return;

	int loop_num_old = 0;
	int loop_num = 0;
	int part_num = 0;
	bool part = false;
	std::uint64_t off = 0;
	std::uint64_t size = 0;
	LoopPtr loop;
	GptPtr gpt;

	for (auto i : dd) {
		std::tie (loop_num, part, off, size) = i;
		log_debug ("%d, %d, %ld, %ld", loop_num, part, off, size);

		if (loop_num != loop_num_old) {
			loop_num_old = loop_num;
			part_num = 0;

			loop = Loop::create();
			if (!loop)
				break;

			loop->bytes_size = one_gig;
			loop->name = "loop" + std::to_string (loop_num);
			loop->device = "/dev/zero";
			loop->device_major = 7;
			loop->device_minor = loop_num;

			if (!new_parent->add_child (loop, false))
				break;

			// ----------------------------------------

			gpt = Gpt::create();
			if (!gpt)
				break;

			gpt->bytes_size = loop->bytes_size;
			gpt->name = "Gpt";

			if (!loop->add_child (gpt, false))
				break;
		}

		if (part) {
			++part_num;

			GptPartitionPtr part = GptPartition::create();
			part->parent_offset = off;
			part->bytes_size    = size;
			part->name          = "loop" + std::to_string (loop_num) + "p" + std::to_string (part_num);

			if (!gpt->add_child (part, false))
				break;
		} else {
			PartitionPtr space = Partition::create();
			space->bytes_size    = size;
			space->bytes_used    = size;
			space->parent_offset = off;
			space->sub_type ("Space");
			space->sub_type ("Unallocated");
			space->name = "Unallocated";

			if (!gpt->add_child (space, false))
				break;
		}
	}

	Container::commit_transaction();
}


void
test_generate_add (ContainerPtr& parent)
{
	const disk_def dd {
		// Loop, Part, Offset, Size
		std::make_tuple (1, true,          0,  357564416),
		std::make_tuple (1, false, 357564416,  358612992),
		std::make_tuple (1, true,  716177408,  357564416),
		std::make_tuple (2, true,          0,  357564416),
		std::make_tuple (2, false, 357564416,  716177408),
		std::make_tuple (3, true,          0,  357564416),
		std::make_tuple (3, false, 357564416,  716177408),
		std::make_tuple (4, false,         0,  716177408),
		std::make_tuple (4, true,  716177408,  357564416),
		std::make_tuple (5, false,         0, 1073741824),
		std::make_tuple (6, false,         0, 1073741824),
		std::make_tuple (7, false,         0,  716177408),
		std::make_tuple (7, true,  716177408,  357564416),
		std::make_tuple (8, false,         0, 1073741824),
		std::make_tuple (9, false,         0, 1073741824)
	};

	create_disks (parent, dd, "add");
}

void
test_generate_delete (ContainerPtr& parent)
{
	const disk_def dd {
		// Loop, Part, Offset, Size
		std::make_tuple (1, true,          0,  357564416),
		std::make_tuple (1, true,  357564416,  358612992),
		std::make_tuple (1, true,  716177408,  357564416),
		std::make_tuple (2, true,          0,  357564416),
		std::make_tuple (2, true,  357564416,  358612992),
		std::make_tuple (2, false, 716177408,  357564416),
		std::make_tuple (3, true,          0,  357564416),
		std::make_tuple (3, true,  357564416,  716177408),
		std::make_tuple (4, false,         0,  357564416),
		std::make_tuple (4, true,  357564416,  358612992),
		std::make_tuple (4, true,  716177408,  357564416),
		std::make_tuple (5, false,         0,  357564416),
		std::make_tuple (5, true,  357564416,  358612992),
		std::make_tuple (5, false, 716177408,  357564416),
		std::make_tuple (6, false,         0,  357564416),
		std::make_tuple (6, true,  357564416,  716177408),
		std::make_tuple (7, true,          0,  716177408),
		std::make_tuple (7, true,  716177408,  357564416),
		std::make_tuple (8, true,          0,  716177408),
		std::make_tuple (8, false, 716177408,  357564416),
		std::make_tuple (9, true,          0, 1073741824)
	};

	create_disks (parent, dd, "delete");
}

void
test_generate_move (ContainerPtr& parent)
{
	const disk_def dd {
		// Loop, Part, Offset, Size
		std::make_tuple (1, true,          0,  357564416),
		std::make_tuple (1, false, 357564416,  716177408),
		std::make_tuple (2, true,          0,  357564416),
		std::make_tuple (2, false, 357564416,  716177408),
		std::make_tuple (3, false,         0,  268435456),
		std::make_tuple (3, true,  268435456,  262144000),
		std::make_tuple (3, false, 530579456,  543162368),
		std::make_tuple (4, false,         0,  268435456),
		std::make_tuple (4, true,  268435456,  262144000),
		std::make_tuple (4, false, 530579456,  543162368),
		std::make_tuple (5, false,         0,  268435456),
		std::make_tuple (5, true,  268435456,  262144000),
		std::make_tuple (5, false, 530579456,  543162368),
		std::make_tuple (6, false,         0,  805306368),
		std::make_tuple (6, true,  805306368,  268435456),
		std::make_tuple (7, false,         0,  805306368),
		std::make_tuple (7, true,  805306368,  268435456),
		std::make_tuple (8, false,         0,  104857600),
		std::make_tuple (8, true,  104857600,  734003200),
		std::make_tuple (8, false, 838860800,  234881024),
		std::make_tuple (9, false,         0,  209715200),
		std::make_tuple (9, true,  209715200,  734003200),
		std::make_tuple (9, false, 943718400,  130023424),
	};

	create_disks (parent, dd, "move");
}

void
test_generate_myriad (ContainerPtr& parent)
{
	return_if_fail (parent);
#if 0
	ContainerPtr c = Container::create();
	c->name = "top";
	all_children.push_back(c);

	for (int i = 0; i < 99; i++) {
#if 1
		start_thread (std::bind (add_child, i), "new");
#else
		add_child(i);
#endif
	}

	wait_for_threads();
	// log_info ("START: %d", count_containers(c));

#if 0
	for (int i = 0; i < 100; i++) {
		start_thread (std::bind (alter_child), "alter");
	}
#endif

#if 0
	for (int i = 0; i < 3; i++) {
		// start_thread (std::bind (delete_child), "delete");
		delete_child();
	}
#endif

	// wait_for_threads();

	// log_info ("Threads have finished");
	// run_list(c);
#if 1
	// tidy_children();
	log_info ("%dC/%ldV children", count_containers(c), all_children.size());
#endif
#endif
}

void
test_generate_resize (ContainerPtr& parent)
{
	const disk_def dd {
		// Loop, Part, Offset, Size
		std::make_tuple (1,  true,          0,  536870912),
		std::make_tuple (1,  false, 536870912,  536870912),
		std::make_tuple (2,  true,          0,  536870912),
		std::make_tuple (2,  false, 536870912,  536870912),
		std::make_tuple (3,  false,         0,  357564416),
		std::make_tuple (3,  true,  357564416,  358612992),
		std::make_tuple (3,  false, 716177408,  357564416),
		std::make_tuple (4,  false,         0,  357564416),
		std::make_tuple (4,  true,  357564416,  358612992),
		std::make_tuple (4,  false, 716177408,  357564416),
		std::make_tuple (5,  false,         0,  357564416),
		std::make_tuple (5,  true,  357564416,  358612992),
		std::make_tuple (5,  false, 716177408,  357564416),
		std::make_tuple (6,  false,         0,  357564416),
		std::make_tuple (6,  true,  357564416,  358612992),
		std::make_tuple (6,  false, 716177408,  357564416),
		std::make_tuple (7,  false,         0,  357564416),
		std::make_tuple (7,  true,  357564416,  358612992),
		std::make_tuple (7,  false, 716177408,  357564416),
		std::make_tuple (8,  false,         0,  357564416),
		std::make_tuple (8,  true,  357564416,  358612992),
		std::make_tuple (8,  false, 716177408,  357564416),
		std::make_tuple (9,  false,         0,  357564416),
		std::make_tuple (9,  true,  357564416,  358612992),
		std::make_tuple (9,  false, 716177408,  357564416),
		std::make_tuple (10, false,         0,  357564416),
		std::make_tuple (10, true,  357564416,  358612992),
		std::make_tuple (10, false, 716177408,  357564416),
		std::make_tuple (11, false,         0,  536870912),
		std::make_tuple (11, true,  536870912,  536870912),
		std::make_tuple (12, false,         0,  536870912),
		std::make_tuple (12, true,  536870912,  536870912),
	};

	create_disks (parent, dd, "resize");
}

void
test_generate (ContainerPtr& parent, const std::string& name)
{
	     if (name == "add")    test_generate_add    (parent);
	else if (name == "delete") test_generate_delete (parent);
	else if (name == "move")   test_generate_move   (parent);
	else if (name == "myriad") test_generate_myriad (parent);
	else if (name == "resize") test_generate_resize (parent);
	else
		log_error ("Unknown test case: %s", SP(name));
}


void
test_execute_add (ContainerPtr& child)
{
	return_if_fail (child);

	ContainerPtr parent = child->get_parent();
	if (!parent)
		return;

	ContainerPtr gp = parent->get_parent();
	if (!gp)
		return;

	log_info ("ADD parent %s(%p), child %s(%p)", SP(parent->get_name_default()), VP(parent), SP(child->get_name_default()), VP(child));
	log_debug ("po   = %ld", child->parent_offset);
	log_debug ("size = %ld", child->bytes_size);
	std::string desc = "Test: add " + child->get_name_default();
	std::string name = gp->name;

	std::uint64_t off  = 0;
	std::uint64_t size = 0;
	if (name == "loop1") { off = 357564416; size =  358612992; }
	if (name == "loop2") { off = 357564416; size =  358612992; }
	if (name == "loop3") { off = 357564416; size =  716177408; }
	if (name == "loop4") { off = 357564416; size =  358612992; }
	if (name == "loop5") { off = 357564416; size =  358612992; }
	if (name == "loop6") { off = 357564416; size =  716177408; }
	if (name == "loop7") { off =         0; size =  716177408; }
	if (name == "loop8") { off =         0; size =  716177408; }
	if (name == "loop9") { off =         0; size = 1073741824; }

	ContainerPtr part = Partition::create();
	part->sub_type ("TestAdd");
	part->parent_offset = off;
	part->bytes_size    = size;
	part->bytes_used    = size;
	part->name          = "wibble";

	ContainerPtr new_parent = Container::start_transaction (parent, desc);
	if (!new_parent)
		return;

	if (new_parent->add_child (part, false)) {
		Container::commit_transaction();
	} else {
		Container::cancel_transaction();
	}
}

void
test_execute_delete (ContainerPtr& child)
{
	return_if_fail (child);
	// main_app->get_timeline()->dump();

	ContainerPtr parent = child->get_parent();
	if (!parent)
		return;

	ContainerPtr top_level = child->get_top_level();
	if (!top_level)
		return;

	// log_info ("top_level = %s", SP(top_level->name));
	// run_list (top_level);

	std::string name = child->name;
	if ((name != "loop1p2") && (name != "loop2p2") && (name != "loop3p2") && (name != "loop4p1") && (name != "loop5p1") && (name != "loop6p1") && (name != "loop7p1") && (name != "loop8p1") && (name != "loop9p1"))
		return;

	log_info ("DELETE parent %s(%p), child %s(%p)", SP(parent->get_name_default()), VP(parent), SP(child->get_name_default()), VP(child));
	std::string desc = "Test: delete " + child->get_name_default();

	ContainerPtr new_parent = Container::start_transaction (parent, desc);
	if (!new_parent)
		return;

	if (new_parent->delete_child(child)) {
		Container::commit_transaction();
	} else {
		Container::cancel_transaction();
	}

	parent = nullptr;
	// main_app->get_timeline()->dump();
	// run_list (top_level);
}

void
test_execute_move (ContainerPtr& child)
{
	return_if_fail (child);

	ContainerPtr parent = child->get_parent();
	if (!parent)
		return;

	if (!child->is_a ("GptPartition"))
		return;

	log_info ("MOVE parent %s(%p), child %s(%p)", SP(parent->get_name_default()), VP(parent), SP(child->get_name_default()), VP(child));
	std::string desc = "Test: move " + child->get_name_default();
	std::string name = child->name;
	std::uint64_t off  = 0;

	if (name == "loop1p1") { off = 536870912; }
	if (name == "loop2p1") { off = 716177408; }
	if (name == "loop3p1") { off =         0; }
	if (name == "loop4p1") { off = 644243456; }
	if (name == "loop5p1") { off = 811597824; }
	if (name == "loop6p1") { off =         0; }
	if (name == "loop7p1") { off = 536870912; }
	if (name == "loop8p1") { off = 209715200; }
	if (name == "loop9p1") { off = 104857600; }

	ContainerPtr new_parent = Container::start_transaction (parent, desc);
	if (!new_parent)
		return;

	if (new_parent->move_child (child, off, child->bytes_size)) {
		Container::commit_transaction();
	} else {
		Container::cancel_transaction();
	}
}

void
test_execute_resize (ContainerPtr& child)
{
	return_if_fail (child);

	ContainerPtr parent = child->get_parent();
	if (!parent)
		return;

	if (!child->is_a ("GptPartition"))
		return;

	log_info ("RESIZE parent %s(%p), child %s(%p)", SP(parent->get_name_default()), VP(parent), SP(child->get_name_default()), VP(child));
	std::string desc = "Test: resize " + child->get_name_default();
	std::string name = child->name;
	std::uint64_t off  = 0;
	std::uint64_t size = 0;

	if (name == "loop1p1")  { off =         0; size =   859832320; }
	if (name == "loop2p1")  { off =         0; size =  1073741824; }
	if (name == "loop3p1")  { off = 357564416; size =   502267904; }
	if (name == "loop4p1")  { off = 357564416; size =   716177408; }
	if (name == "loop5p1")  { off = 214958080; size =   644874240; }
	if (name == "loop6p1")  { off =         0; size =   859832320; }
	if (name == "loop7p1")  { off = 214958080; size =   858783744; }
	if (name == "loop8p1")  { off =         0; size =  1073741824; }
	if (name == "loop9p1")  { off = 214958080; size =   500170752; }
	if (name == "loop10p1") { off =         0; size =   715128832; }
	if (name == "loop11p1") { off = 214958080; size =   858783744; }
	if (name == "loop12p1") { off =         0; size =  1073741824; }

	ContainerPtr new_parent = Container::start_transaction (parent, desc);
	if (!new_parent)
		return;

	if (new_parent->move_child (child, off, size)) {
		Container::commit_transaction();
	} else {
		Container::cancel_transaction();
	}
}

void
test_execute (ContainerPtr& child, const std::string& name)
{
	return_if_fail (child);

	if (name == "add")
		test_execute_add (child);
	else if (name == "delete")
		test_execute_delete (child);
	else if (name == "move")
		test_execute_move (child);
	else if (name == "resize")
		test_execute_resize (child);
	// else nothing to do
}

