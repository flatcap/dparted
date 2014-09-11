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

#include <cstdlib>
#include <sstream>
#include <string>

#include "filesystem.h"
#include "action.h"
#include "app.h"
#ifdef DP_BTRFS
#include "btrfs.h"
#endif
#ifdef DP_EXTFS
#include "extfs.h"
#endif
#ifdef DP_FS_MISC
#include "fs_get.h"
#include "fs_identify.h"
#endif
#include "log.h"
#ifdef DP_NTFS
#include "ntfs.h"
#endif
#include "partition.h"
#include "question.h"
#include "stringnum.h"
#include "utils.h"
#include "visitor.h"

Filesystem::Filesystem (void)
{
	LOG_CTOR;
	const char* me = "Filesystem";

	sub_type (me);
}

Filesystem::Filesystem (const Filesystem& c) :
	Container(c)
{
	Filesystem();
	LOG_CTOR;
	// No properties
}

Filesystem::Filesystem (Filesystem&& c)
{
	LOG_CTOR;
	swap(c);
}

Filesystem::~Filesystem()
{
	LOG_DTOR;
}

FilesystemPtr
Filesystem::create (void)
{
	FilesystemPtr p (new Filesystem());
	p->self = p;

	return p;
}


Filesystem&
Filesystem::operator= (const Filesystem& UNUSED(c))
{
	// No properties

	return *this;
}

Filesystem&
Filesystem::operator= (Filesystem&& c)
{
	swap(c);
	return *this;
}


void
Filesystem::swap (Filesystem& UNUSED(c))
{
	// No properties
}

void
swap (Filesystem& lhs, Filesystem& rhs)
{
	lhs.swap (rhs);
}


Filesystem*
Filesystem::clone (void)
{
	LOG_TRACE;
	return new Filesystem (*this);
}


bool
Filesystem::accept (Visitor& v)
{
	FilesystemPtr f = std::dynamic_pointer_cast<Filesystem> (get_smart());
	if (!v.visit(f))
		return false;

	return visit_children(v);
}


std::vector<Action>
Filesystem::get_actions (void)
{
	LOG_TRACE;
	std::vector<Action> actions = {
		{ "delete.filesystem.dialog", true  },
		{ "resize.filesystem.dialog", true  },
	};

	std::vector<Action> base_actions = Container::get_actions();

	actions.insert (std::end (actions), std::begin (base_actions), std::end (base_actions));

	return actions;
}

bool
Filesystem::perform_action (Action action)
{
	if (action.name == "delete.filesystem.dialog") {
		log_info ("Filesystem perform: %s", SP(action.name));
		delete_filesystem_dialog();
		return true;
	} else if (action.name == "delete.filesystem") {
		log_error ("REALLY delete filesystem");
		return true;
	} else {
		return Container::perform_action (action);
	}
}


bool
Filesystem::probe (ContainerPtr& parent, std::uint8_t* buffer, std::uint64_t bufsize)
{
	return_val_if_fail (parent,  false);
	return_val_if_fail (buffer,  false);
	return_val_if_fail (bufsize, false);
	LOG_TRACE;

	FilesystemPtr f;

	//XXX reorder by likelihood
	if (false) {}
#ifdef DP_BTRFS
	else if ((f = Btrfs::get_btrfs    (parent, buffer, bufsize))) {}
#endif
#ifdef DP_EXTFS
	else if ((f = Extfs::get_ext2     (parent, buffer, bufsize))) {}
	else if ((f = Extfs::get_ext3     (parent, buffer, bufsize))) {}
	else if ((f = Extfs::get_ext4     (parent, buffer, bufsize))) {}
#endif
#ifdef DP_NTFS
	else if ((f =  Ntfs::get_ntfs     (parent, buffer, bufsize))) {}
#endif
#ifdef DP_FS_MISC
	else if ((f =        get_reiserfs (parent, buffer, bufsize))) {}
	else if ((f =        get_swap     (parent, buffer, bufsize))) {}
	else if ((f =        get_vfat     (parent, buffer, bufsize))) {}
	else if ((f =        get_xfs      (parent, buffer, bufsize))) {}
#endif

	if (f) {
		log_debug ("volume: %s (%s), child: %s", SP(parent->name), SP(parent->get_type()), SP(f->name));

		ContainerPtr new_parent = Container::start_transaction (parent, "Filesystem: probe");
		if (!new_parent) {
			log_error ("Filesystem probe failed");
			return false;
		}

		if (new_parent->add_child (f, false)) {	//XXX assumption fs is a leaf, move this into get_*?
			Container::commit_transaction();
			return true;
		} else {
			Container::cancel_transaction();
			return false;
		}

	}

	return false;
}

bool
Filesystem::get_mounted_usage (ContainerPtr UNUSED(parent))
{
	return false;
}


void
Filesystem::delete_filesystem_dialog (void)
{
	LOG_TRACE;

	QuestionPtr q = Question::create (std::bind (&Filesystem::question_cb, this, std::placeholders::_1));

	q->type = Question::Type::Delete;

	q->input = {
		{ "title",     "Delete Filesystem",                           },
		{ "primary",   "delete primary",                              },
		{ "secondary", "delete secondary",                            },
		{ "image",     "gtk-delete",                                  },
		{ "help_url",  "http://en.wikipedia.org/wiki/Special:Random", }
	};

	can_delete(q);
	main_app->ask(q);
}

bool
Filesystem::can_delete (QuestionPtr q)
{
	return_val_if_fail (q, false);

	if (get_count_real_children() > 1)
		return false;

	q->options.push_back ({
		Option::Type::checkbox,
		"delete.filesystem",
		std::string ("Delete ") + get_type(),
		std::string ("\"") + get_name_default() + std::string ("\""),
		"1",
		get_smart(),
		true,
		false,
		-1, -1, -1, -1
	});

	ContainerPtr parent = get_parent();
	if (parent)
		return parent->can_delete(q);

	return false;
}

void
Filesystem::question_cb (QuestionPtr q)
{
	return_if_fail (q);

	log_info ("Question finished: %d", q->result);
	if (q->result != 99) {
		log_info ("User cancelled delete");
		return;
	}

	for (auto& o : q->options) {
		log_info ("\t[%c] %s", (o.value == "1") ? 'X' : ' ', SP(o.description));
	}

	for (auto& o : q->options) {
		if (o.value != "1") {
			break;
		}

		Action a { o.name, true };
		if (!o.object->perform_action (a)) {
			log_error ("action %s failed", SP(o.name));
			break;
		}
	}
}

