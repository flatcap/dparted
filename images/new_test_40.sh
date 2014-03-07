#!/bin/bash

IMAGE_SIZE="500M"

FS_LIST=(
	"mkfs.btrfs           -L btrfs_label"
	"mkfs.ext2     -q -F  -L ext2_label"
	"mkfs.ext3     -q -F  -L ext3_label"
	"mkfs.ext4     -q -F  -L ext4_label"
	"mkfs.ntfs     -q -fF -L ntfs_label"
	"mkfs.reiserfs -q -ff -l reiserfs_label"
	"mkswap               -L swap_label"
	"mkfs.vfat            -n vfat_label"
	"mkfs.xfs             -L xfs_label"
)

source common.sh

##
# 09.01	Table.LvmTable -> Partition.LvmPartition
# 09.03	Table.LvmTable -> Partition.Space.Reserved
# 09.04	Table.LvmTable -> Partition.Space.Unallocated
function test_40()
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

	GROUP="$FUNCNAME"
	vgcreate "$GROUP" "$LOOP" > /dev/null
	[ $? = 0 ] || error || return

	lvcreate --size 400m --name test $GROUP $LOOP:12-111 > /dev/null
	[ $? = 0 ] || error || return

	ok "${LOOP_LIST[@]}"
}

##
# 13.01	Whole.Group.LvmGroup -> Whole.Volume.LvmVolume.LvmLinear
# 13.02	Whole.Group.LvmGroup -> Whole.Volume.LvmVolume.LvmLinear.LvmMetadata
# 13.03	Whole.Group.LvmGroup -> Whole.Volume.LvmVolume.LvmMirror
# 13.04	Whole.Group.LvmGroup -> Whole.Volume.LvmVolume.LvmRaid
# 13.05	Whole.Group.LvmGroup -> Whole.Volume.LvmVolume.LvmStripe
function test_41()
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

##
# 14.01	Whole.Volume.LvmVolume.LvmLinear -> Filesystem.Btrfs
# 14.02	Whole.Volume.LvmVolume.LvmLinear -> Filesystem.Extfs.Ext2
# 14.03	Whole.Volume.LvmVolume.LvmLinear -> Filesystem.Extfs.Ext3
# 14.04	Whole.Volume.LvmVolume.LvmLinear -> Filesystem.Extfs.Ext4
# 14.05	Whole.Volume.LvmVolume.LvmLinear -> Filesystem.Ntfs
# 14.06	Whole.Volume.LvmVolume.LvmLinear -> Filesystem.Reiserfs
# 14.07	Whole.Volume.LvmVolume.LvmLinear -> Filesystem.Swap
# 14.08	Whole.Volume.LvmVolume.LvmLinear -> Filesystem.Vfat
# 14.09	Whole.Volume.LvmVolume.LvmLinear -> Filesystem.Xfs
function test_42()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP_LIST=()
	for INDEX in ${!FS_LIST[*]}; do
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

		${FS_LIST[$INDEX]} /dev/$GROUP/$VOLUME >& /dev/null
		[ $? = 0 ] || error || return
	done

	ok "${LOOP_LIST[@]}"
}

##
# 15.01	Whole.Volume.LvmVolume.LvmMirror -> Filesystem.Btrfs
# 15.02	Whole.Volume.LvmVolume.LvmMirror -> Filesystem.Extfs.Ext2
# 15.03	Whole.Volume.LvmVolume.LvmMirror -> Filesystem.Extfs.Ext3
# 15.04	Whole.Volume.LvmVolume.LvmMirror -> Filesystem.Extfs.Ext4
# 15.05	Whole.Volume.LvmVolume.LvmMirror -> Filesystem.Ntfs
# 15.06	Whole.Volume.LvmVolume.LvmMirror -> Filesystem.Reiserfs
# 15.07	Whole.Volume.LvmVolume.LvmMirror -> Filesystem.Swap
# 15.08	Whole.Volume.LvmVolume.LvmMirror -> Filesystem.Vfat
# 15.09	Whole.Volume.LvmVolume.LvmMirror -> Filesystem.Xfs
function test_43()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP_LIST=()
	for INDEX in ${!FS_LIST[*]}; do
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

		${FS_LIST[$INDEX]} /dev/$GROUP/$VOLUME >& /dev/null
		[ $? = 0 ] || error || return

		break
	done

	ok "${LOOP_LIST[@]}"
}

##
# 16.01	Whole.Volume.LvmVolume.LvmRaid -> Filesystem.Btrfs
# 16.02	Whole.Volume.LvmVolume.LvmRaid -> Filesystem.Extfs.Ext2
# 16.03	Whole.Volume.LvmVolume.LvmRaid -> Filesystem.Extfs.Ext3
# 16.04	Whole.Volume.LvmVolume.LvmRaid -> Filesystem.Extfs.Ext4
# 16.05	Whole.Volume.LvmVolume.LvmRaid -> Filesystem.Ntfs
# 16.06	Whole.Volume.LvmVolume.LvmRaid -> Filesystem.Reiserfs
# 16.07	Whole.Volume.LvmVolume.LvmRaid -> Filesystem.Swap
# 16.08	Whole.Volume.LvmVolume.LvmRaid -> Filesystem.Vfat
# 16.09	Whole.Volume.LvmVolume.LvmRaid -> Filesystem.Xfs
function test_44()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP_LIST=()
	for INDEX in ${!FS_LIST[*]}; do
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

		${FS_LIST[$INDEX]} /dev/$GROUP/$VOLUME >& /dev/null
		[ $? = 0 ] || error || return
	done

	ok "${LOOP_LIST[@]}"
}

##
# 17.01	Whole.Volume.LvmVolume.LvmStripe -> Filesystem.Btrfs
# 17.02	Whole.Volume.LvmVolume.LvmStripe -> Filesystem.Extfs.Ext2
# 17.03	Whole.Volume.LvmVolume.LvmStripe -> Filesystem.Extfs.Ext3
# 17.04	Whole.Volume.LvmVolume.LvmStripe -> Filesystem.Extfs.Ext4
# 17.05	Whole.Volume.LvmVolume.LvmStripe -> Filesystem.Ntfs
# 17.06	Whole.Volume.LvmVolume.LvmStripe -> Filesystem.Reiserfs
# 17.07	Whole.Volume.LvmVolume.LvmStripe -> Filesystem.Swap
# 17.08	Whole.Volume.LvmVolume.LvmStripe -> Filesystem.Vfat
# 17.09	Whole.Volume.LvmVolume.LvmStripe -> Filesystem.Xfs
function test_45()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP_LIST=()
	for INDEX in ${!FS_LIST[*]}; do
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

		${FS_LIST[$INDEX]} /dev/$GROUP/$VOLUME >& /dev/null
		[ $? = 0 ] || error || return
	done

	ok "${LOOP_LIST[@]}"
}


# 14.10	Whole.Volume.LvmVolume.LvmLinear -> Misc.Random
# 14.11	Whole.Volume.LvmVolume.LvmLinear -> Misc.Zero
# 14.12	Whole.Volume.LvmVolume.LvmLinear -> Wrapper.Luks
# 15.10	Whole.Volume.LvmVolume.LvmMirror -> Misc.Random
# 15.11	Whole.Volume.LvmVolume.LvmMirror -> Misc.Zero
# 15.12	Whole.Volume.LvmVolume.LvmMirror -> Wrapper.Luks
# 16.10	Whole.Volume.LvmVolume.LvmRaid -> Misc.Random
# 16.11	Whole.Volume.LvmVolume.LvmRaid -> Misc.Zero
# 16.12	Whole.Volume.LvmVolume.LvmRaid -> Wrapper.Luks
# 17.10	Whole.Volume.LvmVolume.LvmStripe -> Misc.Random
# 17.11	Whole.Volume.LvmVolume.LvmStripe -> Misc.Zero
# 17.12	Whole.Volume.LvmVolume.LvmStripe -> Wrapper.Luks

if [ $# = 0 ]; then
	#cleanup
	test_40
	test_41
	test_42
	test_43
	test_44
	test_45
elif [ $# = 1 -a $1 = "-d" ]; then
	cleanup
else
	usage
fi

