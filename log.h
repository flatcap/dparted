/* Copyright (c) 2013 Richard Russon (FlatCap)
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Library General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _LOG_H_
#define _LOG_H_

enum class LogLevel {
/* Logging levels - Determine what gets logged */
	Debug		= (1 <<  0),	/* x = 42 */
	Trace		= (1 <<  1),	/* Entering function x() */
	Quiet		= (1 <<  2),	/* Quietable output */
	Info		= (1 <<  3),	/* Volume needs defragmenting */
	Verbose		= (1 <<  4),	/* Forced to continue */
	Progress	= (1 <<  5),	/* 54% complete */
	Warning		= (1 <<  6),	/* You should backup before starting */
	Error		= (1 <<  7),	/* Operation failed, no damage done */
	Perror		= (1 <<  8),	/* Message : standard error description */
	Critical	= (1 <<  9),	/* Operation failed,damage may have occurred */
	Enter		= (1 << 10),	/* Enter a function */
	Leave		= (1 << 11)	/* Leave a function */
};

int log_info  (const char* format, ...) __attribute__ ((format (printf, 1, 2)));
int log_error (const char* format, ...) __attribute__ ((format (printf, 1, 2)));
int log_debug (const char* format, ...) __attribute__ ((format (printf, 1, 2)));
int log_trace (const char* format, ...) __attribute__ ((format (printf, 1, 2)));

bool log_init (const char* name);
void log_close (void);

#if 0
unsigned int log_set_level (unsigned int level);
unsigned int log_get_level (void);
#endif

#endif // _LOG_H_

