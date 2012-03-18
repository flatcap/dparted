#!/bin/bash

IMAGE_SIZE="5G"
IMAGE_DIR="."

source common.sh

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


cleanup

test_50
test_51
test_52
test_53
test_54
test_55

