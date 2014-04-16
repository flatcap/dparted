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

#include <sstream>

#include "lvm_mirror.h"
#include "action.h"
#include "log.h"
#include "visitor.h"
#include "utils.h"

LvmMirror::LvmMirror (void)
{
	log_ctor ("ctor LvmMirror");
	const char* me = "LvmMirror";

	sub_type (me);
}

LvmMirror::~LvmMirror()
{
	log_dtor ("dtor LvmMirror");
}

LvmMirrorPtr
LvmMirror::create (void)
{
	LvmMirrorPtr p (new LvmMirror());
	p->self = p;

	return p;
}


bool
LvmMirror::accept (Visitor& v)
{
	LvmMirrorPtr l = std::dynamic_pointer_cast<LvmMirror> (get_smart());
	if (!v.visit(l))
		return false;

	return visit_children(v);
}


std::vector<Action>
LvmMirror::get_actions (void)
{
	LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.lvm_mirror", true },
	};

	std::vector<Action> parent_actions = LvmVolume::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

bool
LvmMirror::perform_action (Action action)
{
	if (action.name == "dummy.lvm_mirror") {
		log_debug ("LvmMirror perform: %s", action.name.c_str());
		return true;
	} else {
		return LvmVolume::perform_action (action);
	}
}


void
LvmMirror::add_child (ContainerPtr& child)
{
	return_if_fail (child);

	/* Check:
	 *	available space
	 *	alignment type
	 *	size (restrictions)
	 *	valid type within this parent
	 */


#if 0
	bool isvol = child->is_a ("LvmVolume");
	if (isvol) {
		add_segment (child);
		child->parent = this;
		return;
	}
#endif

#if 0
	if (children.size() > 0) {
		ContainerPtr last = children.back();

		last->next = child;
		child->prev = last;
	}
#endif
#if 0
	bytes_used += child->bytes_size;

	bool inserted = false;

	for (auto i = children.begin(); i != children.end(); ++i) {
		if ((*i)->parent_offset > child->parent_offset) {
			children.insert (i, child);
			inserted = true;
			break;
		}
	}

	if (!inserted) {
		children.push_back (child);
	}

	log_debug ("insert: %s (%s)", this->name.c_str(), child->name.c_str());

	child->parent = this;
#endif
	//XXX for now
	LvmVolume::add_child (child);
}

void
LvmMirror::delete_child (ContainerPtr& UNUSED(child))
{
}


