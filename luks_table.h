/* Copyright (c) 2014 Richard Russon (FlatCap) *
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

typedef std::shared_ptr<class LuksTable> LuksTablePtr;

class LuksTable : public Table
{
public:
	static LuksTablePtr create (void);
	virtual ~LuksTable();
	LuksTable& operator= (const LuksTable& c);
	LuksTable& operator= (LuksTable&& c);

	void swap (LuksTable& c);
	friend void swap (LuksTable& lhs, LuksTable& rhs);

	LuksTablePtr copy (void);

	virtual bool accept (Visitor& v);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

	static bool probe (ContainerPtr& parent, std::uint8_t* buffer, std::uint64_t bufsize);

	bool luks_open  (void);
	bool luks_close (void);

public:
	// properties
	std::string   cipher_mode;
	std::string   cipher_name;
	std::string   hash_spec;
	std::uint32_t key_bits;
	std::string   mk_digest;
	std::uint32_t mk_digest_iterations;
	std::string   mk_digest_salt;
	std::uint32_t payload_offset;
	std::uint16_t version;

	std::uint32_t pass1_active;
	std::uint32_t pass1_iterations;
	std::string   pass1_salt;
	std::uint32_t pass1_key_offset;
	std::uint32_t pass1_stripes;

	std::uint64_t header_size;

protected:
	LuksTable (void);
	LuksTable (const LuksTable& c);
	LuksTable (LuksTable&& c);

	virtual LuksTable* clone (void);

	void on_reply (QuestionPtr q);
	bool is_luks (const std::string& device);
	bool is_mounted (const std::string& device);
	bool luks_open_actual (const std::string& device, const std::string& password, bool probe);

	bool we_opened_this_device = false;
};

#endif // _LUKS_TABLE_H_

