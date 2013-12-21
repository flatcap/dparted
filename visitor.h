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

#include "pointers.h"

/**
 * class Visitor
 */
class Visitor
{
public:
	virtual bool visit_enter (ContainerPtr& c) { return true; }
	virtual bool visit_leave (void)            { return true; }

	virtual bool visit (ContainerPtr& c) = 0;

	virtual bool visit (BlockPtr&        b) { return visit ((ContainerPtr&) b); }
	virtual bool visit (DiskPtr&         d) { return visit ((ContainerPtr&) d); }
	virtual bool visit (ExtendedPtr&     e) { return visit ((ContainerPtr&) e); }
	virtual bool visit (FilePtr&         f) { return visit ((ContainerPtr&) f); }
	virtual bool visit (FilesystemPtr&   f) { return visit ((ContainerPtr&) f); }
	virtual bool visit (GptPtr&          g) { return visit ((ContainerPtr&) g); }
	virtual bool visit (LoopPtr&         l) { return visit ((ContainerPtr&) l); }
	virtual bool visit (LvmGroupPtr&     l) { return visit ((ContainerPtr&) l); }
	virtual bool visit (LvmLinearPtr&    l) { return visit ((ContainerPtr&) l); }
	virtual bool visit (LvmMetadataPtr&  l) { return visit ((ContainerPtr&) l); }
	virtual bool visit (LvmMirrorPtr&    l) { return visit ((ContainerPtr&) l); }
	virtual bool visit (LvmPartitionPtr& l) { return visit ((ContainerPtr&) l); }
	virtual bool visit (LvmRaidPtr&      l) { return visit ((ContainerPtr&) l); }
	virtual bool visit (LvmStripePtr&    l) { return visit ((ContainerPtr&) l); }
	virtual bool visit (LvmTablePtr&     l) { return visit ((ContainerPtr&) l); }
	virtual bool visit (LvmVolumePtr&    l) { return visit ((ContainerPtr&) l); }
	virtual bool visit (MdGroupPtr&      m) { return visit ((ContainerPtr&) m); }
	virtual bool visit (MdTablePtr&      m) { return visit ((ContainerPtr&) m); }
	virtual bool visit (MiscPtr&         m) { return visit ((ContainerPtr&) m); }
	virtual bool visit (MsdosPtr&        m) { return visit ((ContainerPtr&) m); }
	virtual bool visit (PartitionPtr&    p) { return visit ((ContainerPtr&) p); }
	virtual bool visit (TablePtr&        t) { return visit ((ContainerPtr&) t); }
	virtual bool visit (VolumePtr&       v) { return visit ((ContainerPtr&) v); }
	virtual bool visit (WholePtr&        w) { return visit ((ContainerPtr&) w); }
};


#endif // _VISITOR_H_

