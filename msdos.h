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
	long start;
	long size;
	int type;
};

class Msdos : public Table
{
public:
	static MsdosPtr create (void);
	virtual ~Msdos();
	virtual bool accept (Visitor& v);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

	static ContainerPtr probe (ContainerPtr& top_level, ContainerPtr& parent, unsigned char* buffer, int bufsize);

public:
	//properties

protected:
	Msdos (void);

	virtual bool read_partition (unsigned char* buffer, int index, struct partition* part);
	virtual unsigned int read_table (unsigned char* buffer, int bufsize, long offset, std::vector<struct partition>& vp);
	virtual void read_chs (unsigned char* buffer, int* cylinder, int* head, int* sector);

private:

};

#endif // _MSDOS_H_

