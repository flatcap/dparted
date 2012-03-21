#!/bin/bash

IMAGE_SIZE="5G"

source common.sh

##
# 10 disk, empty
function test_10()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return

	ok $LOOP
}

##
# 11 disk, unknown
function test_11()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return

	dd if=/dev/urandom bs=32K count=1 of="$LOOP" 2> /dev/null

	ok "$LOOP"
}

##
# 12 disk, filesystem
function test_12()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return

	mke2fs -t ext4 -q "$LOOP"
	[ $? = 0 ] || error || return

	ok "$LOOP"
}


if [ $# = 0 ]; then
	cleanup
	test_10
	test_11
	test_12
elif [ $# = 1 -a $1 = "-d" ]; then
	cleanup
else
	usage
fi

