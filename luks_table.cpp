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
#include <functional>
#include <iomanip>
#include <sstream>
#include <string>

#include <endian.h>
#include <byteswap.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "action.h"
#include "app.h"
#include "endian.h"
#include "log.h"
#include "luks_partition.h"
#include "luks_table.h"
#include "partition.h"
#include "utils.h"
#include "visitor.h"

LuksTable::LuksTable (void)
{
	LOG_CTOR;
	const char* me = "LuksTable";

	sub_type (me);

	declare_prop_var (me, "cipher_mode",          cipher_mode,          "Cipher Mode",          0);
	declare_prop_var (me, "cipher_name",          cipher_name,          "Cipher Name",          0);
	declare_prop_var (me, "hash_spec",            hash_spec,            "Hash Spec",            0);
	declare_prop_var (me, "key_bits",             key_bits,             "Key Bits",             0);
	declare_prop_var (me, "mk_digest",            mk_digest,            "MK Digest",            0);
	declare_prop_var (me, "mk_digest_iterations", mk_digest_iterations, "MK Digest Iterations", 0);
	declare_prop_var (me, "mk_digest_salt",       mk_digest_salt,       "MK Digest Salt",       0);
	declare_prop_var (me, "payload_offset",       payload_offset,       "Payload Offset",       0);
	declare_prop_var (me, "version",              version,              "Version",              0);
}

LuksTable::LuksTable (const LuksTable& c) :
	Table(c)
{
	LuksTable();
	LOG_CTOR;
	cipher_mode          = c.cipher_mode;
	cipher_name          = c.cipher_name;
	hash_spec            = c.hash_spec;
	key_bits             = c.key_bits;
	mk_digest            = c.mk_digest;
	mk_digest_iterations = c.mk_digest_iterations;
	mk_digest_salt       = c.mk_digest_salt;
	payload_offset       = c.payload_offset;
	version              = c.version;
	pass1_active         = c.pass1_active;
	pass1_iterations     = c.pass1_iterations;
	pass1_salt           = c.pass1_salt;
	pass1_key_offset     = c.pass1_key_offset;
	pass1_stripes        = c.pass1_stripes;
	header_size          = c.header_size;
}

LuksTable::LuksTable (LuksTable&& c)
{
	LOG_CTOR;
	swap(c);
}

LuksTable::~LuksTable()
{
	LOG_DTOR;
}

LuksTablePtr
LuksTable::create (void)
{
	LuksTablePtr p (new LuksTable());
	p->self = p;

	return p;
}


LuksTable&
LuksTable::operator= (const LuksTable& c)
{
	cipher_mode          = c.cipher_mode;
	cipher_name          = c.cipher_name;
	hash_spec            = c.hash_spec;
	key_bits             = c.key_bits;
	mk_digest            = c.mk_digest;
	mk_digest_iterations = c.mk_digest_iterations;
	mk_digest_salt       = c.mk_digest_salt;
	payload_offset       = c.payload_offset;
	version              = c.version;
	pass1_active         = c.pass1_active;
	pass1_iterations     = c.pass1_iterations;
	pass1_salt           = c.pass1_salt;
	pass1_key_offset     = c.pass1_key_offset;
	pass1_stripes        = c.pass1_stripes;
	header_size          = c.header_size;

	return *this;
}

LuksTable&
LuksTable::operator= (LuksTable&& c)
{
	swap(c);
	return *this;
}


void
LuksTable::swap (LuksTable& c)
{
	std::swap (cipher_mode,          c.cipher_mode);
	std::swap (cipher_name,          c.cipher_name);
	std::swap (hash_spec,            c.hash_spec);
	std::swap (key_bits,             c.key_bits);
	std::swap (mk_digest,            c.mk_digest);
	std::swap (mk_digest_iterations, c.mk_digest_iterations);
	std::swap (mk_digest_salt,       c.mk_digest_salt);
	std::swap (payload_offset,       c.payload_offset);
	std::swap (version,              c.version);
	std::swap (pass1_active,         c.pass1_active);
	std::swap (pass1_iterations,     c.pass1_iterations);
	std::swap (pass1_salt,           c.pass1_salt);
	std::swap (pass1_key_offset,     c.pass1_key_offset);
	std::swap (pass1_stripes,        c.pass1_stripes);
	std::swap (header_size,          c.header_size);
}

void
swap (LuksTable& lhs, LuksTable& rhs)
{
	lhs.swap (rhs);
}


LuksTable*
LuksTable::clone (void)
{
	LOG_TRACE;
	return new LuksTable (*this);
}


bool
LuksTable::accept (Visitor& v)
{
	LuksTablePtr l = std::dynamic_pointer_cast<LuksTable> (get_smart());
	if (!v.visit(l))
		return false;

	return visit_children(v);
}


std::vector<Action>
LuksTable::get_actions (void)
{
	LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.luks_table", true },
	};

	std::vector<Action> parent_actions = Container::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

bool
LuksTable::perform_action (Action action)
{
	if (action.name == "dummy.luks_table") {
		std::string mapper = "/dev/mapper/luks-" + uuid;
		if (is_mounted (mapper)) {
			luks_close();
		} else {
			luks_open();
		}
		log_debug ("LuksTable perform: %s", action.name.c_str());
		return true;
	} else {
		return Container::perform_action (action);
	}
}


static std::string
read_hex (std::uint8_t* buffer, unsigned int length)
{
	return_val_if_fail (buffer, {});
	return_val_if_fail (length, {});

	std::stringstream ss;

	ss << std::setfill ('0') << std::hex << std::setiosflags (std::ios::uppercase);

	for (unsigned int i = 0; i < length; ++i) {
		ss << std::setw(2) << static_cast<unsigned> (buffer[i]) << ' ';
	}

	std::string hex = ss.str();
	hex.pop_back();
	return hex;
}

bool
LuksTable::probe (ContainerPtr& parent, std::uint8_t* buffer, std::uint64_t bufsize)
{
	return_val_if_fail (parent, false);
	return_val_if_fail (buffer, false);
	LOG_TRACE;

	if (bufsize < 1048576)		// Min size for Luks is 1MiB
		return false;

	const char* signature = "LUKS\xBA\xBE";

	if (strncmp ((const char*) buffer, signature, strlen (signature)))
		return false;

	LuksTablePtr l = create();

	l->version              = be16_to_cpup  (buffer+  6);
	l->cipher_name          = get_fixed_str (buffer+  8, 32);
	l->cipher_mode          = get_fixed_str (buffer+ 40, 32);
	l->hash_spec            = get_fixed_str (buffer+ 72, 32);
	l->payload_offset       = be32_to_cpup  (buffer+104);
	l->key_bits             = be32_to_cpup  (buffer+108) * 8;	// bytes to bits
	l->mk_digest            = read_hex      (buffer+112, 20);
	l->mk_digest_salt       = read_hex      (buffer+132, 32);
	l->mk_digest_iterations = be32_to_cpup  (buffer+164);
	l->uuid                 = get_fixed_str (buffer+168, 40);
	l->bytes_size           = parent->bytes_size;

	// for (int i = 0; i < 16; ++i) {
	l->pass1_active     = be32_to_cpup (buffer+208);	// 0x0000DEAD/0x00AC71F3
	l->pass1_iterations = be32_to_cpup (buffer+212);
	l->pass1_salt       = read_hex     (buffer+216, 32);
	l->pass1_key_offset = be32_to_cpup (buffer+248);
	l->pass1_stripes    = be32_to_cpup (buffer+252);

	// If there isn't a key to measure, then it doesn't matter,
	// we can't open the device.
	std::uint64_t encoded_key_size = l->key_bits * l->pass1_stripes;
	encoded_key_size = align (encoded_key_size, 32768);	//XXX Fairly arbitrary amount to round the padded keysizes up to 128KiB
	encoded_key_size += l->payload_offset;
	encoded_key_size = align (encoded_key_size, 1048576);
	l->header_size          = encoded_key_size;

	log_debug ("header size = %ld", l->header_size);

	// l->device      = "/dev/mapper/luks-" + l->uuid;

	log_debug ("Parent: %s", parent->get_device_name().c_str());

	log_debug ("LUKS:");
	log_debug ("\tversion:       %u", l->version);
	log_debug ("\tcipher name:   %s", l->cipher_name.c_str());
	log_debug ("\tcipher mode:   %s", l->cipher_mode.c_str());
	log_debug ("\thash spec:     %s", l->hash_spec.c_str());
	log_debug ("\tuuid:          %s", l->uuid.c_str());
	log_debug ("\toffset:        %u", l->payload_offset);
	log_debug ("\tkey bits:      %u", l->key_bits);
	log_debug ("\tMK digest:     %s", l->mk_digest.c_str());
	log_debug ("\tMK salt:       %s", l->mk_digest_salt.c_str());
	log_debug ("\tMK iterations: %u", l->mk_digest_iterations);

	log_debug ("\tactive:        %08X", l->pass1_active);
	log_debug ("\titerations:    %u",   l->pass1_iterations);
	log_debug ("\tsalt:          %s",   l->pass1_salt.c_str());
	log_debug ("\tkey offset:    %u",   l->pass1_key_offset);
	log_debug ("\tstripes:       %u",   l->pass1_stripes);

	PartitionPtr p = Partition::create();
	p->sub_type ("Space");
	p->sub_type ("Reserved");

	p->bytes_size = l->header_size;
	p->bytes_used = l->header_size;
	l->add_child (p, false);	//RAR "Marked LUKS reserved space"

	//RAR std::string desc = "Identified LUKS encrypted header";
	parent->add_child (l, false);
	l->luks_open();

	return true;
}

void
LuksTable::on_reply (QuestionPtr q)
{
	log_debug ("user has answered question: %d", q->result);
	if (q->result == -5) {	// OK
		std::string password = q->output["password"];
		std::string device = get_device_inherit();
		luks_open_actual (device, password, true);
	}
}


bool
LuksTable::is_mounted (const std::string& device)
{
	std::string command = "sudo cryptsetup status " + device;
	std::vector<std::string> output;

	int retval = execute_command_out (command, output);
	/* retval:
	 *	0 success, a luks device
	 *	1 invalid command
	 *	1 not a luks device
	 *	4 device doesn't exist
	 */

	//XXX log the output if it exists

	return (retval == 0);
}

bool
LuksTable::is_luks (const std::string& device)
{
	// device exists?		stat			somebody else's problem
	// is luks			cryptsetup isLuks
	// is already mounted?		cryptsetup status

	std::string command = "sudo cryptsetup isLuks " + device;
	std::vector<std::string> output;

	int retval = execute_command_out (command, output);
	/* retval:
	 *	0 success, a luks device
	 *	1 invalid arguments
	 *	1 failed, not a luks device
	 *	4 device doesn't exist
	 */

	//XXX log the output if it exists

	return (retval == 0);
}

bool
LuksTable::luks_open_actual (const std::string& device, const std::string& password, bool probe)
{
	std::string command = "sudo cryptsetup open --type luks " + device + " luks-" + uuid;

	int retval = execute_command_in (command, password, false);
	/* retval:
	 *	0 success, device unlocked
	 *	1 invalid arguments
	 *	2 invalid password
	 *	4 luks device doesn't exist
	 */
	if (retval == 2) {
		log_info ("invalid password for luks device %s", uuid.c_str());
		return false;
	} else if (retval != 0) {
		return false;
	}
	we_opened_this_device = true;

	LuksPartitionPtr p = LuksPartition::create();

	p->bytes_size =  bytes_size - header_size;
	p->parent_offset = header_size;
	p->device = "/dev/mapper/luks-" + uuid;

	//RAR std::string desc = "Discovered LUKS encrypted partition: " + p->device;
	add_child (p, probe);

	//XXX check mount really succeeded

	return true;
}

bool
LuksTable::luks_open (void)
{
	std::string device = get_device_inherit();
	if (!is_luks (device))
		return false;

	std::string mapper = "/dev/mapper/luks-" + uuid;

	//XXX check that the luks device matches the parent device
	if (is_mounted (mapper)) {
		LuksPartitionPtr p = LuksPartition::create();

		p->bytes_size =  bytes_size - header_size;
		p->parent_offset = header_size;
		p->device = mapper;

		//RAR std::string desc = "Unlocked LUKS encrypted parition: " + p->device;
		add_child (p, true);
	} else {
		QuestionPtr q = Question::create (std::bind(&LuksTable::on_reply, this, std::placeholders::_1));
		q->type = Question::Type::Password;
		q->input = { { "device", device } };
		main_app->ask(q);
	}

	return true;	// Not really true, we're waiting for the user
}

bool
LuksTable::luks_close (void)
{
	//XXX close all dependents first, e.g. umount X, vgchange -an, etc
	children.clear();

	std::string mapper = "/dev/mapper/luks-" + uuid;
	std::string command = "sudo cryptsetup luksClose " + mapper;
	std::vector<std::string> output;

	int retval = execute_command_out (command, output);
	/* retval:
	 *	0 success, luks device closed
	 *	1 invalid arguments
	 *	4 luks device doesn't exist
	 */

	return (retval == 0);
}


