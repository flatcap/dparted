#!/bin/bash

IMAGE_SIZE="5G"

source common.sh

##
# 50 disk, lvm table, empty
function test_50()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return

	pvcreate "$LOOP" > /dev/null 2>&1
	[ $? = 0 ] || error || return

	ok "$LOOP"
}

##
# 51 disk, lvm table, volume group, empty
function test_51()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return

	pvcreate "$LOOP" > /dev/null 2>&1
	[ $? = 0 ] || error || return

	vgcreate $FUNCNAME "$LOOP" > /dev/null
	[ $? = 0 ] || error || return

	ok "$LOOP"
}

##
# 52 disk, lvm table, volume group, partition, empty
function test_52()
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

	lvcreate --size 2500m --name $VOLUME $GROUP > /dev/null
	[ $? = 0 ] || error || return

	ok "$LOOP"
}

##
# 53 disk, lvm table, volume group, partition, unknown
function test_53()
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

	lvcreate --size 2500m --name $VOLUME $GROUP > /dev/null
	[ $? = 0 ] || error || return

	dd if=/dev/urandom bs=32K count=1 of="/dev/mapper/$GROUP-$VOLUME" 2> /dev/null

	ok "$LOOP"
}

##
# 54 disk, lvm table, volume group, partition, filesystem
function test_54()
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

	lvcreate --size 2500m --name $VOLUME $GROUP > /dev/null
	[ $? = 0 ] || error || return

	mke2fs -t ext4 -q "/dev/mapper/$GROUP-$VOLUME"
	[ $? = 0 ] || error || return

	ok "$LOOP"
}

##
# 55 disk, lvm table, volume group, (partition, filesystem) * 8
# (include empty space)
function test_55()
{
	local IMAGE
	local LOOP
	local GROUP=$FUNCNAME
	local INDEX=${FUNCNAME##*_}
	local VOLUME

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return

	pvcreate "$LOOP" > /dev/null 2>&1
	[ $? = 0 ] || error || return

	vgcreate $GROUP "$LOOP" > /dev/null
	[ $? = 0 ] || error || return

	VOLUME="simple$INDEX.1"
	lvcreate --size 200m --name $VOLUME $GROUP > /dev/null	# filesystem 1
	[ $? = 0 ] || error || return

	mke2fs -t ext4 -q "/dev/mapper/$GROUP-$VOLUME"
	[ $? = 0 ] || error || return

	VOLUME="gap$INDEX.1"
	lvcreate --size 200m --name $VOLUME $GROUP > /dev/null	# gap 1
	[ $? = 0 ] || error || return

	VOLUME="simple$INDEX.2"
	lvcreate --size 500m --name $VOLUME $GROUP > /dev/null	# filesystem 2
	[ $? = 0 ] || error || return

	mke2fs -t ext4 -q "/dev/mapper/$GROUP-$VOLUME"
	[ $? = 0 ] || error || return

	VOLUME="simple$INDEX.3"
	lvcreate --size 500m --name $VOLUME $GROUP > /dev/null	# filesystem 3
	[ $? = 0 ] || error || return

	mke2fs -t ext4 -q "/dev/mapper/$GROUP-$VOLUME"
	[ $? = 0 ] || error || return

	VOLUME="gap$INDEX.2"
	lvcreate --size 500m --name $VOLUME $GROUP > /dev/null	# gap 2
	[ $? = 0 ] || error || return

	VOLUME="simple$INDEX.4"
	lvcreate --size 200m --name $VOLUME $GROUP > /dev/null	# filesystem 4
	[ $? = 0 ] || error || return

	mke2fs -t ext4 -q "/dev/mapper/$GROUP-$VOLUME"
	[ $? = 0 ] || error || return

	VOLUME="simple$INDEX.5"
	lvcreate --size 200m --name $VOLUME $GROUP > /dev/null	# filesystem 5
	[ $? = 0 ] || error || return

	mke2fs -t ext4 -q "/dev/mapper/$GROUP-$VOLUME"
	[ $? = 0 ] || error || return

	VOLUME="gap$INDEX.3"
	lvcreate --size 100m --name $VOLUME $GROUP > /dev/null	# gap 3
	[ $? = 0 ] || error || return

	VOLUME="simple$INDEX.6"
	lvcreate --size 200m --name $VOLUME $GROUP > /dev/null	# filesystem 6
	[ $? = 0 ] || error || return

	mke2fs -t ext4 -q "/dev/mapper/$GROUP-$VOLUME"
	[ $? = 0 ] || error || return

	VOLUME="simple$INDEX.7"
	lvcreate --size 400m --name $VOLUME $GROUP > /dev/null	# filesystem 7
	[ $? = 0 ] || error || return

	mke2fs -t ext4 -q "/dev/mapper/$GROUP-$VOLUME"
	[ $? = 0 ] || error || return

	VOLUME="gap$INDEX.4"
	lvcreate --size 200m --name $VOLUME $GROUP > /dev/null	# gap 4
	[ $? = 0 ] || error || return

	VOLUME="simple$INDEX.8"
	lvcreate --size 200m --name $VOLUME $GROUP > /dev/null	# filesystem 8
	[ $? = 0 ] || error || return

	mke2fs -t ext4 -q "/dev/mapper/$GROUP-$VOLUME"
	[ $? = 0 ] || error || return

	lvremove -f /dev/mapper/$GROUP-gap* > /dev/null
	[ $? = 0 ] || error || return

	ok "$LOOP"
}


if [ $# = 0 ]; then
	cleanup
	test_50
	test_51
	test_52
	test_53
	test_54
	test_55
elif [ $# = 1 -a $1 = "-d" ]; then
	cleanup
else
	usage
fi

