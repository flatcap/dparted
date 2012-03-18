#!/bin/bash

IMAGE_SIZE="5G"
IMAGE_DIR="."

source common.sh

##
# disk, table, partition, lvm table, empty
function test_60()
{
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	msdos_init "$LOOP"
	[ $? == 0 ] || error || return

	msdos_create $LOOP primary 1 500
	[ $? == 0 ] || error || return

	ok "$LOOP"
}

##
# disk, table, partition, lvm table, partition, empty
function test_61()
{
	echo -n "$FUNCNAME: "
	error
}

##
# disk, table, partition, lvm table, partition, unknown
function test_62()
{
	echo -n "$FUNCNAME: "
	error
}

##
# disk, table, partition, lvm table, partition, filesystem
function test_63()
{
	echo -n "$FUNCNAME: "
	error
}

##
# disk, table, partition, lvm table, (partition, filesystem) * 8
# (include empty space)
function test_64()
{
	echo -n "$FUNCNAME: "
	error
}


cleanup

test_60
test_61
test_62
test_63
test_64

