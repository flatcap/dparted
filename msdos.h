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

#ifndef _MSDOS_H_
#define _MSDOS_H_

#include <memory>
#include <string>
#include <vector>

#include "table.h"

class Msdos;

typedef std::shared_ptr<Msdos> MsdosPtr;

struct partition {
	std::uint64_t start;
	std::uint64_t size;
	std::uint8_t  type;
};

class Msdos : public Table
{
public:
	static MsdosPtr create (void);
	virtual ~Msdos();
	virtual bool accept (Visitor& v);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

	static bool probe (ContainerPtr& parent, std::uint8_t* buffer, std::uint64_t bufsize);

public:
	// properties

protected:
	Msdos (void);

	virtual bool read_partition (std::uint8_t* buffer, int index, struct partition* part);
	virtual unsigned int read_table (std::uint8_t* buffer, std::uint64_t bufsize, std::uint64_t offset, std::vector<struct partition>& vp);
	// void read_chs (std::uint8_t* buffer, std::uint16_t& cylinder, std::uint8_t& head, std::uint8_t& sector);
};

#endif // _MSDOS_H_

