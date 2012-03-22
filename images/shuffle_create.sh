#!/bin/bash

IMAGE_SIZE="5G"

source common.sh

##
# ?? shuffled linear lvm volume
function test_shuffle()
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


if [ $# = 0 ]; then
	cleanup
	test_shuffle
elif [ $# = 1 -a $1 = "-d" ]; then
	cleanup
else
	usage
fi

