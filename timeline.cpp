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

#include <chrono>

#include "container.h"
#include "log.h"
#include "timeline.h"
#include "transaction.h"
#include "utils.h"

Timeline::Timeline (void)
{
	LOG_CTOR;
}

Timeline::~Timeline()
{
	// for (auto& t : txn_list) {
	// 	std::chrono::steady_clock::time_point then = std::get<0>(t);
	// 	int ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - then).count();
	// 	log_debug ("%3dms ago (%d): %p/%p : %s",
	// 		ms,
	// 		std::get<1>(t),
	// 		std::get<2>(t).get(),
	// 		std::get<3>(t).get(),
	// 		std::get<4>(t).c_str());
	// }
	LOG_DTOR;
}

TimelinePtr
Timeline::create (ContainerPtr& cont)
{
	TimelinePtr p (new Timeline());
	p->self = p;
	p->top_level = cont;

	return p;
}


bool
Timeline::adjust (int amount)
{
	return_val_if_fail (amount, false);
	// log_code ("adjust timeline %+d", amount);

	// if (amount < 0) {
	// 	if (txn_cursor == std::begin (txn_list)) {
	// 		log_code ("already at the beginning");
	// 		return false;
	// 	}

	// 	txn_cursor--;

	// 	ContainerPtr cold = std::get<2>(*txn_cursor);
	// 	ContainerPtr cnew = std::get<3>(*txn_cursor);

	// 	log_info ("Undo event: %s", std::get<4>(*txn_cursor).c_str());
	// 	exchange (cold, cnew);
	// } else {
	// 	if (txn_cursor == std::end (txn_list)) {
	// 		log_info ("already at the end");
	// 		return false;
	// 	}

	// 	ContainerPtr cold = std::get<2>(*txn_cursor);
	// 	ContainerPtr cnew = std::get<3>(*txn_cursor);

	// 	log_info ("Redo event: %s", std::get<4>(*txn_cursor).c_str());
	// 	exchange (cold, cnew);

	// 	txn_cursor++;
	// }
	return false;
}


bool
Timeline::commit (TransactionPtr txn)
{
	return_val_if_fail (txn, false);

	const char *names[] = { "add", "delete", "change" };

	if (txn->notifications.empty()) {
		log_code ("empty transaction");
		return false;
	}

	auto n = txn->notifications[0];					// First notification is the top-level of all the changes
	exchange (std::get<1>(n).lock(), std::get<2>(n).lock());	// Put the new containers into place

	log_code ("Commit: %s", txn->description.c_str());
	for (auto n : txn->notifications) {
		NotifyType type = std::get<0>(n);
		std::stringstream n1; ContainerPtr c1 = std::get<1>(n).lock(); if (c1) n1 << c1;
		std::stringstream n2; ContainerPtr c2 = std::get<2>(n).lock(); if (c2) n2 << c2;

		log_code ("\t%s:", names[(int) type]);
		log_code ("\t\t%s", n1.str().c_str());
		log_code ("\t\t%s", n2.str().c_str());

		c1->notify (type, c1, c2);				// Let everyone know about the changes
	}

	txn_cursor = std::end (txn_list);

	return true;
}

