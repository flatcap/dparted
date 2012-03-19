#!/bin/bash

IMAGE_SIZE="5G"
IMAGE_DIR="."

source common.sh

##
# 40 disk, protective table, partition, gpt table, empty
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
# 41 disk, protective table, partition, gpt table, partition, empty
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
# 42 disk, protective table, partition, gpt table, partition, unknown
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

	dd if=/dev/urandom bs=32K count=1 of="$SUB" 2> /dev/null

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return

	ok "$LOOP"
}

##
# 43 disk, protective table, partition, gpt table, partition, filesystem
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
# 44 disk, protective table, partition, gpt table, (partition, filesystem) * 8
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


cleanup

test_40
test_41
test_42
test_43
test_44

