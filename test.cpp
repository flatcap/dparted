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

#include "gpt.h"
#include "gpt_partition.h"
#include "log.h"
#include "loop.h"
#include "partition.h"
#include "test.h"
#include "utils.h"

bool
generate_test (ContainerPtr& parent, std::string name)
{
	const int one_gig = 1073741824;

	log_error ("generate_test %s", name.c_str());

	ContainerPtr new_parent = Container::start_transaction (parent, "Loop: fake devices");
	if (!new_parent)
		return false;

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

		static std::vector<std::pair<std::uint64_t,std::uint64_t>> v {
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
			space->name = "space";

			gpt->add_child (space, false);
		}

		if ((start + size) < one_gig) {
			PartitionPtr space = Partition::create();
			space->bytes_size    = one_gig - (start+size);
			space->bytes_used    = space->bytes_size;
			space->parent_offset = (start+size);
			space->sub_type ("Space");
			space->sub_type ("Unallocated");
			space->name = "space";

			gpt->add_child (space, false);
		}
	}

	Container::commit_transaction();
	return true;
}

