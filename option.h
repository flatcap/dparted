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

#ifndef _OPTION_H_
#define _OPTION_H_

#include <string>
#include <memory>

typedef std::shared_ptr<class Container> ContainerPtr;

typedef struct _option
{
	enum class Type {
		radio_button,
		checkbox,
		label,
		text_1,
		text_n,
		password
	} type;

	std::string name;
	std::string description;
	std::string notes;
	std::string value;

	ContainerPtr object;

	bool read_only;
	bool advanced;

	int min_size;
	int max_size;
	int width;
	int height;
} Option;


#endif // _OPTION_H_

