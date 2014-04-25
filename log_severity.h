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

enum class Severity;

inline Severity operator| (Severity a, Severity b) { return (Severity) ((std::uint32_t) a | (std::uint32_t) b); }
inline Severity operator& (Severity a, Severity b) { return (Severity) ((std::uint32_t) a & (std::uint32_t) b); }
inline Severity operator~ (Severity a) { return (Severity) (~(std::uint32_t) a); }

enum class Severity {
	// EMERG   0 system is unusable
	SystemEmergency	= (1 <<  0),		// Kernel panic

	// ALERT   1 action must be taken immediately
	SystemAlert	= (1 <<  1),            // Action must be taken immediately

	// CRIT    2 critical conditions
	Critical	= (1 <<  2),            // Operation failed, damage may have occurred

	// ERR     3 error conditions
	Error		= (1 <<  3),            // Operation failed, no damage done
	Perror		= (1 <<  4),            // Message : standard error description
	Code		= (1 <<  5),            // There's a mistake in the code

	// WARNING 4 warning conditions
	Warning		= (1 <<  6),            // You should backup before starting

	// NOTICE  5 normal but significant condition
	Verbose		= (1 <<  7),            // Forced to continue
	User		= (1 <<  8),            // Asked user "Are you sure?"

	// INFO    6 informational
	Info		= (1 <<  9),            // Volume needs defragmenting
	Progress	= (1 << 10),            // 54% complete
	Quiet		= (1 << 11),            // Quietable output
	Command		= (1 << 12),            // Running "command"

	// DEBUG   7 debug-level messages
	Debug		= (1 << 13),            // x = 42
	Trace		= (1 << 14),            // Before calculate
	CommandIn	= (1 << 15),            // Sending data to command: ...
	CommandOut	= (1 << 16),            // Received data from command: ...
	IoIn		= (1 << 17),            // Read data from file
	IoOut		= (1 << 18),            // Wrote data to file
	Dot		= (1 << 19),            // Create dot output
	Hex		= (1 << 20),            // Memory dump
	ConfigRead	= (1 << 21),            // Read  config entry
	ConfigWrite	= (1 << 22),            // Wrote config entry
	Enter		= (1 << 23),            // Enter a function
	Leave		= (1 << 24),            // Leave a function
	File		= (1 << 25),            // File opened/closed/mmaped
	Ctor		= (1 << 26),		// Constructor
	Dtor		= (1 << 27),		// Destructor
	Thread		= (1 << 28),		// Thread started

	// Handy shortcuts
	NoMessages      = 0,
	AllMessages	= ~0,
	AllEmergency	= SystemEmergency,
	AllAlert	= SystemAlert,
	AllCritical	= Critical,
	AllError	= Error | Perror | Code,
	AllWarning	= Warning,
	AllNotice	= Verbose | User,
	AllInfo		= Info | Progress | Quiet | Command,
	AllDebug	= Debug | Trace | CommandIn | CommandOut | IoIn | IoOut | Dot | Hex | ConfigRead | ConfigWrite | Enter | Leave | File | Ctor | Dtor | Thread
};

#endif // _SEVERITY_H_

