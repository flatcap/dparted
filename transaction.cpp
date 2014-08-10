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
	timestamp (std::chrono::steady_clock::now()),
	write_lock(m)
{
	write_lock.lock();
}

Transaction::~Transaction()
{
	write_lock.unlock();
}

TransactionPtr
Transaction::create (std::mutex& write_lock)
{
	TransactionPtr p (new Transaction (write_lock));
	p->self = p;

	return p;
}

