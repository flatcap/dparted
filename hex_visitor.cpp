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
	if (c->name == "dummy")
		return true;

	std::uint8_t* buf = nullptr;
	int bufsize = c->bytes_size;

	buf = c->get_buffer (0, bufsize);
	dump (c, buf, bufsize);

	return true;
}

#ifdef DP_MSDOS
/**
 * visit (ExtendedPtr)
 */
bool
HexVisitor::visit (ExtendedPtr c)
{
	// An Extended has size, but the device is limited to 1KiB
	std::uint8_t* buf = nullptr;
	int bufsize = 0;
	ContainerPtr parent;

	parent = c->parent.lock();
	if (!parent) {
		std::cout << "\033[01;31m" << c << "\033[0m\n";
		return false;
	}

	// Get our-sized buffer from parent
	bufsize = c->bytes_size;
	buf = parent->get_buffer (c->parent_offset, bufsize);
	dump (c, buf, bufsize);

	return true;
}

#endif
#ifdef DP_LVM
/**
 * visit (LvmGroupPtr)
 */
bool
HexVisitor::visit (LvmGroupPtr c)
{
	// An LvmGroup has segments, but no device
	std::uint8_t* buf = nullptr;
	int bufsize = 0;

	//XXX offsets should be cumulative across the segments
	for (auto i : c->segments) {
		bufsize = i->bytes_size;
		buf = i->get_buffer (0, bufsize);
		dump (i, buf, bufsize);
	}

	return true;
}

#endif

void
HexVisitor::dump (ContainerPtr c, std::uint8_t* buf, std::uint64_t size)
{
	if (!c)
		return;

	std::string type;
	for (auto i : c->type) {
		type += i + ".";
	}
	type.pop_back();

	std::cout << type << std::endl;
	if (buf) {
		log_debug ("%s: Offset: %ld (%ld MiB), Size: %ld (%ld MiB)\n", c->name.c_str(), c->parent_offset, c->parent_offset >> 20, c->bytes_size, c->bytes_size >> 20);

		std::uint64_t abbr = (abbreviate & ~15);	// Round down to multiple of 16

		if ((abbr == 0) || (abbr >= (size/2))) {
			dump_hex2 (buf, 0, size);
		} else {
			dump_hex2 (buf, 0, abbr);
			std::cout << "\t~~~\n";
			dump_hex2 (buf, size-abbr, abbr);
		}
		std::cout << std::endl;
	} else {
		std::cout << "\033[01;31m" << c << "\033[0m\n";
	}

}

