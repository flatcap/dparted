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

#ifndef _THREAD_H_
#define _THREAD_H_

#include <thread>

#ifdef DP_NO_THREAD
#define THREAD FakeThread
struct FakeThread
{
	FakeThread (std::function<void(void)> fn) { fn(); }
	void detach (void) {}
	void join   (void) {}
};
#else
#define THREAD std::thread
#endif

#endif // _THREAD_H_

