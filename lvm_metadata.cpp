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

#include "lvm_metadata.h"
#include "log_trace.h"
#include "visitor.h"

/**
 * LvmMetadata
 */
LvmMetadata::LvmMetadata (void)
{
	const char* me = "LvmMetadata";

	sub_type (me);
}

/**
 * create
 */
LvmMetadataPtr
LvmMetadata::create (void)
{
	LvmMetadataPtr l (new LvmMetadata());
	l->weak = l;

	return l;
}


/**
 * accept
 */
bool
LvmMetadata::accept (Visitor& v)
{
	LvmMetadataPtr l = std::dynamic_pointer_cast<LvmMetadata> (get_smart());
	if (!v.visit(l))
		return false;
	return visit_children(v);
}


/**
 * add_child
 */
void
LvmMetadata::add_child (ContainerPtr& child)
{
	if (!child)
		return;

	if (child->is_a ("LvmVolume")) {
		LvmVolumePtr v = std::dynamic_pointer_cast<LvmVolume> (child);
		sibling = v;
		v->sibling = get_smart();
	} else {
		// probably a partition
		add_segment (child);
		//log_debug ("segment: %s (%s) -- %s\n", this->name.c_str(), child->name.c_str(), child->uuid.c_str());
	}
}


/**
 * get_actions
 */
std::vector<Action>
LvmMetadata::get_actions (void)
{
	// LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.lvmmetadata", true },
	};

	std::vector<Action> parent_actions = LvmLinear::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

/**
 * perform_action
 */
bool
LvmMetadata::perform_action (Action action)
{
	if (action.name == "dummy.lvmmetadata") {
		std::cout << "LvmMetadata perform: " << action.name << std::endl;
		return true;
	} else {
		return LvmLinear::perform_action (action);
	}
}


