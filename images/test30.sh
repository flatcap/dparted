#!/bin/bash

IMAGE_SIZE="5G"
IMAGE_DIR="."

source common.sh

##
# 30 disk, msdos table, extended partition, empty
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
# 31 disk, msdos table, extended partition, partition, empty
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
# 32 disk, msdos table, extended partition, partition, unknown
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

	dd if=/dev/urandom bs=32K count=1 of="$SUB" 2> /dev/null

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return

	ok "$LOOP"
}

##
# 33 disk, msdos table, extended partition, partition, filesystem
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
# 34 disk, msdos table, extended partition, (partition, filesystem) * 8
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


cleanup

test_30
test_31
test_32
test_33
test_34

