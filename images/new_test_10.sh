#!/bin/bash

IMAGE_SIZE="500M"

source common.sh

##
# 08 - Table.Gpt
# 	08.01 Table.Gpt -> Partition.GptPartition
# 	08.03 Table.Gpt -> Partition.Space.Reserved
# 	08.04 Table.Gpt -> Partition.Space.Unallocated
function test_10()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP_LIST=()
	INDEX=0
	IMAGE="$(create_image $FUNCNAME$INDEX)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	gpt_init "$IMAGE" 2> /dev/null
	[ $? = 0 ] || error || return

	gpt_create "$IMAGE" 50 400
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return

	LOOP_LIST+=($LOOP)

	ok "${LOOP_LIST[@]}"
}

##
# 05 - Filesystem
# 	05.01 Partition.GptPartition -> Filesystem.Btrfs
function test_11()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	gpt_init "$IMAGE"
	[ $? = 0 ] || error || return

	gpt_create $IMAGE 50 400
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return

	mkfs.btrfs -L "btrfs_label" ${LOOP}p1 &> /dev/null
	[ $? = 0 ] || error || return

	ok "$LOOP"
}

##
# 05 - Filesystem
# 	05.02 Partition.GptPartition -> Filesystem.Extfs.Ext2
function test_12()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	gpt_init "$IMAGE"
	[ $? = 0 ] || error || return

	gpt_create $IMAGE 50 400
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return

	mkfs.ext2 -q -F -L "ext2_label" ${LOOP}p1 &> /dev/null
	[ $? = 0 ] || error || return

	ok "$LOOP"
}

##
# 05 - Filesystem
# 	05.03 Partition.GptPartition -> Filesystem.Extfs.Ext3
function test_13()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	gpt_init "$IMAGE"
	[ $? = 0 ] || error || return

	gpt_create $IMAGE 50 400
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return

	mkfs.ext3 -q -F -L "ext3_label" ${LOOP}p1 &> /dev/null
	[ $? = 0 ] || error || return

	ok "$LOOP"
}

##
# 05 - Filesystem
# 	05.04 Partition.GptPartition -> Filesystem.Extfs.Ext4
function test_14()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	gpt_init "$IMAGE"
	[ $? = 0 ] || error || return

	gpt_create $IMAGE 50 400
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return

	mkfs.ext4 -q -F -L "ext4_label" ${LOOP}p1 &> /dev/null
	[ $? = 0 ] || error || return

	ok "$LOOP"
}

##
# 05 - Filesystem
# 	05.05 Partition.GptPartition -> Filesystem.Ntfs
function test_15()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	gpt_init "$IMAGE"
	[ $? = 0 ] || error || return

	gpt_create $IMAGE 50 400
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return

	mkfs.ntfs -q -fF -L "ntfs_label" ${LOOP}p1 &> /dev/null
	[ $? = 0 ] || error || return

	ok "$LOOP"
}

##
# 05 - Filesystem
# 	05.06 Partition.GptPartition -> Filesystem.Reiserfs
function test_16()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	gpt_init "$IMAGE"
	[ $? = 0 ] || error || return

	gpt_create $IMAGE 50 400
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return

	mkfs.reiserfs -q -ff -l "reiserfs_label" ${LOOP}p1 &> /dev/null
	[ $? = 0 ] || error || return

	ok "$LOOP"
}

##
# 05 - Filesystem
# 	05.07 Partition.GptPartition -> Filesystem.Swap
function test_17()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	gpt_init "$IMAGE"
	[ $? = 0 ] || error || return

	gpt_create $IMAGE 50 400
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return

	mkswap -L "swap_label" ${LOOP}p1 &> /dev/null
	[ $? = 0 ] || error || return

	ok "$LOOP"
}

##
# 05 - Filesystem
# 	05.08 Partition.GptPartition -> Filesystem.Vfat
function test_18()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	gpt_init "$IMAGE"
	[ $? = 0 ] || error || return

	gpt_create $IMAGE 50 400
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return

	mkfs.vfat -n "vfat_label" ${LOOP}p1 &> /dev/null
	[ $? = 0 ] || error || return

	ok "$LOOP"
}

##
# 05 - Filesystem
# 	05.09 Partition.GptPartition -> Filesystem.Xfs
function test_19()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	gpt_init "$IMAGE"
	[ $? = 0 ] || error || return

	gpt_create $IMAGE 50 400
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return

	mkfs.xfs -L "xfs_label" ${LOOP}p1 &> /dev/null
	[ $? = 0 ] || error || return

	ok "$LOOP"
}


# 05.10 Partition.GptPartition -> Misc.Random
# 05.11 Partition.GptPartition -> Misc.Zero
# 05.12 Partition.GptPartition -> Wrapper.Luks

if [ $# = 0 ]; then
	#cleanup
	test_10
	test_11
	test_12
	test_13
	test_14
	test_15
	test_16
	test_17
	test_18
	test_19
elif [ $# = 1 -a $1 = "-d" ]; then
	cleanup
else
	usage
fi

