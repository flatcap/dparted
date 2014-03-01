#!/bin/bash

IMAGE_SIZE="500M"

source common.sh

function test_50()
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

	mdadm -C --level=linear -n1 --force /dev/md0 "$LOOP" &> /dev/null
	[ $? = 0 ] || error || return

	ok "${LOOP_LIST[@]}"
}

function test_51()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP_LIST=()
	for INDEX in {0..7}; do
		IMAGE="$(create_image $FUNCNAME$INDEX)"
		[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

		LOOP="$(create_loop $IMAGE)"
		[ -n "$LOOP" -a -b "$LOOP" ] || error || return
		LOOP_LIST+=($LOOP)

		pvcreate "$LOOP" > /dev/null
		[ $? = 0 ] || error || return
	done

	GROUP="$FUNCNAME"

	vgcreate "$GROUP-linear" "${LOOP_LIST[0]}" > /dev/null
	[ $? = 0 ] || error || return

	vgcreate "$GROUP-mirror" "${LOOP_LIST[1]}" "${LOOP_LIST[2]}" > /dev/null
	[ $? = 0 ] || error || return

	vgcreate "$GROUP-raid" "${LOOP_LIST[3]}" "${LOOP_LIST[4]}" "${LOOP_LIST[5]}" > /dev/null
	[ $? = 0 ] || error || return

	vgcreate "$GROUP-stripe" "${LOOP_LIST[6]}" "${LOOP_LIST[7]}" > /dev/null
	[ $? = 0 ] || error || return

	lvcreate --size 400m --name test-linear $GROUP-linear > /dev/null
	[ $? = 0 ] || error || return

	lvcreate --size 400m --mirrors 1 --name test-mirror $GROUP-mirror > /dev/null
	[ $? = 0 ] || error || return

	lvcreate --size 500m --stripes 2 --type raid5 --name test-raid $GROUP-raid > /dev/null
	[ $? = 0 ] || error || return

	lvcreate --size 800m --stripes 2 --name test-stripe $GROUP-stripe > /dev/null
	[ $? = 0 ] || error || return

	ok "${LOOP_LIST[@]}"
}

function test_52()
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
	for INDEX in ${!COMMAND_LIST[*]}; do
		IMAGE="$(create_image $FUNCNAME$INDEX)"
		[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

		LOOP="$(create_loop $IMAGE)"
		[ -n "$LOOP" -a -b "$LOOP" ] || error || return
		LOOP_LIST+=($LOOP)

		pvcreate "$LOOP" > /dev/null
		[ $? = 0 ] || error || return

		GROUP="$FUNCNAME$INDEX"
		VOLUME="test-$INDEX"

		vgcreate "$GROUP" "$LOOP" > /dev/null
		[ $? = 0 ] || error || return

		lvcreate --size 400m --name $VOLUME $GROUP > /dev/null
		[ $? = 0 ] || error || return

		${COMMAND_LIST[$INDEX]} /dev/$GROUP/$VOLUME >& /dev/null
		[ $? = 0 ] || error || return
	done

	ok "${LOOP_LIST[@]}"
}

function test_53()
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
	for INDEX in ${!COMMAND_LIST[*]}; do
		IMAGE1="$(create_image $FUNCNAME$((INDEX*2)))"
		[ -n "$IMAGE1" -a -f "$IMAGE1" ] || error || return

		LOOP1="$(create_loop $IMAGE1)"
		[ -n "$LOOP1" -a -b "$LOOP1" ] || error || return
		LOOP_LIST+=($LOOP1)

		pvcreate "$LOOP1" > /dev/null
		[ $? = 0 ] || error || return

		IMAGE2="$(create_image $FUNCNAME$((INDEX*2+1)))"
		[ -n "$IMAGE2" -a -f "$IMAGE2" ] || error || return

		LOOP2="$(create_loop $IMAGE2)"
		[ -n "$LOOP2" -a -b "$LOOP2" ] || error || return
		LOOP_LIST+=($LOOP2)

		pvcreate "$LOOP2" > /dev/null
		[ $? = 0 ] || error || return

		GROUP="$FUNCNAME$INDEX"
		VOLUME="test-$INDEX"

		vgcreate "$GROUP" "$LOOP1" "$LOOP2" > /dev/null
		[ $? = 0 ] || error || return

		lvcreate --mirrors 1 --size 400m --name $VOLUME $GROUP > /dev/null
		[ $? = 0 ] || error || return

		${COMMAND_LIST[$INDEX]} /dev/$GROUP/$VOLUME >& /dev/null
		[ $? = 0 ] || error || return
	done

	ok "${LOOP_LIST[@]}"
}

function test_54()
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
	for INDEX in ${!COMMAND_LIST[*]}; do
		IMAGE1="$(create_image $FUNCNAME$((INDEX*3)))"
		[ -n "$IMAGE1" -a -f "$IMAGE1" ] || error || return

		LOOP1="$(create_loop $IMAGE1)"
		[ -n "$LOOP1" -a -b "$LOOP1" ] || error || return
		LOOP_LIST+=($LOOP1)

		pvcreate "$LOOP1" > /dev/null
		[ $? = 0 ] || error || return

		IMAGE2="$(create_image $FUNCNAME$((INDEX*3+1)))"
		[ -n "$IMAGE2" -a -f "$IMAGE2" ] || error || return

		LOOP2="$(create_loop $IMAGE2)"
		[ -n "$LOOP2" -a -b "$LOOP2" ] || error || return
		LOOP_LIST+=($LOOP2)

		pvcreate "$LOOP2" > /dev/null
		[ $? = 0 ] || error || return

		IMAGE3="$(create_image $FUNCNAME$((INDEX*3+2)))"
		[ -n "$IMAGE3" -a -f "$IMAGE3" ] || error || return

		LOOP3="$(create_loop $IMAGE3)"
		[ -n "$LOOP3" -a -b "$LOOP3" ] || error || return
		LOOP_LIST+=($LOOP3)

		pvcreate "$LOOP3" > /dev/null
		[ $? = 0 ] || error || return

		GROUP="$FUNCNAME$INDEX"
		VOLUME="test-$INDEX"

		vgcreate "$GROUP" "$LOOP1" "$LOOP2" "$LOOP3" > /dev/null
		[ $? = 0 ] || error || return

		lvcreate --stripes 2 --type raid5 --size 400m --name $VOLUME $GROUP > /dev/null
		[ $? = 0 ] || error || return

		${COMMAND_LIST[$INDEX]} /dev/$GROUP/$VOLUME >& /dev/null
		[ $? = 0 ] || error || return
	done

	ok "${LOOP_LIST[@]}"
}

function test_55()
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
	for INDEX in ${!COMMAND_LIST[*]}; do
		IMAGE1="$(create_image $FUNCNAME$((INDEX*2)))"
		[ -n "$IMAGE1" -a -f "$IMAGE1" ] || error || return

		LOOP1="$(create_loop $IMAGE1)"
		[ -n "$LOOP1" -a -b "$LOOP1" ] || error || return
		LOOP_LIST+=($LOOP1)

		pvcreate "$LOOP1" > /dev/null
		[ $? = 0 ] || error || return

		IMAGE2="$(create_image $FUNCNAME$((INDEX*2+1)))"
		[ -n "$IMAGE2" -a -f "$IMAGE2" ] || error || return

		LOOP2="$(create_loop $IMAGE2)"
		[ -n "$LOOP2" -a -b "$LOOP2" ] || error || return
		LOOP_LIST+=($LOOP2)

		pvcreate "$LOOP2" > /dev/null
		[ $? = 0 ] || error || return

		GROUP="$FUNCNAME$INDEX"
		VOLUME="test-$INDEX"

		vgcreate "$GROUP" "$LOOP1" "$LOOP2" > /dev/null
		[ $? = 0 ] || error || return

		lvcreate --stripes 2 --size 400m --name $VOLUME $GROUP > /dev/null
		[ $? = 0 ] || error || return

		${COMMAND_LIST[$INDEX]} /dev/$GROUP/$VOLUME >& /dev/null
		[ $? = 0 ] || error || return
	done

	ok "${LOOP_LIST[@]}"
}


if [ $# = 0 ]; then
	#cleanup
	test_50
	# test_51
	# test_52
	# test_53
	# test_54
	# test_55
elif [ $# = 1 -a $1 = "-d" ]; then
	cleanup
else
	usage
fi

