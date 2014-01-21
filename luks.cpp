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

#include <sstream>
#include <string>
#include <cstring>

#include "app.h"
#include "log.h"
#include "luks.h"
#include "log_trace.h"
#include "visitor.h"

/**
 * Luks
 */
Luks::Luks (void)
{
	const char* me = "Luks";

	declare (me);

	declare_prop (me, "version",     ptype, "desc of version");
	declare_prop (me, "cipher_name", ptype, "desc of cipher_name");
	declare_prop (me, "cipher_mode", ptype, "desc of cipher_mode");
	declare_prop (me, "hash_spec",   ptype, "desc of hash_spec");
}

/**
 * create
 */
LuksPtr
Luks::create (void)
{
	LuksPtr l (new Luks());
	l->weak = l;

	return l;
}


/**
 * accept
 */
bool
Luks::accept (Visitor& v)
{
	LuksPtr l = std::dynamic_pointer_cast<Luks> (get_smart());
	if (!v.visit(l))
		return false;
	return visit_children(v);
}

/**
 * probe
 */
ContainerPtr
Luks::probe (ContainerPtr& top_level, ContainerPtr& parent, unsigned char* buffer, int bufsize)
{
	const char* signature = "LUKS\xBA\xBE";

	if (strncmp ((const char*) buffer, signature, strlen (signature)))
		return nullptr;

	LuksPtr l = create();

	l->version     = *(short int*) (buffer+6);
	l->cipher_name = (char*) (buffer+8);
	l->cipher_mode = (char*) (buffer+40);
	l->hash_spec   = (char*) (buffer+72);
	l->uuid        = (char*) (buffer+168);

	l->device      = "/dev/mapper/luks-" + l->uuid;

#if 0
	log_info ("LUKS:\n");
	log_info ("\tversion:     %d\n", l->version);
	log_info ("\tcipher name: %s\n", l->cipher_name.c_str());
	log_info ("\tcipher mode: %s\n", l->cipher_mode.c_str());
	log_info ("\thash spec:   %s\n", l->hash_spec.c_str());
	log_info ("\tuuid:        %s\n", l->uuid.c_str());
#endif

	ContainerPtr c(l);
	main_app->queue_add_probe(c);
	return c;
}

