#!/bin/bash

IMAGE_SIZE="6G"

source common.sh

##
# 70 disk, gpt table, partition * 100
function test_70()
{
	local IMAGE
	local LOOP

	echo -n "$FUNCNAME: "

	IMAGE="$(create_image $FUNCNAME)"
	[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

	gpt_init "$IMAGE"
	[ $? = 0 ] || error || return

	gpt_create $IMAGE 1 49

	for offset in {50..4950..50}; do
		gpt_create $IMAGE $offset 50
	done

	LOOP="$(create_loop $IMAGE)"
	[ -n "$LOOP" -a -b "$LOOP" ] || error || return

	for num in {1..100}; do
		[ -b "${LOOP}p$num" ] || error || return
	done

	#gpt_print $LOOP

	ok "$LOOP"
}


if [ $# = 0 ]; then
	cleanup
	test_70
elif [ $# = 1 -a $1 = "-d" ]; then
	cleanup
else
	usage
fi

