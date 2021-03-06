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

#ifndef _DOT_VISITOR_H_
#define _DOT_VISITOR_H_

#include <sstream>
#include <stack>
#include <string>

#include "visitor.h"

class DotVisitor : public Visitor
{
public:
	DotVisitor (void);
	virtual ~DotVisitor();

	virtual bool visit_enter (ContainerPtr& c);
	virtual bool visit_leave (void);

	virtual bool visit (ContainerPtr  p);
	virtual bool visit (FilesystemPtr p);
#ifdef DP_GPT
	virtual bool visit (GptPtr        p);
#endif
#ifdef DP_LOOP
	virtual bool visit (LoopPtr       p);
#endif
	virtual bool visit (MiscPtr       p);
#ifdef DP_MSDOS
	virtual bool visit (MsdosPtr      p);
	virtual bool visit (ExtendedPtr   p);
#endif
	virtual bool visit (PartitionPtr  p);

	virtual bool visit (WholePtr      p);
#ifdef DP_LVM
	virtual bool visit (LvmVolumePtr  p);
	virtual bool visit (LvmRaidPtr    p);
	virtual bool visit (LvmGroupPtr   p);
	virtual bool visit (LvmLinearPtr  p);
#endif

	template <class T>
	std::string parent_link (std::shared_ptr<T> t);

	std::string get_dot (void);
	void run_dotty (void);

	bool display  = false;
	int  resize   = -1;
	bool save_gv  = false;
	bool save_png = false;

protected:
	std::stack<ContainerPtr> parents;
	std::stringstream dot_objects;
	std::stringstream dot_links;
};

// Simple wrapper
template<class T>
void run_dotty (std::shared_ptr<T>& top)
{
	ContainerPtr c (top);
	if (!c)
		return;

	DotVisitor dv;
	dv.resize = 60;
	c->accept (dv);
	dv.run_dotty();
}

#endif // _DOT_VISITOR_H_

