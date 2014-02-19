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

#include <sstream>
#include <string>
#include <cstring>
#include <functional>

#include "app.h"
#include "log.h"
#include "luks.h"
#include "log_trace.h"
#include "visitor.h"
#include "utils.h"

Luks::Luks (void)
{
	const char* me = "Luks";

	sub_type (me);

	declare_prop (me, "version",     version,     "desc of version");
	declare_prop (me, "cipher_name", cipher_name, "desc of cipher_name");
	declare_prop (me, "cipher_mode", cipher_mode, "desc of cipher_mode");
	declare_prop (me, "hash_spec",   hash_spec,   "desc of hash_spec");
}

LuksPtr
Luks::create (void)
{
	LuksPtr l (new Luks());
	l->weak = l;

	return l;
}


bool
Luks::accept (Visitor& v)
{
	LuksPtr l = std::dynamic_pointer_cast<Luks> (get_smart());
	if (!v.visit(l))
		return false;
	return visit_children(v);
}

ContainerPtr
Luks::probe (ContainerPtr& UNUSED(top_level), ContainerPtr& parent, unsigned char* buffer, int UNUSED(bufsize))
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

	//std::cout << "Parent: " << parent->get_device_name() << std::endl;
	l->luks_open(parent->get_device_name(), false);

#if 0
	log_info ("LUKS:\n");
	log_info ("\tversion:     %d\n", l->version);		//XXX wrong endian (version == 1)
	log_info ("\tcipher name: %s\n", l->cipher_name.c_str());
	log_info ("\tcipher mode: %s\n", l->cipher_mode.c_str());
	log_info ("\thash spec:   %s\n", l->hash_spec.c_str());
	log_info ("\tuuid:        %s\n", l->uuid.c_str());
#endif

#if 0
	question_cb_t fn = std::bind(&Luks::on_reply, l, std::placeholders::_1);
	QuestionPtr q = Question::create (l, fn);
	q->title = "Enter Password";
	q->question = "for luks device " + l->device;
	q->answers = { "Cancel", "Done" };
#endif
#if 0
	PasswordDialogPtr p = PasswordDialog::create();
	p->title = "Luks";
	gui_app->ask_pass (p);
#endif

	//main_app->ask (q);

	ContainerPtr c(l);
	main_app->queue_add_probe(c);	//XXX do this when we've asked for a password

	return c;
}

void
Luks::on_reply (QuestionPtr UNUSED(q))
{
	std::cout << "user has answered question" << std::endl;
}


bool
Luks::is_mounted (const std::string& device)
{
	std::string command = "sudo cryptsetup status " + device;
	//std::cout << "Command: " << command << std::endl;

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
Luks::is_luks (const std::string& device)
{
	// device exists?		stat			somebody else's problem
	// is luks			cryptsetup isLuks
	// is already mounted?		cryptsetup status

	std::string command = "sudo cryptsetup isLuks " + device;
	//std::cout << "Command: " << command << std::endl;

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
Luks::luks_open (const std::string& parent, bool UNUSED(probe))
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
	if (is_mounted (mapper))
		return true;

	std::string command = "sudo cryptsetup open --type luks " + parent + " luks-" + uuid;
	//std::cout << "Command: " << command << std::endl;

	std::string password = "password";
	execute_command2 (command, password);
	we_opened_this_device = true;
	return false;
}

bool
Luks::luks_close (void)
{
	//XXX close all dependents first, e.g. umount X, vgchange -an, etc
	std::string command = "cryptsetup close " + device;
	std::cout << "Command: " << command << std::endl;
	return false;
}


#ifdef RAR
cryptsetup status /dev/mapper/luks-0e6518ff-e7b5-4c84-adac-6a94d10a9116
	/dev/mapper/luks-0e6518ff-e7b5-4c84-adac-6a94d10a9116 is active and is in use.
	  type:    LUKS1
	  cipher:  aes-xts-plain64
	  keysize: 512 bits
	  device:  /dev/loop4p5
	  offset:  4096 sectors
	  size:    33046528 sectors
	  mode:    read/write

cryptsetup status /dev/mapper/luks-0e6518ff-e7b5-4c84-adac-6a94d10a9116
	/dev/mapper/luks-0e6518ff-e7b5-4c84-adac-6a94d10a9116 is active.
	  type:    LUKS1
	  cipher:  aes-xts-plain64
	  keysize: 512 bits
	  device:  /dev/loop4p5
	  offset:  4096 sectors
	  size:    33046528 sectors
	  mode:    read/write

create
isLuks
luksAddKey
luksClose
luksDelKey
luksDump
luksFormat
luksHeaderBackup
luksHeaderRestore
luksKillSlot
luksOpen
luksRemoveKey
luksResume
luksSuspend
luksUUID
remove
resize
status

cryptsetup luksDump /dev/sda2
LUKS header information for /dev/sda2

Version:        1
Cipher name:    aes
Cipher mode:    xts-plain64
Hash spec:      sha1
Payload offset: 4096
MK bits:        512
MK digest:      df 25 17 cb 40 5b ac 5b ce 88 94 5e 82 92 9f fa 70 42 a6 0c
MK salt:        c7 b7 61 29 5a 9c a9 88 cf 7b 9a 40 74 9c a0 5f
                fd 7c 3d 83 b7 86 da af 93 32 3c 2c ae d2 17 73
MK iterations:  52250
UUID:           bf56d741-efe5-4931-8f4f-91aa934caf4a

Key Slot 0: ENABLED
        Iterations:             209835
        Salt:                   a2 37 d4 0e ad 47 c7 b2 04 70 d9 df 34 43 a0 9a
                                2e 58 33 98 44 e6 bb 97 b5 24 26 3f bf 1a ab 94
        Key material offset:    8
        AF stripes:             4000
Key Slot 1: DISABLED
Key Slot 2: DISABLED
Key Slot 3: DISABLED
Key Slot 4: DISABLED
Key Slot 5: DISABLED
Key Slot 6: DISABLED
Key Slot 7: DISABLED
#endif

std::vector<Action>
Luks::get_actions (void)
{
	// LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.luks", true },
	};

	std::vector<Action> parent_actions = Device::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

bool
Luks::perform_action (Action action)
{
	if (action.name == "dummy.luks") {
		std::cout << "Luks perform: " << action.name << std::endl;
		return true;
	} else {
		return Device::perform_action (action);
	}
}


