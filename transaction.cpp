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


#include "log.h"
#include "transaction.h"
#include "container.h"
#include "utils.h"

Transaction::Transaction (void) :
	timestamp (std::chrono::steady_clock::now())
{
	LOG_CTOR;
}

Transaction::~Transaction()
{
	LOG_DTOR;
}

TransactionPtr
Transaction::create (void)
{
	TransactionPtr p (new Transaction());
	p->self = p;

	return p;
}


void
Transaction::dump (int indent /*=0*/)
{
	std::string tabs;

	if (indent > 0) {
		tabs.resize (indent, '\t');
	}

	std::chrono::steady_clock::time_point now  = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point then = timestamp;
	int ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - then).count();

	log_code ("%sCommit: '%s' (%dms ago)", SP(tabs), SP(description), ms);
	for (auto n : notifications) {
		dump_notification (n, indent+1);
	}
}

void
Transaction::dump_notification (NotifyType type, ContainerPtr c1, ContainerPtr c2, int indent /*=0*/)
{
	return_if_fail (c1);
	return_if_fail (c2);

	std::string tabs;
	if (indent > 0) {
		tabs.resize (indent, '\t');
	}

	std::string name1 = c1->get_type() + "(" + std::to_string (c1->get_unique()) + ")";
	std::string name2 = c2->get_type() + "(" + std::to_string (c2->get_unique()) + ")";

#if 0
	log_code ("%s%-7s: %-16s(%p:%ld) : %-16s(%p:%ld)",
		tabs.c_str(),
		get_notify_name (type),
		name1.c_str(), c1.get(), c1.use_count(),
		name2.c_str(), c2.get(), c2.use_count());
#else
	log_code ("%s%-7s: %-16s : %-16s",
		tabs.c_str(),
		get_notify_name (type),
		name1.c_str(),
		name2.c_str());
#endif
}

void
Transaction::dump_notification (Notification& n, int indent /*=0*/)
{
	dump_notification (std::get<0>(n), std::get<1>(n), std::get<2>(n), indent);
}

const char*
Transaction::get_notify_name (NotifyType type)
{
	const char *name[] = { "add", "delete", "change" };

	return name[(int) type];
}

