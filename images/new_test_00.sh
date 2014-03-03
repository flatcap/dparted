#!/bin/bash

IMAGE_SIZE="500M"

source common.sh

##
# 03 - Filesystem
# 	03.01 Block.Loop -> Filesystem.Btrfs
# 	03.02 Block.Loop -> Filesystem.Extfs.Ext2
# 	03.03 Block.Loop -> Filesystem.Extfs.Ext3
# 	03.04 Block.Loop -> Filesystem.Extfs.Ext4
# 	03.05 Block.Loop -> Filesystem.Ntfs
# 	03.06 Block.Loop -> Filesystem.Reiserfs
# 	03.07 Block.Loop -> Filesystem.Swap
# 	03.08 Block.Loop -> Filesystem.Vfat
# 	03.09 Block.Loop -> Filesystem.Xfs
function test_00()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	COMMAND_LIST=(
		'mkfs.btrfs           -L "btrfs_label"'
		'mkfs.ext2     -q -F  -L "ext2_label"'
		'mkfs.ext3     -q -F  -L "ext3_label"'
		'mkfs.ext4     -q -F  -L "ext4_label"'
		'mkfs.ntfs     -q -fF -L "ntfs_label"'
		'mkfs.reiserfs -q -ff -l "reiserfs_label"'
		'mkswap               -L "swap_label"'
		'mkfs.vfat            -n "vfat_label"'
		'mkfs.xfs             -L "xfs_label"'
	)

	LOOP_LIST=()
	INDEX=0
	for COMMAND in "${COMMAND_LIST[@]}"; do
		IMAGE="$(create_image $FUNCNAME$INDEX)"
		[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

		LOOP="$(create_loop $IMAGE)"
		[ -n "$LOOP" -a -b "$LOOP" ] || error || return
		LOOP_LIST+=($LOOP)

		$COMMAND ${LOOP} &> /dev/null
		[ $? = 0 ] || error || return

		: $((INDEX++))
	done

	ok "${LOOP_LIST[@]}"
}

##
# 03 - Misc
# 	03.10 Block.Loop -> Misc.Random
# 	03.11 Block.Loop -> Misc.Zero
function test_01()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP_LIST=()
	INDEX=0
	IMAGE="$(create_image $FUNCNAME$INDEX)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	LOOP_LIST+=($LOOP)

	# Do nothing to the first loop

	: $((INDEX++))
	IMAGE="$(create_image $FUNCNAME$INDEX)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	LOOP_LIST+=($LOOP)

	# Random data in the second loop
	dd if=/dev/urandom bs=1M count=1 of=$LOOP &> /dev/null
	[ $? = 0 ] || error || return

	ok "${LOOP_LIST[@]}"
}

##
# 03 - Table
# 	03.12 Block.Loop -> Table.Gpt
# 	03.15 Block.Loop -> Table.Msdos
function test_02()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP_LIST=()
	INDEX=0
	IMAGE="$(create_image $FUNCNAME$INDEX)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	LOOP_LIST+=($LOOP)

	gpt_init "$LOOP" 2> /dev/null
	[ $? = 0 ] || error || return

	: $((INDEX++))
	IMAGE="$(create_image $FUNCNAME$INDEX)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	LOOP_LIST+=($LOOP)

	msdos_init "$LOOP" 2> /dev/null
	[ $? = 0 ] || error || return

	ok "${LOOP_LIST[@]}"
}

##
# 03 - Table
# 	03.13 Block.Loop -> Table.LvmTable
function test_03()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP_LIST=()
	INDEX=0
	IMAGE="$(create_image $FUNCNAME$INDEX)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	LOOP_LIST+=($LOOP)

	pvcreate "$LOOP" > /dev/null
	[ $? = 0 ] || error || return

	ok "${LOOP_LIST[@]}"
}

##
# 03 - Table
# 	03.14 Block.Loop -> Table.MdTable
function test_04()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP_LIST=()
	IMAGE="$(create_image $FUNCNAME$INDEX)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	LOOP_LIST+=($LOOP)

	mdadm -C --level=linear -n1 --force /dev/md0 "$LOOP" &> /dev/null
	[ $? = 0 ] || error || return

	ok "${LOOP_LIST[@]}"
}

##
# 03 - Wrapper.Luks
# 	03.16 Block.Loop -> Wrapper.Luks
function test_05()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP_LIST=()
	INDEX=0
	IMAGE="$(create_image $FUNCNAME$INDEX)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	LOOP_LIST+=($LOOP)

	echo password | cryptsetup luksFormat --batch-mode --force-password "$LOOP"
	[ $? = 0 ] || error || return

	ok "${LOOP_LIST[@]}"
}


if [ $# = 0 ]; then
	#cleanup
	test_00
	test_01
	test_02
	test_03
	test_04
	test_05
elif [ $# = 1 -a $1 = "-d" ]; then
	cleanup
else
	usage
fi

