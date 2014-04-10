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

#ifndef _SEVERITY_H_
#define _SEVERITY_H_

enum class Severity {
	// EMERG   0 system is unusable
	SystemEmergency = 0 + ( 0 << 3),	// Kernel panic

	// ALERT   1 action must be taken immediately
	SystemAlert	= 1 + ( 0 << 3),	// Action must be taken immediately

	// CRIT    2 critical conditions
	Critical	= 2 + ( 0 << 3),	// Operation failed, damage may have occurred

	// ERR     3 error conditions
	Error		= 3 + ( 0 << 3),	// Operation failed, no damage done
	Perror		= 3 + ( 1 << 3),	// Message : standard error description
	Code		= 3 + ( 2 << 3),	// There's a mistake in the code

	// WARNING 4 warning conditions
	Warning		= 4 + ( 0 << 3),	// You should backup before starting

	// NOTICE  5 normal but significant condition
	Verbose		= 5 + ( 0 << 3),	// Forced to continue
	User		= 5 + ( 1 << 3),	// Asked user "Are you sure?"

	// INFO    6 informational
	Info		= 6 + ( 0 << 3),	// Volume needs defragmenting
	Progress	= 6 + ( 1 << 3),	// 54% complete
	Quiet		= 6 + ( 2 << 3),	// Quietable output

	// DEBUG   7 debug-level messages
	Debug		= 7 + ( 0 << 3),	// x = 42
	Trace		= 7 + ( 1 << 3),	// Before calculate
	CommandIn	= 7 + ( 2 << 3),	// Sending data to command: ...
	CommandOut	= 7 + ( 3 << 3),	// Received data from command: ...
	IoIn		= 7 + ( 4 << 3),	// Read data from file
	IoOut		= 7 + ( 5 << 3),	// Wrote data to file
	Dot		= 7 + ( 6 << 3),	// Create dot output
	Hex		= 7 + ( 7 << 3),	// Memory dump
	ConfigRead	= 7 + ( 8 << 3),	// Read config entry
	ConfigWrite	= 7 + ( 9 << 3),	// Wrote confi entry
	Enter		= 7 + (10 << 3),	// Enter a function
	Leave		= 7 + (11 << 3),	// Leave a function
	File		= 7 + (12 << 3),	// File opened/closed/mmaped
};

#endif // _SEVERITY_H_

