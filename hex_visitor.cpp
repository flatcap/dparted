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

#include <iostream>

#include "hex_visitor.h"

#include "container.h"
#include "utils.h"

HexVisitor::HexVisitor (void)
{
}

HexVisitor::~HexVisitor()
{
}


bool
HexVisitor::visit_enter (ContainerPtr& UNUSED(c))
{
	indent++;
	return true;
}

bool
HexVisitor::visit_leave (void)
{
	indent--;
	return true;
}


/**
 * visit (ContainerPtr)
 */
bool
HexVisitor::visit (ContainerPtr c)
{
	if (c->name != "dummy") {
#if 0
		std::string tabs;
		if (indent > 0) {
			tabs.resize (indent, '\t');
		}
		output << tabs;
#endif

		unsigned char* buf = nullptr;
		const int bufsize = 1024;

		buf = c->get_buffer (0, bufsize);
		if (buf) {
			std::cout << "\033[01;32m" << c << "\033[0m\n";
			//std::cout << (void*) buf << "\n";
			dump_hex2 (buf, 0, bufsize);
			printf ("\n");
		} else {
			std::cout << "\033[01;31m" << c << "\033[0m\n";
		}
	}

	return true;
}


void
HexVisitor::hexdump (void)
{
	std::cout << output.str();
}

