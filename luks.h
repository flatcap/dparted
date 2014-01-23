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

#ifndef _LUKS_H_
#define _LUKS_H_

#include <string>
#include <memory>

#include "partition.h"
#include "question.h"

class Luks;

typedef std::shared_ptr<Luks> LuksPtr;

/**
 * class Luks
 */
class Luks : public Partition
{
public:
	virtual ~Luks() = default;
	static LuksPtr create (void);
	virtual bool accept (Visitor& v);

	static ContainerPtr probe (ContainerPtr& top_level, ContainerPtr& parent, unsigned char* buffer, int bufsize);

	bool luks_open (const std::string& parent, bool probe);
	bool luks_close (void);

public:
	int         version;
	std::string cipher_name;
	std::string cipher_mode;
	std::string hash_spec;

protected:
	Luks (void);

	void on_reply (QuestionPtr q);

	bool we_opened_this_device = false;

private:

};


#endif // _LUKS_H_

