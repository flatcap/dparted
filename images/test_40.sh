#!/bin/bash

IMAGE_SIZE="5G"

source common.sh

##
# 40 disk, protective table, partition, gpt table, empty
function test_40()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	gpt_init "$IMAGE"
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return

	ok "$LOOP"
}

##
# 41 disk, protective table, partition, gpt table, partition, empty
function test_41()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	gpt_init "$IMAGE"
	[ $? = 0 ] || error || return

	gpt_create $IMAGE 1 1023		# partition 1
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return

	ok "$LOOP"
}

##
# 42 disk, protective table, partition, gpt table, partition, unknown
function test_42()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	gpt_init "$IMAGE"
	[ $? = 0 ] || error || return

	gpt_create $IMAGE 1 1023		# partition 1
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return

	dd if=/dev/urandom bs=32K count=1 of="${LOOP}p1" 2> /dev/null

	ok "$LOOP"
}

##
# 43 disk, protective table, partition, gpt table, partition, filesystem
function test_43()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	gpt_init "$IMAGE"
	[ $? = 0 ] || error || return

	gpt_create $IMAGE 1024 1023		# partition 1
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return

	mke2fs -t ext4 -q "${LOOP}p1"		# filesystem 1
	[ $? = 0 ] || error || return

	ok "$LOOP"
}

##
# 44 disk, protective table, partition, gpt table, (partition, filesystem) * 8
# (include empty space)
function test_44()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	gpt_init "$IMAGE"
	[ $? = 0 ] || error || return

	gpt_create $IMAGE 1 1023		# partition 1
	[ $? = 0 ] || error || return

	gpt_create $IMAGE 1200 512		# partition 2
	[ $? = 0 ] || error || return

	gpt_create $IMAGE 1800 512		# partition 3
	[ $? = 0 ] || error || return

	gpt_create $IMAGE 2500 768		# partition 4
	[ $? = 0 ] || error || return

	gpt_create $IMAGE 3300 400		# partition 5
	[ $? = 0 ] || error || return

	gpt_create $IMAGE 3900 199		# partition 6
	[ $? = 0 ] || error || return

	gpt_create $IMAGE 4300 150		# partition 7
	[ $? = 0 ] || error || return

	gpt_create $IMAGE 4500 400		# partition 8
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return
	[ -b "${LOOP}p2" ] || error || return
	[ -b "${LOOP}p3" ] || error || return
	[ -b "${LOOP}p4" ] || error || return
	[ -b "${LOOP}p5" ] || error || return
	[ -b "${LOOP}p6" ] || error || return
	[ -b "${LOOP}p7" ] || error || return
	[ -b "${LOOP}p8" ] || error || return

	mke2fs -t ext4 -q "${LOOP}p1"		# filesystem 1
	[ $? = 0 ] || error || return

	mke2fs -t ext4 -q "${LOOP}p2"		# filesystem 2
	[ $? = 0 ] || error || return

	mke2fs -t ext4 -q "${LOOP}p3"		# filesystem 3
	[ $? = 0 ] || error || return

	mke2fs -t ext4 -q "${LOOP}p4"		# filesystem 4
	[ $? = 0 ] || error || return

	mke2fs -t ext4 -q "${LOOP}p5"		# filesystem 5
	[ $? = 0 ] || error || return

	mke2fs -t ext4 -q "${LOOP}p6"		# filesystem 6
	[ $? = 0 ] || error || return

	mke2fs -t ext4 -q "${LOOP}p7"		# filesystem 7
	[ $? = 0 ] || error || return

	mke2fs -t ext4 -q "${LOOP}p8"		# filesystem 8
	[ $? = 0 ] || error || return

	ok "$LOOP"
}


if [ $# = 0 ]; then
	cleanup
	test_40
	test_41
	test_42
	test_43
	test_44
elif [ $# = 1 -a $1 = "-d" ]; then
	cleanup
else
	usage
fi

