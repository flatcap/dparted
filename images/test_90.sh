#!/bin/bash

IMAGE_SIZE="5G"

source common.sh

##
# 90 disk, create raid5 volume
function test_90()
{
	local IMAGE
	local LOOP
	local LOOP_LIST
	local GROUP=$FUNCNAME

	echo -n "$FUNCNAME: "

	for i in {1..3}; do
		IMAGE="$(create_image ${FUNCNAME}$i)"
		[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

		LOOP="$(create_loop $IMAGE)"
		[ -n "$LOOP" -a -b "$LOOP" ] || error || return

		LOOP_LIST="$LOOP_LIST $LOOP"
	done

	mdadm --create /dev/md0 --level=5 --raid-devices=3 $LOOP_LIST
	[ $? = 0 ] || error || return

	mke2fs -t ext4 -L md_raid /dev/md0
	[ $? = 0 ] || error || return

	populate_ext4 /dev/md0 1200M
	[ $? = 0 ] || error || return

	ok $LOOP_LIST
}


if [ $# = 0 ]; then
	cleanup
	test_90
elif [ $# = 1 -a $1 = "-d" ]; then
	cleanup
else
	usage
fi

