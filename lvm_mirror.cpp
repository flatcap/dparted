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

#include <sstream>

#include "lvm_mirror.h"
#include "log.h"
#include "log_trace.h"
#include "visitor.h"

/**
 * create
 */
LvmMirrorPtr
LvmMirror::create (void)
{
	LvmMirrorPtr l (new LvmMirror());
	l->declare ("lvm_mirror");
	l->weak = l;

	return l;
}

/**
 * accept
 */
bool
LvmMirror::accept (Visitor& v)
{
	LvmMirrorPtr l = std::dynamic_pointer_cast<LvmMirror> (get_smart());
	if (!v.visit (l))
		return false;
	return visit_children (v);
}


/**
 * add_child
 */
void
LvmMirror::add_child (ContainerPtr child)
{
	/* Check:
	 *	available space
	 *	alignment type
	 *	size (restrictions)
	 *	valid type within this parent
	 */


#if 0
	bool isvol = child->is_a("lvm_volume");
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

	for (auto i = children.begin(); i != children.end(); i++) {
		if ((*i)->parent_offset > child->parent_offset) {
			children.insert (i, child);
			inserted = true;
			break;
		}
	}

	if (!inserted) {
		children.push_back (child);
	}

	//log_debug ("insert: %s (%s)\n", this->name.c_str(), child->name.c_str());

	child->parent = this;
#endif
	//XXX for now
	LvmVolume::add_child (child);
}

/**
 * delete_child
 */
void
LvmMirror::delete_child (ContainerPtr child)
{
}


