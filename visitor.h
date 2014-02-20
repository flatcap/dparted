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

#ifndef _VISITOR_H_
#define _VISITOR_H_

#include <memory>

#include "btrfs.h"
#include "container.h"
#include "device.h"
#include "disk.h"
#include "extended.h"
#include "extfs.h"
#include "file.h"
#include "filesystem.h"
#include "gpt.h"
#include "loop.h"
#include "luks.h"
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
#include "md_partition.h"
#include "md_table.h"
#include "misc.h"
#include "msdos.h"
#include "ntfs.h"
#include "partition.h"
#include "piece.h"
#include "table.h"
#include "volume.h"
#include "whole.h"
#include "log_trace.h"

class Visitor
{
public:
	virtual bool visit_enter (ContainerPtr& UNUSED(c)) { return true; }
	virtual bool visit_leave (void)                    { return true; }

	virtual bool visit (ContainerPtr c) = 0;

	//XXX move the dirty inheritance stuff elsewhere? (hideous dependency problem)
	virtual bool visit (BtrfsPtr        p) { return visit (std::dynamic_pointer_cast<Filesystem>(p)); }
	virtual bool visit (DiskPtr         p) { return visit (std::dynamic_pointer_cast<Device>    (p)); }
	virtual bool visit (ExtfsPtr        p) { return visit (std::dynamic_pointer_cast<Filesystem>(p)); }
	virtual bool visit (FilePtr         p) { return visit (std::dynamic_pointer_cast<Container> (p)); }
	virtual bool visit (FilesystemPtr   p) { return visit (std::dynamic_pointer_cast<Container> (p)); }
	virtual bool visit (GptPtr          p) { return visit (std::dynamic_pointer_cast<Table>     (p)); }
	virtual bool visit (LoopPtr         p) { return visit (std::dynamic_pointer_cast<Device>    (p)); }
	virtual bool visit (LuksPtr         p) { return visit (std::dynamic_pointer_cast<Device>    (p)); }
	virtual bool visit (LvmGroupPtr     p) { return visit (std::dynamic_pointer_cast<Whole>     (p)); }
	virtual bool visit (LvmLinearPtr    p) { return visit (std::dynamic_pointer_cast<LvmVolume> (p)); }
	virtual bool visit (LvmMetadataPtr  p) { return visit (std::dynamic_pointer_cast<LvmLinear> (p)); }
	virtual bool visit (LvmMirrorPtr    p) { return visit (std::dynamic_pointer_cast<LvmVolume> (p)); }
	virtual bool visit (LvmPartitionPtr p) { return visit (std::dynamic_pointer_cast<Piece>     (p)); }
	virtual bool visit (LvmRaidPtr      p) { return visit (std::dynamic_pointer_cast<LvmVolume> (p)); }
	virtual bool visit (LvmStripePtr    p) { return visit (std::dynamic_pointer_cast<LvmVolume> (p)); }
	virtual bool visit (LvmTablePtr     p) { return visit (std::dynamic_pointer_cast<Table>     (p)); }
	virtual bool visit (LvmVolumePtr    p) { return visit (std::dynamic_pointer_cast<Volume>    (p)); }
	virtual bool visit (MdGroupPtr      p) { return visit (std::dynamic_pointer_cast<Whole>     (p)); }
	virtual bool visit (MdPartitionPtr  p) { return visit (std::dynamic_pointer_cast<Piece>     (p)); }
	virtual bool visit (MdTablePtr      p) { return visit (std::dynamic_pointer_cast<Table>     (p)); }
	virtual bool visit (MiscPtr         p) { return visit (std::dynamic_pointer_cast<Container> (p)); }
	virtual bool visit (MsdosPtr        p) { return visit (std::dynamic_pointer_cast<Table>     (p)); }
	virtual bool visit (NtfsPtr         p) { return visit (std::dynamic_pointer_cast<Filesystem>(p)); }
	virtual bool visit (PartitionPtr    p) { return visit (std::dynamic_pointer_cast<Device>    (p)); }
	virtual bool visit (PiecePtr        p) { return visit (std::dynamic_pointer_cast<Container> (p)); }
	virtual bool visit (TablePtr        p) { return visit (std::dynamic_pointer_cast<Container> (p)); }
	virtual bool visit (WholePtr        p) { return visit (std::dynamic_pointer_cast<Container> (p)); }

	// Multiple inheritance
	virtual bool visit (ExtendedPtr     p) { return (visit (std::dynamic_pointer_cast<Msdos>(p)) && visit (std::dynamic_pointer_cast<Device>(p))); }
	virtual bool visit (VolumePtr       p) { return (visit (std::dynamic_pointer_cast<Whole>(p)) && visit (std::dynamic_pointer_cast<Device>(p))); }

	virtual bool visit (DevicePtr       p) {
		if ((p->is_a ("Whole")) || (p->is_a ("Table")))
			return true;
		else
			return visit (std::dynamic_pointer_cast<Container> (p));
	}
};


#endif // _VISITOR_H_

