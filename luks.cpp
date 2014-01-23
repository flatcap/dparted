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
#include <functional>

#include "app.h"
#include "log.h"
#include "luks.h"
#include "log_trace.h"
#include "visitor.h"
#include "utils.h"

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

	//std::cout << "Parent: " << parent->get_device_name() << std::endl;
	//l->luks_open(parent->get_device_name(), false);

#if 1
	log_info ("LUKS:\n");
	log_info ("\tversion:     %d\n", l->version);		//XXX wrong endian (version == 1)
	log_info ("\tcipher name: %s\n", l->cipher_name.c_str());
	log_info ("\tcipher mode: %s\n", l->cipher_mode.c_str());
	log_info ("\thash spec:   %s\n", l->hash_spec.c_str());
	log_info ("\tuuid:        %s\n", l->uuid.c_str());
#endif

	question_cb_t fn = std::bind(&Luks::on_reply, l, std::placeholders::_1);
	QuestionPtr q = Question::create (l, fn);
	q->title = "Enter Password";
	q->question = "for luks device " + l->device;
	q->answers = { "Cancel", "Done" };

	main_app->ask (q);

	ContainerPtr c(l);
	//main_app->queue_add_probe(c);	//XXX do this when we've asked for a password

	return c;
}

/**
 * on_reply
 */
void
Luks::on_reply (QuestionPtr q)
{
	std::cout << "user has answered question" << std::endl;
}


/**
 * luks_open
 */
bool
Luks::luks_open (const std::string& parent, bool probe)
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

	std::string command = "cryptsetup open --type luks " + parent + " luks-" + uuid;
	std::cout << "Command: " << command << std::endl;
	return false;
}

/**
 * luks_close
 */
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
