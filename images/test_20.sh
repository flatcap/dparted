#!/bin/bash

IMAGE_SIZE="5G"

source common.sh

##
# 20 disk, msdos table, empty
function test_20()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	msdos_init "$IMAGE"
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return

	ok "$LOOP"
}

##
# 21 disk, msdos table, partition, empty
function test_21()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	msdos_init "$IMAGE"
	[ $? = 0 ] || error || return

	msdos_create $IMAGE primary 1 1023	# partition 1
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return

	ok "$LOOP"
}

##
# 22 disk, msdos table, partition, unknown
function test_22()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	msdos_init "$IMAGE"
	[ $? = 0 ] || error || return

	msdos_create $IMAGE primary 1 1023	# partition 1
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return

	dd if=/dev/urandom bs=32K count=1 of="${LOOP}p1" 2> /dev/null

	ok "$LOOP"
}

##
# 23 disk, msdos table, partition, filesystem
function test_23()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	msdos_init "$IMAGE"
	[ $? = 0 ] || error || return

	msdos_create $IMAGE primary 1 1023	# partition 1
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return

	mke2fs -t ext4 -q "${LOOP}p1"		# filesystem 1
	[ $? = 0 ] || error || return

	ok "$LOOP"
}

##
# 24 disk, msdos table, (partition, filesystem) * 4
# (include empty space)
function test_24()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	msdos_init "$IMAGE"
	[ $? = 0 ] || error || return

	msdos_create $IMAGE primary 1 1023	# partition 1
	[ $? = 0 ] || error || return

	msdos_create $IMAGE primary 1024 512	# partition 2
	[ $? = 0 ] || error || return

	msdos_create $IMAGE primary 2048 1024	# partition 3
	[ $? = 0 ] || error || return

	msdos_create $IMAGE primary 3584 512	# partition 4
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return
	[ -b "${LOOP}p2" ] || error || return
	[ -b "${LOOP}p3" ] || error || return
	[ -b "${LOOP}p4" ] || error || return

	mke2fs -t ext4 -q "${LOOP}p1"		# filesystem 1
	[ $? = 0 ] || error || return

	mke2fs -t ext4 -q "${LOOP}p2"		# filesystem 2
	[ $? = 0 ] || error || return

	mke2fs -t ext4 -q "${LOOP}p3"		# filesystem 3
	[ $? = 0 ] || error || return

	mke2fs -t ext4 -q "${LOOP}p4"		# filesystem 4
	[ $? = 0 ] || error || return

	ok "$LOOP"
}


if [ $# = 0 ]; then
	cleanup
	test_20
	test_21
	test_22
	test_23
	test_24
elif [ $# = 1 -a $1 = "-d" ]; then
	cleanup
else
	usage
fi

