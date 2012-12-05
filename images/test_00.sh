#!/bin/bash

IMAGE_SIZE="5G"

source common.sh

##
# 00 disk, create one of each type of filesystem
function test_00()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	gpt_init "$IMAGE"
	[ $? = 0 ] || error || return

	gpt_create $IMAGE 1 498			# partition 1
	[ $? = 0 ] || error || return

	gpt_create $IMAGE 500 499		# partition 2
	[ $? = 0 ] || error || return

	gpt_create $IMAGE 1000 499		# partition 3
	[ $? = 0 ] || error || return

	gpt_create $IMAGE 1500 499		# partition 4
	[ $? = 0 ] || error || return

	gpt_create $IMAGE 2000 499		# partition 5
	[ $? = 0 ] || error || return

	gpt_create $IMAGE 2500 499		# partition 6
	[ $? = 0 ] || error || return

	gpt_create $IMAGE 3000 499		# partition 7
	[ $? = 0 ] || error || return

	gpt_create $IMAGE 3500 499		# partition 8
	[ $? = 0 ] || error || return

	gpt_create $IMAGE 4000 499		# partition 9
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
	[ -b "${LOOP}p9" ] || error || return

	#mkfs.btrfs        -L "btrfs_label"    ${LOOP}p1		# filesystem 1
	#[ $? = 0 ] || error || return

	mkfs.ext2  -q -F  -L "ext2_label"     ${LOOP}p2  &> /dev/null		# filesystem 2
	[ $? = 0 ] || error || return

	mkfs.ext3 -q  -F  -L "ext3_label"     ${LOOP}p3  &> /dev/null		# filesystem 3
	[ $? = 0 ] || error || return

	mkfs.ext4 -q  -F  -L "ext4_label"     ${LOOP}p4  &> /dev/null		# filesystem 4
	[ $? = 0 ] || error || return

	mkfs.ntfs -q  -fF -L "ntfs_label"     ${LOOP}p5  &> /dev/null		# filesystem 5
	[ $? = 0 ] || error || return

	mkfs.reiserfs -q -ff -l "reiserfs_label" ${LOOP}p6  &> /dev/null		# filesystem 6
	[ $? = 0 ] || error || return

	mkswap            -L "swap_label"     ${LOOP}p7  &> /dev/null		# filesystem 7
	[ $? = 0 ] || error || return

	mkfs.vfat         -n "vfat_label"     ${LOOP}p8  &> /dev/null		# filesystem 8
	[ $? = 0 ] || error || return

	mkfs.xfs          -L "xfs_label"      ${LOOP}p9  &> /dev/null		# filesystem 9
	[ $? = 0 ] || error || return

	ok "$LOOP"
}

##
# 01 disk, shuffled linear lvm volume
function test_01()
{
	local IMAGE
	local LOOP
	local GROUP=$FUNCNAME
	local INDEX=${FUNCNAME##*_}
	local VOLUME="simple$INDEX"

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return

	pvcreate "$LOOP" > /dev/null 2>&1
	[ $? = 0 ] || error || return

	vgcreate $GROUP "$LOOP" > /dev/null
	[ $? = 0 ] || error || return

	lvcreate --size 52m --name $VOLUME $GROUP > /dev/null
	lvcreate --size 52m --name junk1   $GROUP > /dev/null
	lvcreate --size 52m --name junk2   $GROUP > /dev/null
	lvcreate --size 40m --name junk3   $GROUP > /dev/null

	lvremove -f /dev/mapper/$GROUP-junk2		> /dev/null
	lvextend --size +52m /dev/mapper/$GROUP-$VOLUME	> /dev/null
	lvremove -f /dev/mapper/$GROUP-junk1		> /dev/null
	lvextend --size +52m /dev/mapper/$GROUP-$VOLUME	> /dev/null
	lvremove -f /dev/mapper/$GROUP-junk3		> /dev/null
	lvextend --size +40m /dev/mapper/$GROUP-$VOLUME	> /dev/null

	mke2fs -t ext4 -q -L "jigsaw" /dev/mapper/$GROUP-$VOLUME

	#lvs --all -o lv_name,vg_name,lv_attr,lv_size,lv_uuid,lv_path,lv_kernel_major,lv_kernel_minor,seg_count $VOLUME

	populate_ext4 /dev/mapper/$GROUP-$VOLUME 100M

	ok "$LOOP"
}

##
# 02 disk, create raid10 (mirrored striped) volume
function test_02()
{
	local IMAGE
	local LOOP
	local LOOP_LIST
	local GROUP=$FUNCNAME
	local INDEX=${FUNCNAME##*_}
	local VOLUME="$INDEX"

	echo -n "$FUNCNAME: "

	for i in {1..8}; do
		IMAGE="$(create_image ${FUNCNAME}$i)"
		[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

		LOOP="$(create_loop $IMAGE)"
		[ -n "$LOOP" -a -b "$LOOP" ] || error || return

		pvcreate "$LOOP" > /dev/null 2>&1
		[ $? = 0 ] || error || return

		LOOP_LIST="$LOOP_LIST $LOOP"
	done

	vgcreate $GROUP $LOOP_LIST > /dev/null
	[ $? = 0 ] || error || return

	lvcreate $GROUP -i 2 -m 3 --name bob --size 320M > /dev/null
	[ $? = 0 ] || error || return

	mke2fs -t ext4 -q -L "$GROUP bob" /dev/mapper/$GROUP-bob
	[ $? = 0 ] || error || return

	#populate_ext4 /dev/mapper/$VOLUME-bob 250M

	ok $LOOP_LIST
}


if [ $# = 0 ]; then
	cleanup
	test_00
	test_01
	test_02
elif [ $# = 1 -a $1 = "-d" ]; then
	cleanup
else
	usage
fi

