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

#include <iterator>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <algorithm>

#include "log.h"
#include "md_table.h"
#include "md_group.h"
#include "main.h"
#include "utils.h"
#include "log_trace.h"
#include "visitor.h"

/**
 * MdGroup
 */
MdGroup::MdGroup (void)
{
	declare ("md_group");
}

/**
 * create
 */
MdGroupPtr
MdGroup::create (void)
{
	MdGroupPtr m (new MdGroup());
	m->weak = m;

	return m;
}


/**
 * accept
 */
bool
MdGroup::accept (Visitor& v)
{
	MdGroupPtr m = std::dynamic_pointer_cast<MdGroup> (get_smart());
	if (!v.visit (m))
		return false;
	return visit_children (v);
}


/**
 * discover
 */
void
MdGroup::discover (ContainerPtr& top_level)
{
	LOG_TRACE;

	//std::string command =
}


#if 0
cat /proc/mdstat

mdadm --add /dev/md0 /dev/loop0
mdadm --add /dev/md0 /dev/loop1
mdadm --add /dev/md0 /dev/loop2

mdadm --assemble /dev/loop{45..47}
mdadm --assemble /dev/md0 /dev/loop{45..47}

mdadm --build /dev/md1 --level=linear /dev/loop{0,1,2}

mdadm --create /dev/md1 --build --level=linear /dev/loop{0,1,2}
mdadm --create /dev/md1 --raid-devices=3 /dev/loop{0,1,2}
mdadm --create /dev/md1 --raid-devices=linear /dev/loop{0,1,2}
mdadm --create /dev/md1 --raid=linear --raid-devices=3 /dev/loop{0,1,2}
mdadm --create /dev/md1 --raid=linear /dev/loop{0,1,2}
mdadm --create /dev/md1 /dev/loop{0,1,2}
mdadm --create /dev/md1 /dev/loop{0,1,2} --help

mdadm --detail --scan --verbose /dev/md0
mdadm --detail --scan --verbose /dev/md0 -v
mdadm --detail --scan --verbose /dev/md0 -vvvv
mdadm --detail --scan /dev/md0

mdadm --detail /dev/md0
mdadm --detail /dev/md0 --verbose
mdadm --detail /dev/md1
mdadm --detail /dev/md?

mdadm --examine /dev/loop45
mdadm --examine /dev/md0

mdadm --grow --size 10G /dev/md0
mdadm --grow /dev/md0
mdadm --grow /dev/md0 --help
mdadm --grow /dev/md0 /dev/loop0

mdadm --remove /dev/md0 /dev/loop0
mdadm --remove /dev/md0 /dev/loop1
mdadm --remove /dev/md0 /dev/loop2

mdadm --stop /dev/md0

mdadm /dev/md1 --build --level=linear /dev/loop{0,1,2}
mdadm /dev/md1 --create --verbose --level=linear --raid-devices=2 /dev/loop{0,1,2}
mdadm /dev/md1 --create --verbose --level=linear --raid-devices=3 /dev/loop{0,1,2}
mdadm /dev/md1 --create --verbose --level=linear -raid-devices=2 /dev/loop{0,1,2}
#endif

