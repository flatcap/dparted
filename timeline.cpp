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
#ifdef DP_LIST
#include "list_visitor.h"
#endif
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
Timeline::undo (int count)
{
	if (count < 1) {
		log_code ("Can't undo %d transactions", count);
		return false;
	}

	int dist = std::distance (std::begin (txn_list), txn_cursor);
	if (count > dist) {
		log_error ("Can't undo %d transactions, there are only %d", count, dist);
		return false;
	}

	for (; count--; ) {
		--txn_cursor;

		auto txn = (*txn_cursor);

		log_info ("Undo event: '%s'", SP(txn->description));

		NotifyType   type;
		ContainerPtr top_old;
		ContainerPtr top_new;

		std::tie (type, top_old, top_new) = txn->notifications[0];

		exchange (top_new, top_old);
		top_old->notify (NotifyType::t_change, top_new, top_old);
		Transaction::dump_notification (type, top_new, top_old, 1);

		ContainerPtr cold;
		ContainerPtr cnew;

		auto start = std::end   (txn->notifications);
		auto end   = std::begin (txn->notifications);
		std::advance (start, -1);

		for (auto it = start; it != end; --it) {
			std::tie (type, cold, cnew) = (*it);

			// Any references to the top-level object, must use the right one
			if (cold == top_new) { cold = top_old; }
			if (cnew == top_new) { cnew = top_old; }

			switch (type) {
				case NotifyType::t_change:
					Transaction::dump_notification (NotifyType::t_change, cold, cnew, 1);
					cnew->notify (NotifyType::t_change, cnew, cold);	// back-to-front
					break;
				case NotifyType::t_add:	// Undoing therefore we delete
					Transaction::dump_notification (NotifyType::t_delete, cold, cnew, 1);
					cold->notify (NotifyType::t_delete, cold, cnew);
					break;
				case NotifyType::t_delete: // Undoing therefore we add
					Transaction::dump_notification (NotifyType::t_add, cold, cnew, 1);
					cold->notify (NotifyType::t_add, cold, cnew);
					break;
			}
		}
	}

	return true;
}

bool
Timeline::redo (int count)
{
	static bool b = false;
	if (!b) {
		dump();
		b = true;
	}

	if (count < 1) {
		log_code ("Can't redo %d transactions", count);
		return false;
	}

	int dist = std::distance (txn_cursor, std::end (txn_list));
	if (count > dist) {
		log_error ("Can't redo %d transactions, there are only %d", count, dist);
		return false;
	}

	for (; count--; ) {
		auto txn = (*txn_cursor);

		log_info ("Redo event: '%s'", SP(txn->description));

		NotifyType   type;
		ContainerPtr top_old;
		ContainerPtr top_new;

		std::tie (type, top_old, top_new) = txn->notifications[0];

		exchange (top_new, top_old);
		top_old->notify (NotifyType::t_change, top_new, top_old);
		Transaction::dump_notification (type, top_new, top_old, 1);

		ContainerPtr cold;
		ContainerPtr cnew;

		auto start = std::begin (txn->notifications);
		auto end   = std::end   (txn->notifications);
		std::advance (start, 1);

		for (auto it = start; it != end; ++it) {
			std::tie (type, cold, cnew) = (*it);

			// Any references to the top-level object, must use the right one
			if (cold == top_new) { cold = top_old; }
			if (cnew == top_new) { cnew = top_old; }

			switch (type) {
				case NotifyType::t_change:
					Transaction::dump_notification (NotifyType::t_change, cold, cnew, 1);
					cnew->notify (NotifyType::t_change, cnew, cold);	// back-to-front
					break;
				case NotifyType::t_add:
					Transaction::dump_notification (NotifyType::t_add, cold, cnew, 1);
					cold->notify (NotifyType::t_add, cold, cnew);
					break;
				case NotifyType::t_delete:
					Transaction::dump_notification (NotifyType::t_delete, cold, cnew, 1);
					cold->notify (NotifyType::t_delete, cold, cnew);
					break;
			}
		}

		++txn_cursor;
	}

	return true;
}

bool
Timeline::adjust (int amount)
{
	return_val_if_fail (amount, false);
	// log_code ("adjust timeline %+d", amount);

	// ContainerPtr top;
	bool result = false;
	if (amount < 0) {
		result = undo (-amount);
		// top = cold->get_top_level();
	} else {
		result = redo (amount);
		// top = cnew->get_top_level();
	}

	// run_list (top);
	return result;
}


bool
Timeline::commit (TransactionPtr txn)
{
	return_val_if_fail (txn, false);

	if (txn->notifications.empty()) {
		log_code ("empty transaction");
		return false;
	}

	auto n = txn->notifications[0];					// First notification is the top-level of all the changes
	exchange (std::get<1>(n), std::get<2>(n));			// Put the new containers into place

	log_code ("Commit: '%s'", SP(txn->description));
	for (auto n : txn->notifications) {
		//XXX std::tie
		NotifyType type = std::get<0>(n);
		std::stringstream n1; ContainerPtr c1 = std::get<1>(n); if (c1) n1 << c1;
		std::stringstream n2; ContainerPtr c2 = std::get<2>(n); if (c2) n2 << c2;

		// log_code ("\t%s:", (int) type);
		// log_code ("\t\t%s", n1.str().c_str());
		// log_code ("\t\t%s", n2.str().c_str());

		c1->notify (type, c1, c2);				// Let everyone know about the changes
	}

	txn_list.push_back (txn);
	txn_cursor = std::end (txn_list);

	// dump();
	return true;
}

void
Timeline::dump (void)
{
	log_info ("---------------------------------------------------------------------------------------------------------------");
	log_info ("Timeline: %ld events", txn_list.size());
	for (auto& t : txn_list) {
		t->dump();
	}
	log_info ("---------------------------------------------------------------------------------------------------------------");
}
