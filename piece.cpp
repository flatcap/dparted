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

#include <cstring>
#include <sstream>

#include "log.h"
#include "main.h"
#include "piece.h"
#include "luks.h"
#include "utils.h"
#include "log_trace.h"
#include "visitor.h"

Piece::Piece (void)
{
	const char* me = "Piece";

	sub_type (me);
}

PiecePtr
Piece::create (void)
{
	PiecePtr m (new Piece());
	m->weak = m;

	return m;
}


bool
Piece::accept (Visitor& v)
{
	PiecePtr m = std::dynamic_pointer_cast<Piece> (get_smart());
	if (!v.visit(m))
		return false;
	return visit_children(v);
}


std::vector<Action>
Piece::get_actions (void)
{
	// LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.piece", true },
	};

	std::vector<Action> parent_actions = Container::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

bool
Piece::perform_action (Action action)
{
	if (action.name == "dummy.piece") {
		std::cout << "Piece perform: " << action.name << std::endl;
		return true;
	} else {
		return Container::perform_action (action);
	}
}


