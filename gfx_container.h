/* Copyright (c) 2014 Richard Russon (FlatCap)
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

#ifndef _GFX_CONTAINER_H_
#define _GFX_CONTAINER_H_

#include <vector>
#include <memory>
#include <string>

#include <gdkmm/pixbuf.h>
#include <gdkmm/rgba.h>
#include <glibmm/refptr.h>

#include "container.h"
#include "theme.h"

class GfxContainer;

typedef std::shared_ptr<GfxContainer> GfxContainerPtr;

/**
 * class GfxContainer - Shield the GUI from the messy Containers
 */
class GfxContainer
{
public:
	//XXX theme updated?
	GfxContainer (ContainerPtr c);
	virtual ~GfxContainer();

	std::vector<GfxContainerPtr> children;

	bool sync (void);
	void dump (void);

	bool set_focus (bool focus);
	void add_to_selection (void);
	void remove_from_selection (void);
	bool init (ContainerPtr c);

	bool update_info (void);
	bool mouse_event (void);

	std::string               display;
	Gdk::RGBA                 colour;
	Gdk::RGBA                 background;
	Glib::RefPtr<Gdk::Pixbuf> icon;
	std::string               label;
	std::string               label_template;

	uint64_t bytes_size = 0;
	uint64_t bytes_used = 0;
	uint64_t parent_offset = 0;

	bool usage = false;

	std::string get_tooltip (void);

protected:
	GfxContainer (void);
	ContainerPtr get_smart (void);
	std::string process_label (const std::string& label_template);

	std::weak_ptr<Container> container;

	Gdk::RGBA                 process_colour (const std::string& str);
	Glib::RefPtr<Gdk::Pixbuf> process_icon (const std::string& str);
	bool                      process_bool (const std::string& str);

	bool focussed = false;
	bool selected = false;

	int seqnum = -1;

	ThemePtr theme;
};


#endif // _GFX_CONTAINER_H_

