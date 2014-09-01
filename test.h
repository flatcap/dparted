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

#ifndef _TEST_H_
#define _TEST_H_

#include <string>

#include "container.h"

void test_generate        (ContainerPtr& parent, const std::string& name);
void test_generate_add    (ContainerPtr& parent);
void test_generate_move   (ContainerPtr& parent);
void test_generate_resize (ContainerPtr& parent);

void test_execute         (ContainerPtr& child,  const std::string& name);
void test_execute_add     (ContainerPtr& child);
void test_execute_move    (ContainerPtr& child);
void test_execute_resize  (ContainerPtr& child);

#endif // _TEST_H_

