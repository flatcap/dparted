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
#include <string>
#include <vector>

typedef std::shared_ptr<class Transaction> TransactionPtr;
typedef std::weak_ptr  <class Transaction> TransactionWeak;

enum class NotifyType {
	t_add,		// Parent,    New Child
	t_delete,	// Parent,    Old Child
	t_change	// Old Child, New Child
};

#include "container.h"

typedef std::tuple<NotifyType,ContainerWeak,ContainerWeak> Notification;

class Transaction
{
public:
	static TransactionPtr create (void);
	virtual ~Transaction();

	std::chrono::steady_clock::time_point timestamp;
	std::string description;
	std::vector<Notification> notifications;

protected:
	Transaction (void);

	TransactionWeak self;
};

#endif // _TRANSACTION_H_

