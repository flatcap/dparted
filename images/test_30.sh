#!/bin/bash

IMAGE_SIZE="5G"

source common.sh

##
# 30 disk, msdos table, extended partition, empty
function test_30()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	msdos_init "$IMAGE"
	[ $? = 0 ] || error || return

	msdos_create $IMAGE extended 500 3500	# extended partition 1
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return

	ok "$LOOP"
}

##
# 31 disk, msdos table, extended partition, partition, empty
function test_31()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	msdos_init "$IMAGE"
	[ $? = 0 ] || error || return

	msdos_create $IMAGE extended 500 3500	# extended partition 1
	[ $? = 0 ] || error || return

	msdos_create $IMAGE logical 1000 1500	# logical partition 5
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return
	[ -b "${LOOP}p5" ] || error || return

	ok "$LOOP"
}

##
# 32 disk, msdos table, extended partition, partition, unknown
function test_32()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	msdos_init "$IMAGE"
	[ $? = 0 ] || error || return

	msdos_create $IMAGE extended 500 3500	# extended partition
	[ $? = 0 ] || error || return

	msdos_create $IMAGE logical 1000 1500	# logical partition 5
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p5" ] || error || return

	dd if=/dev/urandom bs=32K count=1 of="${LOOP}p5" 2> /dev/null

	ok "$LOOP"
}

##
# 33 disk, msdos table, extended partition, partition, filesystem
function test_33()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	msdos_init "$IMAGE"
	[ $? = 0 ] || error || return

	msdos_create $IMAGE extended 500 3500	# extended partition 1
	[ $? = 0 ] || error || return

	msdos_create $IMAGE logical 1000 1500	# logical partition 5
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1" ] || error || return
	[ -b "${LOOP}p5" ] || error || return

	mke2fs -t ext4 -q "${LOOP}p5"		# filesystem 1
	[ $? = 0 ] || error || return

	ok "$LOOP"
}

##
# 34 disk, msdos table, extended partition, (partition, filesystem) * 8
# (include empty space)
function test_34()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	msdos_init "$IMAGE"
	[ $? = 0 ] || error || return

	msdos_create $IMAGE extended 500 4000	# extended partition 1
	[ $? = 0 ] || error || return

	msdos_create $IMAGE logical 600 200	# logical partition 5
	[ $? = 0 ] || error || return

	msdos_create $IMAGE logical 1000 499	# logical partition 6
	[ $? = 0 ] || error || return

	msdos_create $IMAGE logical 1500 500	# logical partition 7
	[ $? = 0 ] || error || return

	msdos_create $IMAGE logical 2500 199	# logical partition 8
	[ $? = 0 ] || error || return

	msdos_create $IMAGE logical 2700 200	# logical partition 9
	[ $? = 0 ] || error || return

	msdos_create $IMAGE logical 3000 199	# logical partition 10
	[ $? = 0 ] || error || return

	msdos_create $IMAGE logical 3200 400	# logical partition 11
	[ $? = 0 ] || error || return

	msdos_create $IMAGE logical 3800 200	# logical partition 12
	[ $? = 0 ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return
	[ -b "${LOOP}p1"  ] || error || return
	[ -b "${LOOP}p5"  ] || error || return
	[ -b "${LOOP}p6"  ] || error || return
	[ -b "${LOOP}p7"  ] || error || return
	[ -b "${LOOP}p8"  ] || error || return
	[ -b "${LOOP}p9"  ] || error || return
	[ -b "${LOOP}p10" ] || error || return
	[ -b "${LOOP}p11" ] || error || return
	[ -b "${LOOP}p12" ] || error || return

	mke2fs -t ext4 -q "${LOOP}p5"		# filesystem 1
	[ $? = 0 ] || error || return

	mke2fs -t ext4 -q "${LOOP}p6"		# filesystem 2
	[ $? = 0 ] || error || return

	mke2fs -t ext4 -q "${LOOP}p7"		# filesystem 3
	[ $? = 0 ] || error || return

	mke2fs -t ext4 -q "${LOOP}p8"		# filesystem 4
	[ $? = 0 ] || error || return

	mke2fs -t ext4 -q "${LOOP}p9"		# filesystem 5
	[ $? = 0 ] || error || return

	mke2fs -t ext4 -q "${LOOP}p10"		# filesystem 6
	[ $? = 0 ] || error || return

	mke2fs -t ext4 -q "${LOOP}p11"		# filesystem 7
	[ $? = 0 ] || error || return

	mke2fs -t ext4 -q "${LOOP}p12"		# filesystem 8
	[ $? = 0 ] || error || return

	ok "$LOOP"
}


if [ $# = 0 ]; then
	cleanup
	test_30
	test_31
	test_32
	test_33
	test_34
elif [ $# = 1 -a $1 = "-d" ]; then
	cleanup
else
	usage
fi

