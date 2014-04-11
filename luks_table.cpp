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
#include "log_trace.h"
#include "luks_partition.h"
#include "luks_table.h"
#include "partition.h"
#include "utils.h"
#include "visitor.h"

LuksTable::LuksTable (void)
{
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

LuksTable::~LuksTable()
{
}

LuksTablePtr
LuksTable::create (void)
{
	LuksTablePtr p (new LuksTable());
	p->self = p;

	return p;
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
		log_debug ("LuksTable perform: %s\n", action.name.c_str());
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

	//for (int i = 0; i < 16; ++i) {
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

	//log_debug ("header size = %ld\n", l->header_size);

	//l->device      = "/dev/mapper/luks-" + l->uuid;

	//log_debug ("Parent: %s\n", parent->get_device_name().c_str());
	l->luks_open (parent->get_device_name(), false);

	PartitionPtr p = Partition::create();
	p->sub_type ("Space");
	p->sub_type ("Reserved");

	p->bytes_size = l->header_size;
	p->bytes_used = l->header_size;
	l->add_child(p);

#if 0
	log_info ("LUKS:\n");
	log_info ("\tversion:       %u\n", l->version);
	log_info ("\tcipher name:   %s\n", l->cipher_name.c_str());
	log_info ("\tcipher mode:   %s\n", l->cipher_mode.c_str());
	log_info ("\thash spec:     %s\n", l->hash_spec.c_str());
	log_info ("\tuuid:          %s\n", l->uuid.c_str());
	log_info ("\toffset:        %u\n", l->payload_offset);
	log_info ("\tkey bits:      %u\n", l->key_bits);
	log_info ("\tMK digest:     %s\n", l->mk_digest.c_str());
	log_info ("\tMK salt:       %s\n", l->mk_digest_salt.c_str());
	log_info ("\tMK iterations: %u\n", l->mk_digest_iterations);
#endif
#if 0

	log_info ("\tactive:        %08X\n", l->pass1_active);
	log_info ("\titerations:    %u\n",   l->pass1_iterations);
	log_info ("\tsalt:          %s\n",   l->pass1_salt.c_str());
	log_info ("\tkey offset:    %u\n",   l->pass1_key_offset);
	log_info ("\tstripes:       %u\n",   l->pass1_stripes);

#endif
#if 0
	question_cb_t fn = std::bind(&LuksTable::on_reply, l, std::placeholders::_1);
	QuestionPtr q = Question::create (l, fn);
	q->title = "Enter Password";
	q->question = "for luks device " + l->device;
	q->answers = { "Cancel", "Done" };
#endif
#if 0
	PasswordDialogPtr p = PasswordDialog::create();
	p->title = "LuksTable";
	gui_app->ask_pass (p);
#endif

	//main_app->ask (q);

#if 0
	main_app->queue_add_probe(c);	//XXX do this when we've asked for a password
#endif

	parent->add_child(l);

	return true;
}

void
LuksTable::on_reply (QuestionPtr UNUSED(q))
{
	log_debug ("user has answered question\n");
}


bool
LuksTable::is_mounted (const std::string& device)
{
	std::string command = "sudo cryptsetup status " + device;
	//log_debug ("Command: %s\n", command.c_str());

	std::string output;
	int retcode = execute_command3 (command, output);
	//XXX log the output if it exists

	// Return codes:
	//	0	YES a luks device
	//	256	NO not a luks device
	//	1024	INVALID device (doesn't exist or access denied)
	return (retcode == 0);
}

bool
LuksTable::is_luks (const std::string& device)
{
	// device exists?		stat			somebody else's problem
	// is luks			cryptsetup isLuks
	// is already mounted?		cryptsetup status

	std::string command = "sudo cryptsetup isLuks " + device;
	//log_debug ("Command: %s\n", command.c_str());

	std::string output;
	int retcode = execute_command3 (command, output);
	//XXX log the output if it exists

	// Return codes:
	//	0	YES a luks device
	//	256	NO not a luks device
	//	1024	INVALID device (doesn't exist or access denied)
	return (retcode == 0);
}


bool
LuksTable::luks_open (const std::string& parent, bool UNUSED(probe))
{
//	cryptsetup open --type luks /dev/loop4p5 luks-0e6518ff-e7b5-4c84-adac-6a94d10a9116
//		/dev/mapper/luks-0e6518ff-e7b5-4c84-adac-6a94d10a9116
//
//	cryptsetup luksUUID /dev/loop4p5
//		0e6518ff-e7b5-4c84-adac-6a94d10a9116
//
//	cryptsetup isLuks /dev/loop4p4; echo $?
//		0 (is luks)
//
//	cryptsetup isLuks /dev/loop4p1; echo $?
//		1 (not luks)
//
//	cryptsetup isLuks /dev/loop4p6; echo $?
//		4 (Device /dev/loop4p6 doesn't exist or access denied.)

	if (!is_luks (parent))
		return false;

	std::string mapper = "/dev/mapper/luks-" + uuid;

	//XXX check that the luks device matches the parent device
	if (!is_mounted (mapper)) {
		std::string command = "sudo cryptsetup open --type luks " + parent + " luks-" + uuid;
		//log_debug ("Command: %s\n", command.c_str());

		std::string password = "password";
		execute_command2 (command, password);
		we_opened_this_device = true;
	}

	//XXX check mount really succeeded
	if (!is_mounted (mapper))
		return false;

	LuksPartitionPtr p = LuksPartition::create();

	p->bytes_size =  bytes_size - 2097152;	// 2MiB
	p->parent_offset = 2097152;
	p->device = mapper;

	p->get_fd();

	// move to container::find_size or utils (or block::)
	off_t size;
	size = lseek (p->fd, 0, SEEK_END);
	p->bytes_size = size;

	add_child(p);
	main_app->queue_add_probe(p);	//XXX do this when we've asked for a password

	return true;
}

bool
LuksTable::luks_close (void)
{
	//XXX close all dependents first, e.g. umount X, vgchange -an, etc
	std::string command = "cryptsetup close " + device;
	log_debug ("Command: %s\n", command.c_str());
	return false;
}


