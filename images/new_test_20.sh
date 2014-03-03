#!/bin/bash

IMAGE_SIZE="500M"

source common.sh

##
# 11.01	Table.Msdos -> Partition.MsdosPartition.Primary
# 11.03	Table.Msdos -> Partition.Space.Reserved
# 11.04	Table.Msdos -> Partition.Space.Unallocated
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

##
# 06.01	Partition.MsdosPartition.Primary -> Filesystem.Btrfs
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

##
# 06.02	Partition.MsdosPartition.Primary -> Filesystem.Extfs.Ext2
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

##
# 06.03	Partition.MsdosPartition.Primary -> Filesystem.Extfs.Ext3
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

##
# 06.04	Partition.MsdosPartition.Primary -> Filesystem.Extfs.Ext4
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

##
# 06.05	Partition.MsdosPartition.Primary -> Filesystem.Ntfs
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

##
# 06.06	Partition.MsdosPartition.Primary -> Filesystem.Reiserfs
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

##
# 06.07	Partition.MsdosPartition.Primary -> Filesystem.Swap
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

##
# 06.08	Partition.MsdosPartition.Primary -> Filesystem.Vfat
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

##
# 06.09	Partition.MsdosPartition.Primary -> Filesystem.Xfs
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


##
# 11.05	Table.Msdos -> Table.Msdos.Extended
function test_30()
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

	msdos_create "$IMAGE" extended 50 400
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return

	LOOP_LIST+=($LOOP)

	ok "${LOOP_LIST[@]}"
}

##
# 12.01	Table.Msdos.Extended -> Partition.MsdosPartition.Primary
# 12.03	Table.Msdos.Extended -> Partition.Space.Reserved
# 12.04	Table.Msdos.Extended -> Partition.Space.Unallocated
function test_30x()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	msdos_init "$IMAGE"
	[ $? = 0 ] || error || return

	msdos_create $IMAGE extended 50 400
	[ $? = 0 ] || error || return

	msdos_create $IMAGE logical 51 399
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return
	[ -b "${LOOP}p5" ] || error || return

	ok "$LOOP"
}

##
# 06.01	Partition.MsdosPartition.Logical -> Filesystem.Btrfs
function test_31()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	msdos_init "$IMAGE"
	[ $? = 0 ] || error || return

	msdos_create $IMAGE extended 50 400
	[ $? = 0 ] || error || return

	msdos_create $IMAGE logical 51 399
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return
	[ -b "${LOOP}p5" ] || error || return

	mkfs.btrfs -L "btrfs_label" ${LOOP}p5 &> /dev/null
	[ $? = 0 ] || error || return

	ok "$LOOP"
}

##
# 06.02	Partition.MsdosPartition.Logical -> Filesystem.Extfs.Ext2
function test_32()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	msdos_init "$IMAGE"
	[ $? = 0 ] || error || return

	msdos_create $IMAGE extended 50 400
	[ $? = 0 ] || error || return

	msdos_create $IMAGE logical 51 399
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return
	[ -b "${LOOP}p5" ] || error || return

	mkfs.ext2 -q -F -L "ext2_label" ${LOOP}p5 &> /dev/null
	[ $? = 0 ] || error || return

	ok "$LOOP"
}

##
# 06.03	Partition.MsdosPartition.Logical -> Filesystem.Extfs.Ext3
function test_33()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	msdos_init "$IMAGE"
	[ $? = 0 ] || error || return

	msdos_create $IMAGE extended 50 400
	[ $? = 0 ] || error || return

	msdos_create $IMAGE logical 51 399
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return
	[ -b "${LOOP}p5" ] || error || return

	mkfs.ext3 -q -F -L "ext3_label" ${LOOP}p5 &> /dev/null
	[ $? = 0 ] || error || return

	ok "$LOOP"
}

##
# 06.04	Partition.MsdosPartition.Logical -> Filesystem.Extfs.Ext4
function test_34()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	msdos_init "$IMAGE"
	[ $? = 0 ] || error || return

	msdos_create $IMAGE extended 50 400
	[ $? = 0 ] || error || return

	msdos_create $IMAGE logical 51 399
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return
	[ -b "${LOOP}p5" ] || error || return

	mkfs.ext4 -q -F -L "ext4_label" ${LOOP}p5 &> /dev/null
	[ $? = 0 ] || error || return

	ok "$LOOP"
}

##
# 06.05	Partition.MsdosPartition.Logical -> Filesystem.Ntfs
function test_35()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	msdos_init "$IMAGE"
	[ $? = 0 ] || error || return

	msdos_create $IMAGE extended 50 400
	[ $? = 0 ] || error || return

	msdos_create $IMAGE logical 51 399
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return
	[ -b "${LOOP}p5" ] || error || return

	mkfs.ntfs -q -fF -L "ntfs_label" ${LOOP}p5 &> /dev/null
	[ $? = 0 ] || error || return

	ok "$LOOP"
}

##
# 06.06	Partition.MsdosPartition.Logical -> Filesystem.Reiserfs
function test_36()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	msdos_init "$IMAGE"
	[ $? = 0 ] || error || return

	msdos_create $IMAGE extended 50 400
	[ $? = 0 ] || error || return

	msdos_create $IMAGE logical 51 399
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return
	[ -b "${LOOP}p5" ] || error || return

	mkfs.reiserfs -q -ff -l "reiserfs_label" ${LOOP}p5 &> /dev/null
	[ $? = 0 ] || error || return

	ok "$LOOP"
}

##
# 06.07	Partition.MsdosPartition.Logical -> Filesystem.Swap
function test_37()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	msdos_init "$IMAGE"
	[ $? = 0 ] || error || return

	msdos_create $IMAGE extended 50 400
	[ $? = 0 ] || error || return

	msdos_create $IMAGE logical 51 399
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return
	[ -b "${LOOP}p5" ] || error || return

	mkswap -L "swap_label" ${LOOP}p5 &> /dev/null
	[ $? = 0 ] || error || return

	ok "$LOOP"
}

##
# 06.08	Partition.MsdosPartition.Logical -> Filesystem.Vfat
function test_38()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	msdos_init "$IMAGE"
	[ $? = 0 ] || error || return

	msdos_create $IMAGE extended 50 400
	[ $? = 0 ] || error || return

	msdos_create $IMAGE logical 51 399
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return
	[ -b "${LOOP}p5" ] || error || return

	mkfs.vfat -n "vfat_label" ${LOOP}p5 &> /dev/null
	[ $? = 0 ] || error || return

	ok "$LOOP"
}

##
# 06.09	Partition.MsdosPartition.Logical -> Filesystem.Xfs
function test_39()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	msdos_init "$IMAGE"
	[ $? = 0 ] || error || return

	msdos_create $IMAGE extended 50 400
	[ $? = 0 ] || error || return

	msdos_create $IMAGE logical 51 399
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return
	[ -b "${LOOP}p5" ] || error || return

	mkfs.xfs -L "xfs_label" ${LOOP}p5 &> /dev/null
	[ $? = 0 ] || error || return

	ok "$LOOP"
}


# 06.10	Partition.MsdosPartition.Logical -> Misc.Random
# 06.11	Partition.MsdosPartition.Logical -> Misc.Zero
# 06.12	Partition.MsdosPartition.Logical -> Wrapper.Luks

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
	test_30
	test_30x
	test_31
	test_32
	test_33
	test_34
	test_35
	test_36
	test_37
	test_38
	test_39
elif [ $# = 1 -a $1 = "-d" ]; then
	cleanup
else
	usage
fi

