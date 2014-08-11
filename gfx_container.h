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

#ifndef _GFX_CONTAINER_H_
#define _GFX_CONTAINER_H_

#include <vector>
#include <memory>
#include <string>

#include <gdkmm/pixbuf.h>
#include <gdkmm/rgba.h>
#include <glibmm/refptr.h>

#include "container.h"
#include "gfx_container_listener.h"
#include "container_listener.h"
#include "theme.h"
#include "theme_listener.h"

typedef std::shared_ptr<class GfxContainer> GfxContainerPtr;
typedef std::weak_ptr  <class GfxContainer> GfxContainerWeak;

/**
 * class GfxContainer - Shield the GUI from the messy Containers
 */
class GfxContainer :
	public IContainerListener,
	public IThemeListener
{
public:
	//XXX theme updated?
	virtual ~GfxContainer();
	static GfxContainerPtr create (GfxContainerPtr parent, ContainerPtr c);

	std::vector<GfxContainerPtr> children;
	std::mutex mutex_children;

	bool sync (void);
	void dump2 (void);

	bool set_focus (bool focus);
	bool get_focus (void);
	void add_to_selection (void);
	void remove_from_selection (void);
	bool init (ContainerPtr c);

	bool update_info (void);
	bool mouse_event (void);

	std::string               name;
	std::string               type;
	std::string               device;
	std::string               display;
	std::string               treeview;
	Gdk::RGBA                 colour;
	std::string               colour2;
	Gdk::RGBA                 background;
	Glib::RefPtr<Gdk::Pixbuf> icon;
	std::string               label;
	std::string               label_template;
	std::string               treepath;

	uint64_t bytes_size    = 0;
	uint64_t bytes_used    = 0;
	uint64_t parent_offset = 0;

	bool usage = false;

	std::string get_tooltip (void);

	GfxContainerPtr get_left (void);
	GfxContainerPtr get_right (void);
	int get_index (const GfxContainerPtr& me);
	int get_depth (void);

	ContainerListenerPtr get_listener (void);
	ContainerPtr get_container (void);
	std::string dump (void);

	void add_listener (GfxContainerListenerPtr& gcl);
	GfxContainerPtr get_parent (void);
	GfxContainerPtr get_toplevel (void);

	virtual void theme_changed (const ThemePtr& theme);

	void insert_child (GfxContainerPtr& child);

protected:
	GfxContainer (void);
	std::string process_label (const std::string& label_template);
	GfxContainerPtr get_smart (void);
	GfxContainerPtr find (const ContainerPtr& cont);

	ContainerWeak    container;
	GfxContainerWeak parent;

	Gdk::RGBA                 process_colour (const std::string& str);
	Glib::RefPtr<Gdk::Pixbuf> process_icon   (const std::string& str);
	bool                      process_bool   (const std::string& str);

	bool focussed = false;
	bool selected = false;
	bool expanded = false;

	int seqnum = -1;

	ThemePtr theme;

	friend std::ostream& operator<< (std::ostream& stream, const GfxContainerPtr& c);

private:
	GfxContainerWeak self;
	std::vector<GfxContainerListenerWeak> gfx_container_listeners;

	virtual void container_added   (const ContainerPtr& parent, const ContainerPtr& child);
	virtual void container_changed (const ContainerPtr& before, const ContainerPtr& after);
};


#endif // _GFX_CONTAINER_H_

