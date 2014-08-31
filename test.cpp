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

void
test_generate (ContainerPtr& parent, const std::string& name)
{
	return_if_fail (parent);

	if (name == "move")
		test_generate_move (parent);
	else
		log_error ("Unknown test case: %s", name.c_str());
}

void
test_generate_move (ContainerPtr& parent)
{
	return_if_fail (parent);

	const int one_gig = 1073741824;

	static std::vector<std::pair<std::uint64_t,std::uint64_t>> v {
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

	for (int i = 1; i <= 9; ++i) {
		LoopPtr loop = Loop::create();

		if (!loop)
			break;

		loop->bytes_size = one_gig;
		loop->name = "loop" + std::to_string(i);
		loop->device = "/dev/zero";

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

	if (name == "move")
		test_execute_move (child);
	else
		log_error ("Unknown test case: %s", name.c_str());
}

void
test_execute_move (ContainerPtr& child)
{
	return_if_fail (child);

	ContainerPtr parent = child->get_parent();

	// log_info ("%10ld  %10ld  %10ld", child->parent_offset, child->parent_offset + child->bytes_size, child->bytes_size);
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
	if (!new_parent) {
		return;
	}

	new_parent->move_child(child, off, child->bytes_size);
	Container::commit_transaction();
}

