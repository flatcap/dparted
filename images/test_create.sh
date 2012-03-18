#!/bin/bash

IMAGE_SIZE="5G"
IMAGE_DIR="."

source common.sh

##
# disk, empty
function test_10()
{
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	ok $LOOP
}

##
# disk, unknown
function test_11()
{
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	dd if=/dev/urandom bs=16K count=1 of="$LOOP" 2> /dev/null

	ok "$LOOP"
}

##
# disk, filesystem
function test_12()
{
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	mke2fs -t ext4 -q "$LOOP"
	[ $? == 0 ] || error || return

	ok "$LOOP"
}


##
# disk, msdos table, empty
function test_20()
{
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	msdos_init "$LOOP"
	[ $? == 0 ] || error || return

	ok "$LOOP"
}

##
# disk, msdos table, partition, empty
function test_21()
{
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	msdos_init "$LOOP"
	[ $? == 0 ] || error || return

	msdos_create $LOOP primary 1 500
	[ $? == 0 ] || error || return

	ok "$LOOP"
}

##
# disk, msdos table, partition, unknown
function test_22()
{
	local LOOP
	local SUB

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	msdos_init "$LOOP"
	[ $? == 0 ] || error || return

	msdos_create $LOOP primary 1 500
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 1 500)"
	[ $? == 0 ] || error || return

	dd if=/dev/urandom bs=16K count=1 of="$SUB" 2> /dev/null

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return

	ok "$LOOP"
}

##
# disk, msdos table, partition, filesystem
function test_23()
{
	local LOOP
	local SUB

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	msdos_init "$LOOP"
	[ $? == 0 ] || error || return

	msdos_create $LOOP primary 1 500
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 1 500)"
	[ $? == 0 ] || error || return
	#XXX check SUB is non-empty, and a block device too

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return

	ok "$LOOP"
}

##
# disk, msdos table, (partition, filesystem) * 4
# (include empty space)
function test_24()
{
	local LOOP
	local SUB

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	msdos_init "$LOOP"
	[ $? == 0 ] || error || return
# FS1
	msdos_create $LOOP primary 1 500
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 1 500)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return
# FS2
	msdos_create $LOOP primary 600 500
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 600 500)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return
# FS3
	msdos_create $LOOP primary 2000 1000
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 2000 1000)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return
# FS4
	msdos_create $LOOP primary 3500 200
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 3500 200)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return

	ok "$LOOP"
}


##
# disk, msdos table, extended partition, empty
function test_30()
{
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	msdos_init "$LOOP"
	[ $? == 0 ] || error || return

	msdos_create $LOOP extended 500 3500
	[ $? == 0 ] || error || return

	ok "$LOOP"
}

##
# disk, msdos table, extended partition, partition, empty
function test_31()
{
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	msdos_init "$LOOP"
	[ $? == 0 ] || error || return

	msdos_create $LOOP extended 500 3500
	[ $? == 0 ] || error || return

	msdos_create $LOOP logical 1000 1500
	[ $? == 0 ] || error || return

	ok "$LOOP"
}

##
# disk, msdos table, extended partition, partition, unknown
function test_32()
{
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	msdos_init "$LOOP"
	[ $? == 0 ] || error || return

	msdos_create $LOOP extended 500 3500
	[ $? == 0 ] || error || return

	msdos_create $LOOP logical 1000 1500
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 1000 1500)"
	[ $? == 0 ] || error || return

	dd if=/dev/urandom bs=16K count=1 of="$SUB" 2> /dev/null

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return

	ok "$LOOP"
}

##
# disk, msdos table, extended partition, partition, filesystem
function test_33()
{
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	msdos_init "$LOOP"
	[ $? == 0 ] || error || return

	msdos_create $LOOP extended 500 3500
	[ $? == 0 ] || error || return

	msdos_create $LOOP logical 1000 1500
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 1000 1500)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return

	ok "$LOOP"
}

##
# disk, msdos table, extended partition, (partition, filesystem) * 8
# (include empty space)
function test_34()
{
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	msdos_init "$LOOP"
	[ $? == 0 ] || error || return

	msdos_create $LOOP extended 500 4000
	[ $? == 0 ] || error || return
# FS1
	msdos_create $LOOP logical 600 200
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 600 200)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return
# FS2
	msdos_create $LOOP logical 1000 499
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 1000 499)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return
# FS3
	msdos_create $LOOP logical 1500 500
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 1500 500)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return
# FS4
	msdos_create $LOOP logical 2500 199
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 2500 199)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return
# FS5
	msdos_create $LOOP logical 2700 200
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 2700 200)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return
# FS6
	msdos_create $LOOP logical 3000 199
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 3000 199)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return
# FS7
	msdos_create $LOOP logical 3200 400
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 3200 400)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return
# FS8
	msdos_create $LOOP logical 3800 200
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 3800 200)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return

	ok "$LOOP"
}


##
# disk, protective table, partition, gpt table, empty
function test_40()
{
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	gpt_init "$LOOP"
	[ $? == 0 ] || error || return

	ok "$LOOP"
}

##
# disk, protective table, partition, gpt table, partition, empty
function test_41()
{
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	gpt_init "$LOOP"
	[ $? == 0 ] || error || return

	gpt_create $LOOP 1 500
	[ $? == 0 ] || error || return

	ok "$LOOP"
}

##
# disk, protective table, partition, gpt table, partition, unknown
function test_42()
{
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	gpt_init "$LOOP"
	[ $? == 0 ] || error || return

	gpt_create $LOOP 1 500
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 1 500)"
	[ $? == 0 ] || error || return

	dd if=/dev/urandom bs=16K count=1 of="$SUB" 2> /dev/null

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return

	ok "$LOOP"
}

##
# disk, protective table, partition, gpt table, partition, filesystem
function test_43()
{
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	gpt_init "$LOOP"
	[ $? == 0 ] || error || return

	gpt_create $LOOP 1 500
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 1 500)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return

	ok "$LOOP"
}

##
# disk, protective table, partition, gpt table, (partition, filesystem) * 8
# (include empty space)
function test_44()
{
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	gpt_init "$LOOP"
	[ $? == 0 ] || error || return
# FS1
	gpt_create $LOOP 600 200
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 600 200)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return
# FS2
	gpt_create $LOOP 1000 499
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 1000 499)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return
# FS3
	gpt_create $LOOP 1500 500
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 1500 500)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return
# FS4
	gpt_create $LOOP 2500 199
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 2500 199)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return
# FS5
	gpt_create $LOOP 2700 200
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 2700 200)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return
# FS6
	gpt_create $LOOP 3000 199
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 3000 199)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return
# FS7
	gpt_create $LOOP 3200 400
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 3200 400)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return
# FS8
	gpt_create $LOOP 3800 200
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 3800 200)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return

	ok "$LOOP"
}


##
# disk, lvm table, empty
function test_50()
{
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	pvcreate "$LOOP" > /dev/null 2>&1
	[ $? == 0 ] || error || return

	ok "$LOOP"
}

##
# disk, lvm table, volume group, empty
function test_51()
{
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	pvcreate "$LOOP" > /dev/null 2>&1
	[ $? == 0 ] || error || return

	vgcreate $FUNCNAME "$LOOP" > /dev/null
	[ $? == 0 ] || error || return

	ok "$LOOP"
}

##
# disk, lvm table, volume group, partition, empty
function test_52()
{
	local LOOP
	local GROUP=$FUNCNAME
	local INDEX=${FUNCNAME##*_}
	local VOLUME="simple$INDEX"

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	pvcreate "$LOOP" > /dev/null 2>&1
	[ $? == 0 ] || error || return

	vgcreate $GROUP "$LOOP" > /dev/null
	[ $? == 0 ] || error || return

	lvcreate --size 2500m --name $VOLUME $GROUP > /dev/null
	[ $? == 0 ] || error || return

	ok "$LOOP"
}

##
# disk, lvm table, volume group, partition, unknown
function test_53()
{
	local LOOP
	local GROUP=$FUNCNAME
	local INDEX=${FUNCNAME##*_}
	local VOLUME="simple$INDEX"

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	pvcreate "$LOOP" > /dev/null 2>&1
	[ $? == 0 ] || error || return

	vgcreate $GROUP "$LOOP" > /dev/null
	[ $? == 0 ] || error || return

	lvcreate --size 2500m --name $VOLUME $GROUP > /dev/null
	[ $? == 0 ] || error || return

	dd if=/dev/urandom bs=16K count=1 of="/dev/mapper/$GROUP-$VOLUME" 2> /dev/null

	ok "$LOOP"
}

##
# disk, lvm table, volume group, partition, filesystem
function test_54()
{
	local LOOP
	local GROUP=$FUNCNAME
	local INDEX=${FUNCNAME##*_}
	local VOLUME="simple$INDEX"

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	pvcreate "$LOOP" > /dev/null 2>&1
	[ $? == 0 ] || error || return

	vgcreate $GROUP "$LOOP" > /dev/null
	[ $? == 0 ] || error || return

	lvcreate --size 2500m --name $VOLUME $GROUP > /dev/null
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "/dev/mapper/$GROUP-$VOLUME"
	[ $? == 0 ] || error || return

	ok "$LOOP"
}

##
# disk, lvm table, volume group, (partition, filesystem) * 8
# (include empty space)
function test_55()
{
	local LOOP
	local GROUP=$FUNCNAME
	local INDEX=${FUNCNAME##*_}
	local VOLUME

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	pvcreate "$LOOP" > /dev/null 2>&1
	[ $? == 0 ] || error || return

	vgcreate $GROUP "$LOOP" > /dev/null
	[ $? == 0 ] || error || return
# FS1
	VOLUME="simple$INDEX.1"
	lvcreate --size 200m --name $VOLUME $GROUP > /dev/null
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "/dev/mapper/$GROUP-$VOLUME"
	[ $? == 0 ] || error || return
# GAP1
	VOLUME="gap$INDEX.1"
	lvcreate --size 200m --name $VOLUME $GROUP > /dev/null
	[ $? == 0 ] || error || return
# FS2
	VOLUME="simple$INDEX.2"
	lvcreate --size 500m --name $VOLUME $GROUP > /dev/null
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "/dev/mapper/$GROUP-$VOLUME"
	[ $? == 0 ] || error || return
# FS3
	VOLUME="simple$INDEX.3"
	lvcreate --size 500m --name $VOLUME $GROUP > /dev/null
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "/dev/mapper/$GROUP-$VOLUME"
	[ $? == 0 ] || error || return
# GAP2
	VOLUME="gap$INDEX.2"
	lvcreate --size 500m --name $VOLUME $GROUP > /dev/null
	[ $? == 0 ] || error || return
# FS4
	VOLUME="simple$INDEX.4"
	lvcreate --size 200m --name $VOLUME $GROUP > /dev/null
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "/dev/mapper/$GROUP-$VOLUME"
	[ $? == 0 ] || error || return
# FS5
	VOLUME="simple$INDEX.5"
	lvcreate --size 200m --name $VOLUME $GROUP > /dev/null
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "/dev/mapper/$GROUP-$VOLUME"
	[ $? == 0 ] || error || return
# GAP3
	VOLUME="gap$INDEX.3"
	lvcreate --size 100m --name $VOLUME $GROUP > /dev/null
	[ $? == 0 ] || error || return
# FS6
	VOLUME="simple$INDEX.6"
	lvcreate --size 200m --name $VOLUME $GROUP > /dev/null
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "/dev/mapper/$GROUP-$VOLUME"
	[ $? == 0 ] || error || return
# FS7
	VOLUME="simple$INDEX.7"
	lvcreate --size 400m --name $VOLUME $GROUP > /dev/null
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "/dev/mapper/$GROUP-$VOLUME"
	[ $? == 0 ] || error || return
# GAP4
	VOLUME="gap$INDEX.4"
	lvcreate --size 200m --name $VOLUME $GROUP > /dev/null
	[ $? == 0 ] || error || return
# FS8
	VOLUME="simple$INDEX.8"
	lvcreate --size 200m --name $VOLUME $GROUP > /dev/null
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "/dev/mapper/$GROUP-$VOLUME"
	[ $? == 0 ] || error || return

	lvremove -f /dev/mapper/$GROUP-gap* > /dev/null
	[ $? == 0 ] || error || return

	ok "$LOOP"
}


##
# disk, table, partition, lvm table, empty
function test_60()
{
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	msdos_init "$LOOP"
	[ $? == 0 ] || error || return

	msdos_create $LOOP primary 1 500
	[ $? == 0 ] || error || return

	ok "$LOOP"
}

##
# disk, table, partition, lvm table, partition, empty
function test_61()
{
	echo -n "$FUNCNAME: "
	error
}

##
# disk, table, partition, lvm table, partition, unknown
function test_62()
{
	echo -n "$FUNCNAME: "
	error
}

##
# disk, table, partition, lvm table, partition, filesystem
function test_63()
{
	echo -n "$FUNCNAME: "
	error
}

##
# disk, table, partition, lvm table, (partition, filesystem) * 8
# (include empty space)
function test_64()
{
	echo -n "$FUNCNAME: "
	error
}


cleanup

test_10
test_11
test_12
echo

test_20
test_21
test_22
test_23
test_24
echo

test_30
test_31
test_32
test_33
test_34
echo

test_40
test_41
test_42
test_43
test_44
echo

test_50
test_51
test_52
test_53
test_54
test_55
echo

