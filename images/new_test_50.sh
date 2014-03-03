#!/bin/bash

IMAGE_SIZE="500M"
MD_INDEX=0

source common.sh

##
# 10.01	Table.MdTable -> Partition.MdPartition
# 10.03	Table.MdTable -> Partition.Space.Reserved
# 10.04	Table.MdTable -> Partition.Space.Unallocated
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

	mdadm -C --level=linear -n1 --force /dev/md$MD_INDEX "$LOOP" &> /dev/null
	[ $? = 0 ] || error || return
	[ -b "/dev/md$MD_INDEX" ] || error || return
	: $((MD_INDEX++))

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
	done

	GROUP="$FUNCNAME"

	mdadm -C --level=linear -n1 --force /dev/md$MD_INDEX "${LOOP_LIST[0]}" >& /dev/null
	[ $? = 0 ] || error || return
	[ -b "/dev/md$MD_INDEX" ] || error || return
	: $((MD_INDEX++))

	mdadm --create --run --level=mirror -n2 /dev/md$MD_INDEX "${LOOP_LIST[1]}" "${LOOP_LIST[2]}" >& /dev/null
	[ $? = 0 ] || error || return
	[ -b "/dev/md$MD_INDEX" ] || error || return
	: $((MD_INDEX++))

	mdadm --create --run --level=raid5 -n3 /dev/md$MD_INDEX "${LOOP_LIST[3]}" "${LOOP_LIST[4]}" "${LOOP_LIST[5]}" >& /dev/null
	[ $? = 0 ] || error || return
	[ -b "/dev/md$MD_INDEX" ] || error || return
	: $((MD_INDEX++))

	mdadm --create --run --level=stripe -n2 /dev/md$MD_INDEX "${LOOP_LIST[6]}" "${LOOP_LIST[7]}" >& /dev/null
	[ $? = 0 ] || error || return
	[ -b "/dev/md$MD_INDEX" ] || error || return
	: $((MD_INDEX++))

	ok "${LOOP_LIST[@]}"
}

##
# 18.01	Whole.Volume.MdVolume.MdLinear -> Filesystem.Btrfs
# 18.02	Whole.Volume.MdVolume.MdLinear -> Filesystem.Extfs.Ext2
# 18.03	Whole.Volume.MdVolume.MdLinear -> Filesystem.Extfs.Ext3
# 18.04	Whole.Volume.MdVolume.MdLinear -> Filesystem.Extfs.Ext4
# 18.05	Whole.Volume.MdVolume.MdLinear -> Filesystem.Ntfs
# 18.06	Whole.Volume.MdVolume.MdLinear -> Filesystem.Reiserfs
# 18.07	Whole.Volume.MdVolume.MdLinear -> Filesystem.Swap
# 18.08	Whole.Volume.MdVolume.MdLinear -> Filesystem.Vfat
# 18.09	Whole.Volume.MdVolume.MdLinear -> Filesystem.Xfs
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

		mdadm -C --level=linear -n1 --force /dev/md$MD_INDEX "$LOOP" >& /dev/null
		[ $? = 0 ] || error || return
		[ -b "/dev/md$MD_INDEX" ] || error || return

		${COMMAND_LIST[$INDEX]} /dev/md$MD_INDEX >& /dev/null
		[ $? = 0 ] || error || return

		: $((MD_INDEX++))
	done

	ok "${LOOP_LIST[@]}"
}

##
# 19.01	Whole.Volume.MdVolume.MdMirror -> Filesystem.Btrfs
# 19.02	Whole.Volume.MdVolume.MdMirror -> Filesystem.Extfs.Ext2
# 19.03	Whole.Volume.MdVolume.MdMirror -> Filesystem.Extfs.Ext3
# 19.04	Whole.Volume.MdVolume.MdMirror -> Filesystem.Extfs.Ext4
# 19.05	Whole.Volume.MdVolume.MdMirror -> Filesystem.Ntfs
# 19.06	Whole.Volume.MdVolume.MdMirror -> Filesystem.Reiserfs
# 19.07	Whole.Volume.MdVolume.MdMirror -> Filesystem.Swap
# 19.08	Whole.Volume.MdVolume.MdMirror -> Filesystem.Vfat
# 19.09	Whole.Volume.MdVolume.MdMirror -> Filesystem.Xfs
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

		IMAGE2="$(create_image $FUNCNAME$((INDEX*2+1)))"
		[ -n "$IMAGE2" -a -f "$IMAGE2" ] || error || return

		LOOP2="$(create_loop $IMAGE2)"
		[ -n "$LOOP2" -a -b "$LOOP2" ] || error || return
		LOOP_LIST+=($LOOP2)

		mdadm --create --run --level=mirror -n2 /dev/md$MD_INDEX "$LOOP1" "$LOOP2" >& /dev/null
		[ $? = 0 ] || error || return
		[ -b "/dev/md$MD_INDEX" ] || error || return

		${COMMAND_LIST[$INDEX]} /dev/md$MD_INDEX >& /dev/null
		[ $? = 0 ] || error || return

		: $((MD_INDEX++))
	done

	ok "${LOOP_LIST[@]}"
}

##
# 20.01	Whole.Volume.MdVolume.MdRaid -> Filesystem.Btrfs
# 20.02	Whole.Volume.MdVolume.MdRaid -> Filesystem.Extfs.Ext2
# 20.03	Whole.Volume.MdVolume.MdRaid -> Filesystem.Extfs.Ext3
# 20.04	Whole.Volume.MdVolume.MdRaid -> Filesystem.Extfs.Ext4
# 20.05	Whole.Volume.MdVolume.MdRaid -> Filesystem.Ntfs
# 20.06	Whole.Volume.MdVolume.MdRaid -> Filesystem.Reiserfs
# 20.07	Whole.Volume.MdVolume.MdRaid -> Filesystem.Swap
# 20.08	Whole.Volume.MdVolume.MdRaid -> Filesystem.Vfat
# 20.09	Whole.Volume.MdVolume.MdRaid -> Filesystem.Xfs
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

		IMAGE2="$(create_image $FUNCNAME$((INDEX*3+1)))"
		[ -n "$IMAGE2" -a -f "$IMAGE2" ] || error || return

		LOOP2="$(create_loop $IMAGE2)"
		[ -n "$LOOP2" -a -b "$LOOP2" ] || error || return
		LOOP_LIST+=($LOOP2)

		IMAGE3="$(create_image $FUNCNAME$((INDEX*3+2)))"
		[ -n "$IMAGE3" -a -f "$IMAGE3" ] || error || return

		LOOP3="$(create_loop $IMAGE3)"
		[ -n "$LOOP3" -a -b "$LOOP3" ] || error || return
		LOOP_LIST+=($LOOP3)

		mdadm --create --run --level=raid5 -n3 /dev/md$MD_INDEX "$LOOP1" "$LOOP2" "$LOOP3" >& /dev/null
		[ $? = 0 ] || error || return
		[ -b "/dev/md$MD_INDEX" ] || error || return

		${COMMAND_LIST[$INDEX]} /dev/md$MD_INDEX >& /dev/null
		[ $? = 0 ] || error || return

		: $((MD_INDEX++))
	done

	ok "${LOOP_LIST[@]}"
}

##
# 21.01	Whole.Volume.MdVolume.MdStripe -> Filesystem.Btrfs
# 21.02	Whole.Volume.MdVolume.MdStripe -> Filesystem.Extfs.Ext2
# 21.03	Whole.Volume.MdVolume.MdStripe -> Filesystem.Extfs.Ext3
# 21.04	Whole.Volume.MdVolume.MdStripe -> Filesystem.Extfs.Ext4
# 21.05	Whole.Volume.MdVolume.MdStripe -> Filesystem.Ntfs
# 21.06	Whole.Volume.MdVolume.MdStripe -> Filesystem.Reiserfs
# 21.07	Whole.Volume.MdVolume.MdStripe -> Filesystem.Swap
# 21.08	Whole.Volume.MdVolume.MdStripe -> Filesystem.Vfat
# 21.09	Whole.Volume.MdVolume.MdStripe -> Filesystem.Xfs
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

		IMAGE2="$(create_image $FUNCNAME$((INDEX*2+1)))"
		[ -n "$IMAGE2" -a -f "$IMAGE2" ] || error || return

		LOOP2="$(create_loop $IMAGE2)"
		[ -n "$LOOP2" -a -b "$LOOP2" ] || error || return
		LOOP_LIST+=($LOOP2)

		mdadm --create --run --level=stripe -n2 /dev/md$MD_INDEX "$LOOP1" "$LOOP2" >& /dev/null
		[ $? = 0 ] || error || return
		[ -b "/dev/md$MD_INDEX" ] || error || return

		${COMMAND_LIST[$INDEX]} /dev/md$MD_INDEX >& /dev/null
		[ $? = 0 ] || error || return

		: $((MD_INDEX++))
	done

	ok "${LOOP_LIST[@]}"
}


# 18.10	Whole.Volume.MdVolume.MdLinear -> Misc.Random
# 18.11	Whole.Volume.MdVolume.MdLinear -> Misc.Zero
# 18.12	Whole.Volume.MdVolume.MdLinear -> Wrapper.Luks
# 19.10	Whole.Volume.MdVolume.MdMirror -> Misc.Random
# 19.11	Whole.Volume.MdVolume.MdMirror -> Misc.Zero
# 19.12	Whole.Volume.MdVolume.MdMirror -> Wrapper.Luks
# 20.10	Whole.Volume.MdVolume.MdRaid -> Misc.Random
# 20.11	Whole.Volume.MdVolume.MdRaid -> Misc.Zero
# 20.12	Whole.Volume.MdVolume.MdRaid -> Wrapper.Luks
# 21.10	Whole.Volume.MdVolume.MdStripe -> Misc.Random
# 21.11	Whole.Volume.MdVolume.MdStripe -> Misc.Zero
# 21.12	Whole.Volume.MdVolume.MdStripe -> Wrapper.Luks

if [ $# = 0 ]; then
	#cleanup
	# test_50
	# test_51
	# test_52
	# test_53
	# test_54
	test_55
elif [ $# = 1 -a $1 = "-d" ]; then
	cleanup
else
	usage
fi

