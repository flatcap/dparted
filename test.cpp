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

#include "gpt.h"
#include "gpt_partition.h"
#include "log.h"
#include "loop.h"
#include "partition.h"
#include "test.h"
#include "utils.h"

const int one_gig = 1073741824;

void
test_generate (ContainerPtr& parent, const std::string& name)
{
	return_if_fail (parent);

	if (name == "add")
		test_generate_add (parent);
	else if (name == "move")
		test_generate_move (parent);
	else if (name == "resize")
		test_generate_resize (parent);
	else
		log_error ("Unknown test case: %s", name.c_str());
}

void
test_generate_add (ContainerPtr& parent)
{
	return_if_fail (parent);

	const std::vector<std::tuple<int,bool,std::uint64_t,std::uint64_t>> v {
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

	log_info ("Test case: add");

	ContainerPtr new_parent = Container::start_transaction (parent, "Test case: add");
	if (!new_parent)
		return;

	int index_old = 0;
	int index = 0;
	bool part = false;
	std::uint64_t off = 0;
	std::uint64_t size = 0;
	LoopPtr loop;
	GptPtr gpt;

	for (auto i : v) {
		std::tie (index, part, off, size) = i;
		log_info ("%d, %d, %ld, %ld", index, part, off, size);

		if (index != index_old) {
			index_old = index;
			loop = Loop::create();
			if (!loop)
				break;

			loop->bytes_size = one_gig;
			loop->name = "loop" + std::to_string (index);
			loop->device = "/dev/zero";
			loop->device_major = 7;
			loop->device_minor = index;

			new_parent->add_child (loop, false);

			// ----------------------------------------

			gpt = Gpt::create();
			if (!gpt)
				break;

			gpt->bytes_size = loop->bytes_size;
			gpt->name = "Gpt";

			loop->add_child (gpt, false);
		}

		if (part) {
			GptPartitionPtr part = GptPartition::create();
			part->parent_offset = off;
			part->bytes_size    = size;
			part->name          = "part" + std::to_string (index);

			gpt->add_child (part, false);
		} else {
			PartitionPtr space = Partition::create();
			space->bytes_size    = size;
			space->bytes_used    = size;
			space->parent_offset = off;
			space->sub_type ("Space");
			space->sub_type ("Unallocated");
			space->name = "Unallocated";

			gpt->add_child (space, false);
		}
	}

	Container::commit_transaction();
}

void
test_generate_move (ContainerPtr& parent)
{
	return_if_fail (parent);

	const std::vector<std::pair<std::uint64_t,std::uint64_t>> v {
		//   Offset,      Size
		{         0, 262144000 },
		{         0, 262144000 },
		{ 268435456, 262144000 },
		{ 268435456, 262144000 },
		{ 268435456, 262144000 },
		{ 805306368, 268435456 },
		{ 805306368, 268435456 },
		{ 104857600, 734003200 },
		{ 209715200, 734003200 }
	};

	log_info ("Test case: move");

	ContainerPtr new_parent = Container::start_transaction (parent, "Test case: move");
	if (!new_parent)
		return;

	for (std::size_t i = 1; i <= v.size(); ++i) {
		LoopPtr loop = Loop::create();

		if (!loop)
			break;

		loop->bytes_size = one_gig;
		loop->name = "loop" + std::to_string(i);
		loop->device = "/dev/zero";
		loop->device_major = 7;
		loop->device_minor = i;

		new_parent->add_child (loop, false);

		// ----------------------------------------

		GptPtr gpt = Gpt::create();
		if (!gpt)
			break;

		gpt->bytes_size = loop->bytes_size;
		gpt->name = "Gpt";

		loop->add_child (gpt, false);

		// ----------------------------------------

		std::uint64_t start = v[i-1].first;
		std::uint64_t size  = v[i-1].second;

		GptPartitionPtr part = GptPartition::create();
		part->parent_offset = start;
		part->bytes_size    = size;
		part->name          = "part" + std::to_string (i);

		gpt->add_child (part, false);

		// ----------------------------------------

		if (start > 0) {
			PartitionPtr space = Partition::create();
			space->bytes_size    = start;
			space->bytes_used    = space->bytes_size;
			space->parent_offset = 0;
			space->sub_type ("Space");
			space->sub_type ("Unallocated");
			space->name = "Unallocated";

			gpt->add_child (space, false);
		}

		if ((start + size) < one_gig) {
			PartitionPtr space = Partition::create();
			space->bytes_size    = one_gig - (start+size);
			space->bytes_used    = space->bytes_size;
			space->parent_offset = (start+size);
			space->sub_type ("Space");
			space->sub_type ("Unallocated");
			space->name = "Unallocated";

			gpt->add_child (space, false);
		}
	}

	Container::commit_transaction();
}

void
test_generate_resize (ContainerPtr& parent)
{
	return_if_fail (parent);

	const std::vector<std::pair<std::uint64_t,std::uint64_t>> v {
		//   Offset,      Size
		{         0, 536870912 },
		{         0, 536870912 },
		{ 357564416, 357564416 },
		{ 357564416, 357564416 },
		{ 357564416, 357564416 },
		{ 357564416, 357564416 },
		{ 357564416, 357564416 },
		{ 357564416, 357564416 },
		{ 357564416, 357564416 },
		{ 357564416, 357564416 },
		{ 536870912, 536870912 },
		{ 536870912, 536870912 }
	};

	log_info ("Test case: resize");

	ContainerPtr new_parent = Container::start_transaction (parent, "Test case: resize");
	if (!new_parent)
		return;

	for (std::size_t i = 1; i <= v.size(); ++i) {
		LoopPtr loop = Loop::create();

		if (!loop)
			break;

		loop->bytes_size = one_gig;
		loop->name = "loop" + std::to_string(i);
		loop->device = "/dev/zero";
		loop->device_major = 7;
		loop->device_minor = i;

		new_parent->add_child (loop, false);

		// ----------------------------------------

		GptPtr gpt = Gpt::create();
		if (!gpt)
			break;

		gpt->bytes_size = loop->bytes_size;
		gpt->name = "Gpt";

		loop->add_child (gpt, false);

		// ----------------------------------------

		std::uint64_t start = v[i-1].first;
		std::uint64_t size  = v[i-1].second;

		GptPartitionPtr part = GptPartition::create();
		part->parent_offset = start;
		part->bytes_size    = size;
		part->name          = "part" + std::to_string (i);

		gpt->add_child (part, false);

		// ----------------------------------------

		if (start > 0) {
			PartitionPtr space = Partition::create();
			space->bytes_size    = start;
			space->bytes_used    = space->bytes_size;
			space->parent_offset = 0;
			space->sub_type ("Space");
			space->sub_type ("Unallocated");
			space->name = "Unallocated";

			gpt->add_child (space, false);
		}

		if ((start + size) < one_gig) {
			PartitionPtr space = Partition::create();
			space->bytes_size    = one_gig - (start+size);
			space->bytes_used    = space->bytes_size;
			space->parent_offset = (start+size);
			space->sub_type ("Space");
			space->sub_type ("Unallocated");
			space->name = "Unallocated";

			gpt->add_child (space, false);
		}
	}

	Container::commit_transaction();
}


void
test_execute (ContainerPtr& child, const std::string& name)
{
	return_if_fail (child);

	if (name == "add")
		test_execute_add (child);
	else if (name == "move")
		test_execute_move (child);
	else if (name == "resize")
		test_execute_resize (child);
	else
		log_error ("Unknown test case: %s", name.c_str());
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

	log_info ("ADD parent %s(%p), child %s(%p)", parent->get_name_default().c_str(), parent.get(), child->get_name_default().c_str(), child.get());
	log_info ("po   = %ld", child->parent_offset);
	log_info ("size = %ld", child->bytes_size);
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

	new_parent->add_child (part, false);
	Container::commit_transaction();
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

	log_info ("MOVE parent %s(%p), child %s(%p)", parent->get_name_default().c_str(), parent.get(), child->get_name_default().c_str(), child.get());
	std::string desc = "Test: move " + child->get_name_default();
	std::string name = child->name;
	std::uint64_t off  = 0;

	if (name == "part1") { off = 536870912; }
	if (name == "part2") { off = 811597824; }
	if (name == "part3") { off =         0; }
	if (name == "part4") { off = 644243456; }
	if (name == "part5") { off = 811597824; }
	if (name == "part6") { off =         0; }
	if (name == "part7") { off = 536870912; }
	if (name == "part8") { off = 209715200; }
	if (name == "part9") { off = 104857600; }

	ContainerPtr new_parent = Container::start_transaction (parent, desc);
	if (!new_parent)
		return;

	new_parent->move_child (child, off, child->bytes_size);
	Container::commit_transaction();
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

	log_info ("RESIZE parent %s(%p), child %s(%p)", parent->get_name_default().c_str(), parent.get(), child->get_name_default().c_str(), child.get());
	std::string desc = "Test: resize " + child->get_name_default();
	std::string name = child->name;
	std::uint64_t off  = 0;
	std::uint64_t size = 0;

	if (name == "part1")  { off =         0; size =   859832320; }
	if (name == "part2")  { off =         0; size =  1073741824; }
	if (name == "part3")  { off = 357564416; size =   502267904; }
	if (name == "part4")  { off = 357564416; size =   716177408; }
	if (name == "part5")  { off = 214958080; size =   644874240; }
	if (name == "part6")  { off =         0; size =   859832320; }
	if (name == "part7")  { off = 214958080; size =   858783744; }
	if (name == "part8")  { off =         0; size =  1073741824; }
	if (name == "part9")  { off = 214958080; size =   500170752; }
	if (name == "part10") { off =         0; size =   715128832; }
	if (name == "part11") { off = 214958080; size =   858783744; }
	if (name == "part12") { off =         0; size =  1073741824; }

	ContainerPtr new_parent = Container::start_transaction (parent, desc);
	if (!new_parent)
		return;

	new_parent->move_child (child, off, size);
	Container::commit_transaction();
}

