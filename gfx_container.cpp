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

#include "gfx_container.h"

/**
 * GfxContainer
 */
GfxContainer::GfxContainer (ContainerPtr c)
{
	theme.reset (new Theme());
	theme->read_config ("themes/default.conf");

	container = c;
	sync();
}

/**
 * ~GfxContainer
 */
GfxContainer::~GfxContainer()
{
}


/**
 * get_smart
 */
ContainerPtr
GfxContainer::get_smart (void)
{
	ContainerPtr c = container.lock();
	if (!c) {
		// Object has gone away
		children.clear();
		seqnum = -1;

		display.clear();
		colour.clear();
		background.clear();
		icon.clear();
		label.clear();
		label_template.clear();

		bytes_size = 0;
		bytes_used = 0;

		usage = false;

		focussed = false;
		selected = false;
	}

	return c;
}

/**
 * sync
 */
bool
GfxContainer::sync (void)
{
	ContainerPtr c = get_smart();
	if (!c)
		return false;

	if (seqnum == c->seqnum)
		return true;

	init(c);
	for (auto child : c->get_children()) {
		GfxContainerPtr g = std::make_shared<GfxContainer>(child);
		children.push_back(g);
	}

	return true;
}

/**
 * init
 */
bool
GfxContainer::init (ContainerPtr c)
{
	if (!c)
		return false;

	std::string path = c->get_path();
	std::string name = c->name;

	display        = theme->get_config (path, name, "display");
	colour         = theme->get_config (path, name, "colour");
	background     = theme->get_config (path, name, "background");
	label_template = theme->get_config (path, name, "label");
	icon           = theme->get_config (path, name, "icon");
	usage          = (theme->get_config (path, name, "usage") == "true");

	label = process_label (label_template);

	bytes_size = c->bytes_size;
	bytes_used = c->bytes_used;
	parent_offset = c->parent_offset;

	return true;
}

/**
 * process_label
 */
std::string
GfxContainer::process_label (std::string label_template)
{
	return label_template;
}

/**
 * update_info
 */
bool GfxContainer::update_info (void)
{
	ContainerPtr c = get_smart();
	if (!c)
		return false;

	// compare wrapper seqnums
	// compare theme seqnums

	return true;
}

/**
 * dump
 */
void
GfxContainer::dump (void)
{
	static int indent = 0;
	std::string tabs;

	if (indent > 0) {
		tabs.resize (indent, '\t');
	}

	std::cout << tabs << "----------------------" << std::endl;
	std::cout << tabs << "display        = " << display        << std::endl;
	std::cout << tabs << "colour         = " << colour         << std::endl;
	std::cout << tabs << "background     = " << background     << std::endl;
	std::cout << tabs << "icon           = " << icon           << std::endl;
	std::cout << tabs << "label          = " << label          << std::endl;
	std::cout << tabs << "label_template = " << label_template << std::endl;
	std::cout << tabs << "bytes_size     = " << bytes_size     << std::endl;
	std::cout << tabs << "bytes_used     = " << bytes_used     << std::endl;
	std::cout << tabs << "usage          = " << usage          << std::endl;
	std::cout << tabs << "seqnum         = " << seqnum         << std::endl;

	indent++;
	for (auto c : children) {
		c->dump();
	}
	indent--;
}


/**
 * set_focus
 */
bool
GfxContainer::set_focus (bool focus)
{
	return false;
}

/**
 * add_to_selection
 */
void
GfxContainer::add_to_selection (void)
{
}

/**
 * remove_from_selection
 */
void
GfxContainer::remove_from_selection (void)
{
}


