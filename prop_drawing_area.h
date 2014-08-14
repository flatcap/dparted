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

#ifndef _PROP_DRAWING_AREA_H_
#define _PROP_DRAWING_AREA_H_

#include <cairomm/cairomm.h>

#include "base_drawing_area.h"

class PropDrawingArea : public BaseDrawingArea
{
public:
	PropDrawingArea (void);
	virtual ~PropDrawingArea();

	virtual void set_data (GfxContainerPtr& c);

	virtual void theme_changed (const ThemePtr& theme);

protected:
	virtual bool on_draw (const Cairo::RefPtr<Cairo::Context>& cr);
	void draw_container (const Cairo::RefPtr<Cairo::Context>& cr, GfxContainerPtr& cont, Rect shape);

	virtual void gfx_container_added   (const GfxContainerPtr& parent, const GfxContainerPtr& child);
	virtual void gfx_container_changed (const GfxContainerPtr& before, const GfxContainerPtr& after);
	virtual void gfx_container_deleted (const GfxContainerPtr& parent, const GfxContainerPtr& child);
};

#endif // _PROP_DRAWING_AREA_H_

