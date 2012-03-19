#!/bin/bash

IMAGE_SIZE="5G"
IMAGE_DIR="."

source common.sh

##
# 20 disk, msdos table, empty
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
# 21 disk, msdos table, partition, empty
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
# 22 disk, msdos table, partition, unknown
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

	dd if=/dev/urandom bs=32K count=1 of="$SUB" 2> /dev/null

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return

	ok "$LOOP"
}

##
# 23 disk, msdos table, partition, filesystem
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
# 24 disk, msdos table, (partition, filesystem) * 4
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


cleanup

test_20
test_21
test_22
test_23
test_24

