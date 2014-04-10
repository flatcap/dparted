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

#ifndef _LOG_H_
#define _LOG_H_

#include "severity.h"
#include "log_macro.h"

int log_redirect (Severity sev, const char *function, const char *file, int line, ...);

void assertion_failure (const char* file, int line, const char* test, const char* function);

#define return_if_fail(TEST)									\
	do {											\
		if (TEST) {									\
		} else {									\
			assertion_failure (__FILE__, __LINE__, #TEST, __PRETTY_FUNCTION__);	\
			return;									\
		}										\
	} while(0)

#define return_val_if_fail(TEST,VALUE)								\
	do {											\
		if (TEST) {									\
		} else {									\
			assertion_failure (__FILE__, __LINE__, #TEST, __PRETTY_FUNCTION__);	\
			return VALUE;								\
		}										\
	} while(0)

// return_if_reached
// warn_if_reached
// warn_if_fail

bool log_init (const char* name);
void log_close (void);

#if 0
unsigned int log_set_level (unsigned int level);
unsigned int log_get_level (void);
#endif

#endif // _LOG_H_

