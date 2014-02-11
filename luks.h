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
	bool is_luks (const std::string& device);
	bool is_mounted (const std::string& device);

	bool we_opened_this_device = false;

private:

};


#endif // _LUKS_H_

