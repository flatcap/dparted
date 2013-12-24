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

#ifndef _VISITOR_H_
#define _VISITOR_H_

#include <memory>

#include "pointers.h"

#include "block.h"
#include "container.h"
#include "disk.h"
#include "extended.h"
#include "file.h"
#include "filesystem.h"
#include "gpt.h"
#include "loop.h"
#include "lvm_group.h"
#include "lvm_linear.h"
#include "lvm_metadata.h"
#include "lvm_mirror.h"
#include "lvm_partition.h"
#include "lvm_raid.h"
#include "lvm_stripe.h"
#include "lvm_table.h"
#include "lvm_volume.h"
#include "md_group.h"
#include "md_table.h"
#include "misc.h"
#include "msdos.h"
#include "partition.h"
#include "table.h"
#include "volume.h"
#include "whole.h"

/**
 * class Visitor
 */
class Visitor
{
public:
	virtual bool visit_enter (ContainerPtr& c) { return true; }
	virtual bool visit_leave (void)            { return true; }

	virtual bool visit (ContainerPtr c) = 0;

	//XXX move the dirty inheritance stuff elsewhere? (hideous dependency problem)
	virtual bool visit (BlockPtr        b) { return visit (std::dynamic_pointer_cast<Container> (b)); }
	virtual bool visit (DiskPtr         d) { return visit (std::dynamic_pointer_cast<Block>       (d)); }
	virtual bool visit (ExtendedPtr     e) { return visit (std::dynamic_pointer_cast<Msdos>       (e)); }
	virtual bool visit (FilePtr         f) { return visit (std::dynamic_pointer_cast<Block>       (f)); }
	virtual bool visit (FilesystemPtr   f) { return visit (std::dynamic_pointer_cast<Container> (f)); }
	virtual bool visit (GptPtr          g) { return visit (std::dynamic_pointer_cast<Table>       (g)); }
	virtual bool visit (LoopPtr         l) { return visit (std::dynamic_pointer_cast<Block>       (l)); }
	virtual bool visit (LvmGroupPtr     l) { return visit (std::dynamic_pointer_cast<Whole>       (l)); }
	virtual bool visit (LvmLinearPtr    l) { return visit (std::dynamic_pointer_cast<LvmVolume>   (l)); }
	virtual bool visit (LvmMetadataPtr  l) { return visit (std::dynamic_pointer_cast<LvmLinear>   (l)); }
	virtual bool visit (LvmMirrorPtr    l) { return visit (std::dynamic_pointer_cast<LvmVolume>   (l)); }
	virtual bool visit (LvmPartitionPtr l) { return visit (std::dynamic_pointer_cast<Partition>   (l)); }
	virtual bool visit (LvmRaidPtr      l) { return visit (std::dynamic_pointer_cast<LvmVolume>   (l)); }
	virtual bool visit (LvmStripePtr    l) { return visit (std::dynamic_pointer_cast<LvmVolume>   (l)); }
	virtual bool visit (LvmTablePtr     l) { return visit (std::dynamic_pointer_cast<Table>       (l)); }
	virtual bool visit (LvmVolumePtr    l) { return visit (std::dynamic_pointer_cast<Volume>      (l)); }
	virtual bool visit (MdGroupPtr      m) { return visit (std::dynamic_pointer_cast<Whole>       (m)); }
	virtual bool visit (MdTablePtr      m) { return visit (std::dynamic_pointer_cast<Table>       (m)); }
	virtual bool visit (MiscPtr         m) { return visit (std::dynamic_pointer_cast<Container> (m)); }
	virtual bool visit (MsdosPtr        m) { return visit (std::dynamic_pointer_cast<Table>       (m)); }
	virtual bool visit (PartitionPtr    p) { return visit (std::dynamic_pointer_cast<Container> (p)); }
	virtual bool visit (TablePtr        t) { return visit (std::dynamic_pointer_cast<Container> (t)); }
	virtual bool visit (VolumePtr       v) { return visit (std::dynamic_pointer_cast<Whole>       (v)); }
	virtual bool visit (WholePtr        w) { return visit (std::dynamic_pointer_cast<Container> (w)); }
};


#endif // _VISITOR_H_

