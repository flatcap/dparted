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

#ifndef _TIMELINE_H_
#define _TIMELINE_H_

#include <chrono>
#include <ctime>
#include <deque>
#include <memory>
#include <string>
#include <tuple>

#include "container.h"
#include "transaction.h"

typedef std::shared_ptr<class Timeline> TimelinePtr;
typedef std::weak_ptr  <class Timeline> TimelineWeak;

class Timeline
{
public:
	static TimelinePtr create (ContainerPtr& cont);
	virtual ~Timeline();

	bool adjust (int amount);
	ContainerPtr get_top_level (void) { return top_level; }

	bool commit (TransactionPtr txn);

	void dump (void);

protected:
	Timeline (void);

	TimelineWeak self;
	ContainerPtr top_level;

	std::deque<TransactionPtr> txn_list;
	// txn_cursor points to the end of the txn_list unless we're
	// re-wound time.  In that case it points to the first redo item.
	std::deque<TransactionPtr>::iterator txn_cursor;
};

#endif // _TIMELINE_H_

