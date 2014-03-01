#!/bin/bash

IMAGE_SIZE="500M"

source common.sh

function test_30()
{
	local IMAGE
	local LOOP
	local PASSWORD="password"

	echo -n "$FUNCNAME: "

	LOOP_LIST=()
	INDEX=0
	IMAGE="$(create_image $FUNCNAME$INDEX)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	LOOP_LIST+=($LOOP)

	echo "$PASSWORD" | cryptsetup luksFormat --batch-mode --force-password "$LOOP"
	[ $? = 0 ] || error || return

	UUID=$(cryptsetup luksUUID "$LOOP") 2> /dev/null
	[ $? = 0 ]     || error || return
	[ -n "$UUID" ] || error || return

	DEVICE="/dev/mapper/luks-$UUID"
	echo "$PASSWORD" | cryptsetup open --type luks "$LOOP" luks-$UUID 2> /dev/null
	[ $? = 0 ] || error || return
	[ -b "$DEVICE" ] || error || return

	mkfs.btrfs -L "btrfs_label" "$DEVICE" &> /dev/null
	[ $? = 0 ] || error || return

	cryptsetup luksClose luks-$UUID 2> /dev/null
	[ $? = 0 ] || error || return

	ok "${LOOP_LIST[@]}"
}

function test_31()
{
	local IMAGE
	local LOOP
	local PASSWORD="password"

	echo -n "$FUNCNAME: "

	LOOP_LIST=()
	INDEX=0
	IMAGE="$(create_image $FUNCNAME$INDEX)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	LOOP_LIST+=($LOOP)

	echo "$PASSWORD" | cryptsetup luksFormat --batch-mode --force-password "$LOOP"
	[ $? = 0 ] || error || return

	UUID=$(cryptsetup luksUUID "$LOOP") 2> /dev/null
	[ $? = 0 ]     || error || return
	[ -n "$UUID" ] || error || return

	DEVICE="/dev/mapper/luks-$UUID"
	echo "$PASSWORD" | cryptsetup open --type luks "$LOOP" luks-$UUID 2> /dev/null
	[ $? = 0 ] || error || return
	[ -b "$DEVICE" ] || error || return

	mkfs.ext2 -q -F -L "ext2_label" "$DEVICE"
	[ $? = 0 ] || error || return

	cryptsetup luksClose luks-$UUID 2> /dev/null
	[ $? = 0 ] || error || return

	ok "${LOOP_LIST[@]}"
}

function test_32()
{
	local IMAGE
	local LOOP
	local PASSWORD="password"

	echo -n "$FUNCNAME: "

	LOOP_LIST=()
	INDEX=0
	IMAGE="$(create_image $FUNCNAME$INDEX)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	LOOP_LIST+=($LOOP)

	echo "$PASSWORD" | cryptsetup luksFormat --batch-mode --force-password "$LOOP"
	[ $? = 0 ] || error || return

	UUID=$(cryptsetup luksUUID "$LOOP") 2> /dev/null
	[ $? = 0 ]     || error || return
	[ -n "$UUID" ] || error || return

	DEVICE="/dev/mapper/luks-$UUID"
	echo "$PASSWORD" | cryptsetup open --type luks "$LOOP" luks-$UUID 2> /dev/null
	[ $? = 0 ] || error || return
	[ -b "$DEVICE" ] || error || return

	mkfs.ext3 -q -F -L "ext3_label" "$DEVICE"
	[ $? = 0 ] || error || return

	cryptsetup luksClose luks-$UUID 2> /dev/null
	[ $? = 0 ] || error || return

	ok "${LOOP_LIST[@]}"
}

function test_33()
{
	local IMAGE
	local LOOP
	local PASSWORD="password"

	echo -n "$FUNCNAME: "

	LOOP_LIST=()
	INDEX=0
	IMAGE="$(create_image $FUNCNAME$INDEX)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	LOOP_LIST+=($LOOP)

	echo "$PASSWORD" | cryptsetup luksFormat --batch-mode --force-password "$LOOP"
	[ $? = 0 ] || error || return

	UUID=$(cryptsetup luksUUID "$LOOP") 2> /dev/null
	[ $? = 0 ]     || error || return
	[ -n "$UUID" ] || error || return

	DEVICE="/dev/mapper/luks-$UUID"
	echo "$PASSWORD" | cryptsetup open --type luks "$LOOP" luks-$UUID 2> /dev/null
	[ $? = 0 ] || error || return
	[ -b "$DEVICE" ] || error || return

	mkfs.ext4 -q -F -L "ext4_label" "$DEVICE"
	[ $? = 0 ] || error || return

	cryptsetup luksClose luks-$UUID 2> /dev/null
	[ $? = 0 ] || error || return

	ok "${LOOP_LIST[@]}"
}

function test_34()
{
	local IMAGE
	local LOOP
	local PASSWORD="password"

	echo -n "$FUNCNAME: "

	LOOP_LIST=()
	INDEX=0
	IMAGE="$(create_image $FUNCNAME$INDEX)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	LOOP_LIST+=($LOOP)

	echo "$PASSWORD" | cryptsetup luksFormat --batch-mode --force-password "$LOOP"
	[ $? = 0 ] || error || return

	UUID=$(cryptsetup luksUUID "$LOOP") 2> /dev/null
	[ $? = 0 ]     || error || return
	[ -n "$UUID" ] || error || return

	DEVICE="/dev/mapper/luks-$UUID"
	echo "$PASSWORD" | cryptsetup open --type luks "$LOOP" luks-$UUID 2> /dev/null
	[ $? = 0 ] || error || return
	[ -b "$DEVICE" ] || error || return

	mkfs.ntfs -q -fF -L "ntfs_label" "$DEVICE"
	[ $? = 0 ] || error || return

	cryptsetup luksClose luks-$UUID 2> /dev/null
	[ $? = 0 ] || error || return

	ok "${LOOP_LIST[@]}"
}

function test_35()
{
	local IMAGE
	local LOOP
	local PASSWORD="password"

	echo -n "$FUNCNAME: "

	LOOP_LIST=()
	INDEX=0
	IMAGE="$(create_image $FUNCNAME$INDEX)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	LOOP_LIST+=($LOOP)

	echo "$PASSWORD" | cryptsetup luksFormat --batch-mode --force-password "$LOOP"
	[ $? = 0 ] || error || return

	UUID=$(cryptsetup luksUUID "$LOOP") 2> /dev/null
	[ $? = 0 ]     || error || return
	[ -n "$UUID" ] || error || return

	DEVICE="/dev/mapper/luks-$UUID"
	echo "$PASSWORD" | cryptsetup open --type luks "$LOOP" luks-$UUID 2> /dev/null
	[ $? = 0 ] || error || return
	[ -b "$DEVICE" ] || error || return

	mkfs.reiserfs -q -ff -l "reiserfs_label" "$DEVICE" >& /dev/null
	[ $? = 0 ] || error || return

	cryptsetup luksClose luks-$UUID 2> /dev/null
	[ $? = 0 ] || error || return

	ok "${LOOP_LIST[@]}"
}

function test_36()
{
	local IMAGE
	local LOOP
	local PASSWORD="password"

	echo -n "$FUNCNAME: "

	LOOP_LIST=()
	INDEX=0
	IMAGE="$(create_image $FUNCNAME$INDEX)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	LOOP_LIST+=($LOOP)

	echo "$PASSWORD" | cryptsetup luksFormat --batch-mode --force-password "$LOOP"
	[ $? = 0 ] || error || return

	UUID=$(cryptsetup luksUUID "$LOOP") 2> /dev/null
	[ $? = 0 ]     || error || return
	[ -n "$UUID" ] || error || return

	DEVICE="/dev/mapper/luks-$UUID"
	echo "$PASSWORD" | cryptsetup open --type luks "$LOOP" luks-$UUID 2> /dev/null
	[ $? = 0 ] || error || return
	[ -b "$DEVICE" ] || error || return

	mkswap -L "swap_label" "$DEVICE" >& /dev/null
	[ $? = 0 ] || error || return

	cryptsetup luksClose luks-$UUID 2> /dev/null
	[ $? = 0 ] || error || return

	ok "${LOOP_LIST[@]}"
}

function test_37()
{
	local IMAGE
	local LOOP
	local PASSWORD="password"

	echo -n "$FUNCNAME: "

	LOOP_LIST=()
	INDEX=0
	IMAGE="$(create_image $FUNCNAME$INDEX)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	LOOP_LIST+=($LOOP)

	echo "$PASSWORD" | cryptsetup luksFormat --batch-mode --force-password "$LOOP"
	[ $? = 0 ] || error || return

	UUID=$(cryptsetup luksUUID "$LOOP") 2> /dev/null
	[ $? = 0 ]     || error || return
	[ -n "$UUID" ] || error || return

	DEVICE="/dev/mapper/luks-$UUID"
	echo "$PASSWORD" | cryptsetup open --type luks "$LOOP" luks-$UUID 2> /dev/null
	[ $? = 0 ] || error || return
	[ -b "$DEVICE" ] || error || return

	mkfs.vfat -n "vfat_label" "$DEVICE" >& /dev/null
	[ $? = 0 ] || error || return

	cryptsetup luksClose luks-$UUID 2> /dev/null
	[ $? = 0 ] || error || return

	ok "${LOOP_LIST[@]}"
}

function test_38()
{
	local IMAGE
	local LOOP
	local PASSWORD="password"

	echo -n "$FUNCNAME: "

	LOOP_LIST=()
	INDEX=0
	IMAGE="$(create_image $FUNCNAME$INDEX)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	LOOP_LIST+=($LOOP)

	echo "$PASSWORD" | cryptsetup luksFormat --batch-mode --force-password "$LOOP"
	[ $? = 0 ] || error || return

	UUID=$(cryptsetup luksUUID "$LOOP") 2> /dev/null
	[ $? = 0 ]     || error || return
	[ -n "$UUID" ] || error || return

	DEVICE="/dev/mapper/luks-$UUID"
	echo "$PASSWORD" | cryptsetup open --type luks "$LOOP" luks-$UUID 2> /dev/null
	[ $? = 0 ] || error || return
	[ -b "$DEVICE" ] || error || return

	mkfs.xfs -L "xfs_label" "$DEVICE" >& /dev/null
	[ $? = 0 ] || error || return

	cryptsetup luksClose luks-$UUID 2> /dev/null
	[ $? = 0 ] || error || return

	ok "${LOOP_LIST[@]}"
}


if [ $# = 0 ]; then
	#cleanup
	test_30
	test_31
	test_32
	test_33
	test_34
	test_35
	test_36
	test_37
	test_38
elif [ $# = 1 -a $1 = "-d" ]; then
	cleanup
else
	usage
fi

