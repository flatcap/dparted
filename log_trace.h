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

#ifndef _LOG_TRACE_H_
#define _LOG_TRACE_H_

#include <string>

#include "log.h"

#define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))

//XXX keep a global depth counter?
//XXX tie into general logging (indent results)?

//#ifdef DEBUG
#define LOG_TRACE		LogTrace __le(__PRETTY_FUNCTION__, __FILE__, __LINE__)
//#else
//#define LOG_TRACE		do {} while (0)
//#endif

/**
 * class LogTrace
 */
class LogTrace
{
public:
	LogTrace (const std::string& function, const std::string& file_name, int line_num) :
		m_function (function),
		m_file_name (file_name),
		m_line_num (line_num)
	{
		log_trace ("Entering: %s -- %s:%d\n", m_function.c_str(), m_file_name.c_str(), m_line_num);
	}

	virtual ~LogTrace()
	{
		//log_trace ("Leaving:  %s -- %s\n", m_function.c_str(), m_file_name.c_str());
	}

protected:
	std::string	m_function;
	std::string	m_file_name;
	int		m_line_num;

private:
	// Stop users dynamically creating this object
	void* operator new (size_t s);
};


#endif // _LOG_TRACE_H_

