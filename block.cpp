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

#include <fcntl.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/major.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <sstream>
#include <string>

#include "block.h"
#include "disk.h"
#include "file.h"
#include "log.h"
#include "loop.h"
#include "main.h"
#include "log_trace.h"

/**
 * Block
 */
Block::Block (void)
{
	declare ("block");
}

/**
 * create
 */
BlockPtr
Block::create (void)
{
	BlockPtr b (new Block());

	b->weak = b;
	return b;
}


