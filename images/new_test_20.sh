#!/bin/bash

IMAGE_SIZE="500M"

source common.sh

function test_20()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP_LIST=()
	INDEX=0
	IMAGE="$(create_image $FUNCNAME$INDEX)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	msdos_init "$IMAGE" 2> /dev/null
	[ $? = 0 ] || error || return

	msdos_create "$IMAGE" primary 50 400
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return

	LOOP_LIST+=($LOOP)

	ok "${LOOP_LIST[@]}"
}

function test_21()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	msdos_init "$IMAGE"
	[ $? = 0 ] || error || return

	msdos_create $IMAGE primary 50 400
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return

	mkfs.btrfs -L "btrfs_label" ${LOOP}p1 &> /dev/null
	[ $? = 0 ] || error || return

	ok "$LOOP"
}

function test_22()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	msdos_init "$IMAGE"
	[ $? = 0 ] || error || return

	msdos_create $IMAGE primary 50 400
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return

	mkfs.ext2 -q -F -L "ext2_label" ${LOOP}p1 &> /dev/null
	[ $? = 0 ] || error || return

	ok "$LOOP"
}

function test_23()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	msdos_init "$IMAGE"
	[ $? = 0 ] || error || return

	msdos_create $IMAGE primary 50 400
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return

	mkfs.ext3 -q -F -L "ext3_label" ${LOOP}p1 &> /dev/null
	[ $? = 0 ] || error || return

	ok "$LOOP"
}

function test_24()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	msdos_init "$IMAGE"
	[ $? = 0 ] || error || return

	msdos_create $IMAGE primary 50 400
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return

	mkfs.ext4 -q -F -L "ext4_label" ${LOOP}p1 &> /dev/null
	[ $? = 0 ] || error || return

	ok "$LOOP"
}

function test_25()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	msdos_init "$IMAGE"
	[ $? = 0 ] || error || return

	msdos_create $IMAGE primary 50 400
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return

	mkfs.ntfs -q -fF -L "ntfs_label" ${LOOP}p1 &> /dev/null
	[ $? = 0 ] || error || return

	ok "$LOOP"
}

function test_26()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	msdos_init "$IMAGE"
	[ $? = 0 ] || error || return

	msdos_create $IMAGE primary 50 400
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return

	mkfs.reiserfs -q -ff -l "reiserfs_label" ${LOOP}p1 &> /dev/null
	[ $? = 0 ] || error || return

	ok "$LOOP"
}

function test_27()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	msdos_init "$IMAGE"
	[ $? = 0 ] || error || return

	msdos_create $IMAGE primary 50 400
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return

	mkswap -L "swap_label" ${LOOP}p1 &> /dev/null
	[ $? = 0 ] || error || return

	ok "$LOOP"
}

function test_28()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	msdos_init "$IMAGE"
	[ $? = 0 ] || error || return

	msdos_create $IMAGE primary 50 400
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return

	mkfs.vfat -n "vfat_label" ${LOOP}p1 &> /dev/null
	[ $? = 0 ] || error || return

	ok "$LOOP"
}

function test_29()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	msdos_init "$IMAGE"
	[ $? = 0 ] || error || return

	msdos_create $IMAGE primary 50 400
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return

	mkfs.xfs -L "xfs_label" ${LOOP}p1 &> /dev/null
	[ $? = 0 ] || error || return

	ok "$LOOP"
}


if [ $# = 0 ]; then
	#cleanup
	test_20
	test_21
	test_22
	test_23
	test_24
	test_25
	test_26
	test_27
	test_28
	test_29
elif [ $# = 1 -a $1 = "-d" ]; then
	cleanup
else
	usage
fi

