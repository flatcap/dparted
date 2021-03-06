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
	LOG_CTOR;
	const char* me = "LvmMirror";

	sub_type (me);
}

LvmMirror::LvmMirror (const LvmMirror& c) :
	LvmVolume(c)
{
	LvmMirror();
	LOG_CTOR;
	// No properties
}

LvmMirror::LvmMirror (LvmMirror&& c)
{
	LOG_CTOR;
	swap(c);
}

LvmMirror::~LvmMirror()
{
	LOG_DTOR;
}

LvmMirrorPtr
LvmMirror::create (void)
{
	LvmMirrorPtr p (new LvmMirror());
	p->self = p;

	return p;
}


LvmMirror&
LvmMirror::operator= (const LvmMirror& UNUSED(c))
{
	// No properties

	return *this;
}

LvmMirror&
LvmMirror::operator= (LvmMirror&& c)
{
	swap(c);
	return *this;
}


void
LvmMirror::swap (LvmMirror& UNUSED(c))
{
	// No properties
}

void
swap (LvmMirror& lhs, LvmMirror& rhs)
{
	lhs.swap (rhs);
}


LvmMirror*
LvmMirror::clone (void)
{
	LOG_TRACE;
	return new LvmMirror (*this);
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

	ContainerPtr me = get_smart();
	std::vector<Action> actions = {
		{ "dummy.lvm_mirror", "Dummy/Lvm Mirror", me, true },
	};

	std::vector<Action> base_actions = LvmVolume::get_actions();

	actions.insert (std::end (actions), std::begin (base_actions), std::end (base_actions));

	return actions;
}

bool
LvmMirror::perform_action (Action action)
{
	if (action.name == "dummy.lvm_mirror") {
		log_debug ("LvmMirror perform: %s", SP(action.name));
		return true;
	} else {
		return LvmVolume::perform_action (action);
	}
}


bool
LvmMirror::add_child (ContainerPtr child, bool probe)
{
	return_val_if_fail (child, false);

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

	for (auto i = std::begin (children); i != std::end (children); ++i) {
		if ((*i)->parent_offset > child->parent_offset) {
			children.insert (i, child);
			inserted = true;
			break;
		}
	}

	if (!inserted) {
		children.push_back (child);
	}

	log_debug ("insert: %s (%s)", SP(this->name), SP(child->name));

	child->parent = this;
#endif
	//XXX for now
	return LvmVolume::add_child (child, probe);
}

bool
LvmMirror::delete_child (ContainerPtr UNUSED(child))
{
	LOG_NOTIMPL;
	return false;
}


