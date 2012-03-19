#!/bin/bash

IMAGE_SIZE="5G"
IMAGE_DIR="."

source common.sh

##
# 10 disk, empty
function test_10()
{
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	ok $LOOP
}

##
# 11 disk, unknown
function test_11()
{
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	dd if=/dev/urandom bs=32K count=1 of="$LOOP" 2> /dev/null

	ok "$LOOP"
}

##
# 12 disk, filesystem
function test_12()
{
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	mke2fs -t ext4 -q "$LOOP"
	[ $? == 0 ] || error || return

	ok "$LOOP"
}


cleanup

test_10
test_11
test_12

