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

	msdos_create $LOOP primary 500 4000
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 1000 1500)"
	[ $? == 0 ] || error || return

	pvcreate "$SUB" #> /dev/null 2>&1
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
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
	SUB="$(sub_loop $LOOP 1000 1500)"
	[ $? == 0 ] || error || return

	dd if=/dev/urandom bs=32K count=1 of="$SUB" 2> /dev/null

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return

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
exit
test_61
test_62
test_63
test_64

