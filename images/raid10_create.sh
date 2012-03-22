#!/bin/bash

IMAGE_SIZE="5G"

source common.sh

function test_bigvol()
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

	ok "$LOOP_LIST"
}

if [ $# = 0 ]; then
	cleanup
	test_bigvol
elif [ $# = 1 -a $1 = "-d" ]; then
	cleanup
else
	usage
fi

