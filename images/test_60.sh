#!/bin/bash

IMAGE_SIZE="5G"

source common.sh

##
# 60 disk, msdos table, partition, lvm table, empty
function test_60()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	msdos_init "$IMAGE"
	[ $? = 0 ] || error || return

	msdos_create $IMAGE primary 500 4000	# partition 1
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return

	pvcreate "${LOOP}p1" &> /dev/null
	[ $? = 0 ] || error || return

	ok "$LOOP"
}

##
# 61 disk, msdos table, partition, lvm table, volume group, empty
function test_61()
{
	local IMAGE
	local LOOP
	local GROUP=$FUNCNAME

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	msdos_init "$IMAGE"
	[ $? = 0 ] || error || return

	msdos_create $IMAGE primary 500 4000	# partition 1
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return

	pvcreate "${LOOP}p1" &> /dev/null
	[ $? = 0 ] || error || return

	vgcreate $GROUP "${LOOP}p1" > /dev/null
	[ $? = 0 ] || error || return

	ok "$LOOP"
}

##
# 62 disk, msdos table, partition, lvm table, volume group, partition, empty
function test_62()
{
	local IMAGE
	local LOOP
	local GROUP=$FUNCNAME
	local INDEX=${FUNCNAME##*_}
	local VOLUME="simple$INDEX"

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	msdos_init "$IMAGE"
	[ $? = 0 ] || error || return

	msdos_create $IMAGE primary 500 4000			# partition 1
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return

	pvcreate "${LOOP}p1" &> /dev/null
	[ $? = 0 ] || error || return

	vgcreate $GROUP "${LOOP}p1" > /dev/null
	[ $? = 0 ] || error || return

	lvcreate --size 500m --name $VOLUME $GROUP > /dev/null	# lv partition 1
	[ $? = 0 ] || error || return

	ok "$LOOP"
}

##
# 63 disk, msdos table, partition, lvm table, volume group, partition, unknown
function test_63()
{
	local IMAGE
	local LOOP
	local GROUP=$FUNCNAME
	local INDEX=${FUNCNAME##*_}
	local VOLUME="simple$INDEX"

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	msdos_init "$IMAGE"
	[ $? = 0 ] || error || return

	msdos_create $IMAGE primary 500 4000			# partition 1
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return

	pvcreate "${LOOP}p1" &> /dev/null
	[ $? = 0 ] || error || return

	vgcreate $GROUP "${LOOP}p1" > /dev/null
	[ $? = 0 ] || error || return

	lvcreate --size 500m --name $VOLUME $GROUP > /dev/null	# lv partition 1
	[ $? = 0 ] || error || return

	dd if=/dev/urandom bs=32K count=1 of="/dev/mapper/$GROUP-$VOLUME" 2> /dev/null

	ok "$LOOP"
}

##
# 64 disk, msdos table, partition, lvm table, volume group, partition, filesystem
function test_64()
{
	local IMAGE
	local LOOP
	local GROUP=$FUNCNAME
	local INDEX=${FUNCNAME##*_}
	local VOLUME="simple$INDEX"

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	msdos_init "$IMAGE"
	[ $? = 0 ] || error || return

	msdos_create $IMAGE primary 500 4000			# partition 1
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return

	pvcreate "${LOOP}p1" &> /dev/null
	[ $? = 0 ] || error || return

	vgcreate $GROUP "${LOOP}p1" > /dev/null
	[ $? = 0 ] || error || return

	lvcreate --size 500m --name $VOLUME $GROUP > /dev/null	# lv partition 1
	[ $? = 0 ] || error || return

	mke2fs -t ext4 -q "/dev/mapper/$GROUP-$VOLUME"
	[ $? = 0 ] || error || return

	ok "$LOOP"
}

##
# 65 disk, msdos table, partition, lvm table, volume group, (partition, filesystem) * 8
# (include empty space)
function test_65()
{
	local IMAGE
	local LOOP
	local GROUP=$FUNCNAME
	local INDEX=${FUNCNAME##*_}
	local VOLUME

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	msdos_init "$IMAGE"
	[ $? = 0 ] || error || return

	msdos_create $IMAGE primary 500 4000			# partition 1
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return

	pvcreate "${LOOP}p1" &> /dev/null
	[ $? = 0 ] || error || return

	vgcreate $GROUP "${LOOP}p1" > /dev/null
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

##
# 66 disk, msdos table, extended partition, lvm table, volume group, partition, filesystem
function test_66()
{
	local IMAGE
	local LOOP
	local GROUP=$FUNCNAME
	local INDEX=${FUNCNAME##*_}
	local VOLUME="simple$INDEX"

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	msdos_init "$IMAGE"
	[ $? = 0 ] || error || return

	msdos_create $IMAGE extended 500 4000			# partition 1
	[ $? = 0 ] || error || return

	msdos_create $IMAGE logical 600 2000			# logical partition 5
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return
	[ -b "${LOOP}p5" ] || error || return

	pvcreate "${LOOP}p5" &> /dev/null
	[ $? = 0 ] || error || return

	vgcreate $GROUP "${LOOP}p5" > /dev/null
	[ $? = 0 ] || error || return

	lvcreate --size 500m --name $VOLUME $GROUP > /dev/null	# lv partition 1
	[ $? = 0 ] || error || return

	mke2fs -t ext4 -q "/dev/mapper/$GROUP-$VOLUME"
	[ $? = 0 ] || error || return

	ok "$LOOP"
}


if [ $# = 0 ]; then
	cleanup
	test_60
	test_61
	test_62
	test_63
	test_64
	test_65
	test_66
elif [ $# = 1 -a $1 = "-d" ]; then
	cleanup
else
	usage
fi

