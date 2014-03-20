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

#ifndef _LUKS_TABLE_H_
#define _LUKS_TABLE_H_

#include <memory>
#include <string>
#include <vector>

#include "table.h"
#include "question.h"

class LuksTable;

typedef std::shared_ptr<LuksTable> LuksTablePtr;

class LuksTable : public Table
{
public:
	static LuksTablePtr create (void);
	virtual ~LuksTable();
	virtual bool accept (Visitor& v);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

	static bool probe (ContainerPtr& parent, std::uint8_t* buffer, std::uint64_t bufsize);

	bool luks_open (const std::string& parent, bool probe);
	bool luks_close (void);

public:
	//properties
	std::string cipher_mode;
	std::string cipher_name;
	std::string hash_spec;
	std::uint16_t version;

protected:
	LuksTable (void);

	void on_reply (QuestionPtr q);
	bool is_luks (const std::string& device);
	bool is_mounted (const std::string& device);

	bool we_opened_this_device = false;

private:

};

#endif // _LUKS_TABLE_H_

