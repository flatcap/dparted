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

#ifndef _TRANSACTION_H_
#define _TRANSACTION_H_

#include <memory>
#include <mutex>
#include <vector>

typedef std::shared_ptr<class Transaction> TransactionPtr;

#include "container.h"

class Transaction
{
public:
	static TransactionPtr create (std::mutex& m);
	virtual ~Transaction();

	void add_action (const std::string& action);
	void commit (void);

protected:
	Transaction (std::mutex& m);

	std::vector<std::string> actions;

private:
	std::weak_ptr<Transaction> self;
	std::mutex& mutex_write_lock;
};

#endif // _TRANSACTION_H_

