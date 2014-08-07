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

std::mutex mutex_write_lock;

Transaction::Transaction (void)
{
}

Transaction::~Transaction()
{
	mutex_write_lock.unlock();

	log_code ("Actions:");
	for (auto a : actions) {
		log_code ("\t%s", a.c_str());
	}
}

TransactionPtr
Transaction::create (void)
{
	if (!mutex_write_lock.try_lock()) {
		log_code ("Can't get global write lock");
		return {};
	}

	TransactionPtr p (new Transaction());
	p->self   = p;

	return p;
}

