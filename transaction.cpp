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

Transaction::Transaction (std::mutex& m) :
	mutex_write_lock(m)
{
}

Transaction::~Transaction()
{
	mutex_write_lock.unlock();
}

TransactionPtr
Transaction::create (const ContainerPtr& cont, std::mutex& m)
{
	TransactionPtr p (new Transaction(m));
	p->self = p;
	cont->add_listener(p);

	return p;
}

void
Transaction::add_action (const std::string& action)
{
	actions.push_back (action);
}

void
Transaction::commit (void)
{
	log_code ("Actions:");
	for (auto& a : actions) {
		log_code ("\t%s", a.c_str());
	}
}

void
Transaction::container_added (const ContainerPtr& UNUSED(parent), const ContainerPtr& UNUSED(cont))
{
	LOG_TRACE;
}

void
Transaction::container_changed (const ContainerPtr& UNUSED(parent), const ContainerPtr& UNUSED(before), const ContainerPtr& UNUSED(after))
{
	LOG_TRACE;
}

