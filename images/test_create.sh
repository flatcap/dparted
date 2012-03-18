#!/bin/bash

IMAGE_SIZE="5G"
IMAGE_DIR="."

##
# Print OK in green
function ok()
{
	echo -e "\e[01;32mOK\e[0m $@"
}

##
# Print Error in red
function error()
{
	echo -e "\e[01;31mError\e[0m"
	false
}


##
# remove loops and images from previous runs
function cleanup()
{
	local LOOPS="$(losetup -a | cut -d: -f1 | tac)"
	if [ -n "$LOOPS" ]; then
		for i in $LOOPS; do
			sync
			losetup -d $i
		done
	fi

	rm -f test_*.img
}

##
# Create a sparse image, then create a loop device from it
function create_loop()
{
	local FUNC="$1"
	local INDEX
	local LOOP
	local IMAGE

	[ -z "$1"    ] && return
	[ -z "$2"    ] || return

	INDEX=${FUNC##*_}
	LOOP="/dev/loop$INDEX"
	IMAGE="$FUNC.img"

	[ -b "$LOOP"       ] || return
	[ -d "$IMAGE_DIR"  ] || return
	[ -n "$IMAGE_SIZE" ] || return

	truncate -s "$IMAGE_SIZE" "$IMAGE"
	[ $? = 0 ] || return

	losetup "$LOOP" "$IMAGE"
	[ $? = 0 ] || return

	rm -f "$IMAGE"
	echo "$LOOP"
}

##
# Create a small subset of a loop device
function sub_loop()
{
	local LOOP
	local START
	local SIZE

	[ -n "$3" ] || return
	[ -z "$4" ] || return
	[ -b "$1" ] || return

	LOOP="$1"
	START="$2"
	SIZE="$3"

	START=$((START*1048576))
	SIZE=$((SIZE*1048576))

	losetup --show -f $LOOP --offset $START --sizelimit $SIZE
}


function msdos_init()
{
	[ -n "$1" ] || return
	[ -z "$2" ] || return
	[ -b "$1" ] || return

	cat <<-EOF | parted $1 > /dev/null
		mklabel msdos
		quit
	EOF
}

function msdos_print()
{
	[ -n "$1" ] || return
	[ -z "$2" ] || return
	[ -b "$1" ] || return

	cat <<-EOF | parted $1
		unit s
		print free
		quit
	EOF
}

function msdos_create()
{
	local LOOP
	local TYPE
	local START
	local SIZE
	local END

	[ -n "$4" ] || return
	[ -z "$5" ] || return
	[ -b "$1" ] || return

	LOOP="$1"
	TYPE="$2"
	START="$3"
	SIZE="$4"

	END="$(((START+SIZE)*2048))"
	START=$((START*2048))

	cat <<-EOF | parted $LOOP > /dev/null
		unit s
		mkpart $TYPE $START $((END-1))
		quit
	EOF
}


function gpt_init()
{
	[ -n "$1" ] || return
	[ -z "$2" ] || return
	[ -b "$1" ] || return

	cat <<-EOF | parted $1 > /dev/null
		mklabel gpt
		quit
	EOF
}

function gpt_print()
{
	[ -n "$1" ] || return
	[ -z "$2" ] || return
	[ -b "$1" ] || return

	cat <<-EOF | parted $1
		unit s
		print free
		quit
	EOF
}

function gpt_create()
{
	local LOOP
	local START
	local SIZE
	local END

	[ -n "$3" ] || return
	[ -z "$4" ] || return
	[ -b "$1" ] || return

	LOOP="$1"
	START="$2"
	SIZE="$3"
	END="$((START+SIZE))"

	START=$((START*2048))
	SIZE=$((SIZE*2048))
	END=$((END*2048))

	cat <<-EOF | parted $LOOP > /dev/null
		unit s
		mkpart primary $START $((END-1))
		quit
	EOF
}


##
# disk, empty
function test_10()
{
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	ok $LOOP
}

##
# disk, unknown
function test_11()
{
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	dd if=/dev/urandom bs=16K count=1 of="$LOOP" 2> /dev/null

	ok "$LOOP"
}

##
# disk, filesystem
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


##
# disk, msdos table, empty
function test_20()
{
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	msdos_init "$LOOP"
	[ $? == 0 ] || error || return

	ok "$LOOP"
}

##
# disk, msdos table, partition, empty
function test_21()
{
	local LOOP
	local NUM

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
# disk, msdos table, partition, unknown
function test_22()
{
	local LOOP
	local SUB

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	msdos_init "$LOOP"
	[ $? == 0 ] || error || return

	msdos_create $LOOP primary 1 500
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 1 500)"
	[ $? == 0 ] || error || return

	dd if=/dev/urandom bs=16K count=1 of="$SUB" 2> /dev/null

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return

	ok "$LOOP"
}

##
# disk, msdos table, partition, filesystem
function test_23()
{
	local LOOP
	local SUB

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	msdos_init "$LOOP"
	[ $? == 0 ] || error || return

	msdos_create $LOOP primary 1 500
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 1 500)"
	[ $? == 0 ] || error || return
	#XXX check SUB is non-empty, and a block device too

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return

	ok "$LOOP"
}

##
# disk, msdos table, (partition, filesystem) * 4
# (include empty space)
function test_24()
{
	local LOOP
	local SUB

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	msdos_init "$LOOP"
	[ $? == 0 ] || error || return
# FS1
	msdos_create $LOOP primary 1 500
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 1 500)"
	[ $? == 0 ] || error || return
	losetup -a | grep $SUB

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return
# FS2
	msdos_create $LOOP primary 600 500
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 600 500)"
	[ $? == 0 ] || error || return
	losetup -a | grep $SUB

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return
# FS3
	msdos_create $LOOP primary 2000 1000
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 2000 1000)"
	[ $? == 0 ] || error || return
	losetup -a | grep $SUB

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return
# FS4
	msdos_create $LOOP primary 3500 200
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 3500 200)"
	[ $? == 0 ] || error || return
	losetup -a | grep $SUB

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return

	ok "$LOOP"
}


##
# disk, msdos table, extended partition, empty
function test_30()
{
	local LOOP
	local NUM

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	msdos_init "$LOOP"
	[ $? == 0 ] || error || return

	msdos_create $LOOP extended 500 3500
	[ $? == 0 ] || error || return

	ok "$LOOP"
}

##
# disk, msdos table, extended partition, partition, empty
function test_31()
{
	local LOOP
	local NUM

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	msdos_init "$LOOP"
	[ $? == 0 ] || error || return

	msdos_create $LOOP extended 500 3500
	[ $? == 0 ] || error || return

	msdos_create $LOOP logical 1000 1500
	[ $? == 0 ] || error || return

	ok "$LOOP"
}

##
# disk, msdos table, extended partition, partition, unknown
function test_32()
{
	local LOOP
	local NUM

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	msdos_init "$LOOP"
	[ $? == 0 ] || error || return

	msdos_create $LOOP extended 500 3500
	[ $? == 0 ] || error || return

	msdos_create $LOOP logical 1000 1500
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 1000 1500)"
	[ $? == 0 ] || error || return

	dd if=/dev/urandom bs=16K count=1 of="$SUB" 2> /dev/null

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return

	ok "$LOOP"
}

##
# disk, msdos table, extended partition, partition, filesystem
function test_33()
{
	local LOOP
	local NUM

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	msdos_init "$LOOP"
	[ $? == 0 ] || error || return

	msdos_create $LOOP extended 500 3500
	[ $? == 0 ] || error || return

	msdos_create $LOOP logical 1000 1500
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 1000 1500)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return

	ok "$LOOP"
}

##
# disk, msdos table, extended partition, (partition, filesystem) * 8
# (include empty space)
function test_34()
{
	local LOOP
	local NUM

	echo -n "$FUNCNAME: "
	echo

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	msdos_init "$LOOP"
	[ $? == 0 ] || error || return

	msdos_create $LOOP extended 500 4000
	[ $? == 0 ] || error || return

# FS1
	msdos_create $LOOP logical 600 200
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 600 200)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return
# FS2
	msdos_create $LOOP logical 1000 499
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 1000 499)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return
# FS3
	msdos_create $LOOP logical 1500 500
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 1500 500)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return
# FS4
	msdos_create $LOOP logical 2500 199
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 2500 199)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return
# FS5
	msdos_create $LOOP logical 2700 200
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 2700 200)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return
# FS6
	msdos_create $LOOP logical 3000 199
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 3000 199)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return
# FS7
	msdos_create $LOOP logical 3200 400
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 3200 400)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return
# FS8
	msdos_create $LOOP logical 3800 200
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 3800 200)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return

	ok "$LOOP"
}


##
# disk, protective table, partition, gpt table, empty
function test_40()
{
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	gpt_init "$LOOP"
	[ $? == 0 ] || error || return

	ok "$LOOP"
}

##
# disk, protective table, partition, gpt table, partition, empty
function test_41()
{
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	gpt_init "$LOOP"
	[ $? == 0 ] || error || return

	gpt_create $LOOP 1 500
	[ $? == 0 ] || error || return

	ok "$LOOP"
}

##
# disk, protective table, partition, gpt table, partition, unknown
function test_42()
{
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	gpt_init "$LOOP"
	[ $? == 0 ] || error || return

	gpt_create $LOOP 1 500
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 1 500)"
	[ $? == 0 ] || error || return

	dd if=/dev/urandom bs=16K count=1 of="$SUB" 2> /dev/null

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return

	ok "$LOOP"
}

##
# disk, protective table, partition, gpt table, partition, filesystem
function test_43()
{
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	gpt_init "$LOOP"
	[ $? == 0 ] || error || return

	gpt_create $LOOP 1 500
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 1 500)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return

	ok "$LOOP"
}

##
# disk, protective table, partition, gpt table, (partition, filesystem) * 8
# (include empty space)
function test_44()
{
	local LOOP

	echo -n "$FUNCNAME: "

	LOOP="$(create_loop $FUNCNAME)"
	[ -n "$LOOP" ] || error || return

	gpt_init "$LOOP"
	[ $? == 0 ] || error || return

# FS1 ---------------------------------------------
	gpt_create $LOOP 600 200
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 600 200)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return

# FS2 ---------------------------------------------
	gpt_create $LOOP 1000 499
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 1000 499)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return

# FS3 ---------------------------------------------
	gpt_create $LOOP 1500 500
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 1500 500)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return

# FS4 ---------------------------------------------
	gpt_create $LOOP 2500 199
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 2500 199)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return

# FS5 ---------------------------------------------
	gpt_create $LOOP 2700 200
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 2700 200)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return

# FS6 ---------------------------------------------
	gpt_create $LOOP 3000 199
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 3000 199)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return

# FS7 ---------------------------------------------
	gpt_create $LOOP 3200 400
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 3200 400)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return

# FS8 ---------------------------------------------
	gpt_create $LOOP 3800 200
	[ $? == 0 ] || error || return

	SUB="$(sub_loop $LOOP 3800 200)"
	[ $? == 0 ] || error || return

	mke2fs -t ext4 -q "$SUB"
	[ $? == 0 ] || error || return

	sync
	losetup -d $SUB
	[ $? == 0 ] || error || return

	ok "$LOOP"
}


##
# disk, table, partition, lvm table, empty
function test_50()
{
	local LOOP
	local NUM

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
# disk, table, partition, lvm table, unknown
function test_51()
{
	echo -n "$FUNCNAME: "
	error
}

##
# disk, table, partition, lvm table, partition, empty
function test_52()
{
	echo -n "$FUNCNAME: "
	error
}

##
# disk, table, partition, lvm table, partition, unknown
function test_53()
{
	echo -n "$FUNCNAME: "
	error
}

##
# disk, table, partition, lvm table, partition, filesystem
function test_54()
{
	echo -n "$FUNCNAME: "
	error
}

##
# disk, table, partition, lvm table, (partition, filesystem) * 8
# (include empty space)
function test_55()
{
	echo -n "$FUNCNAME: "
	error
}


test_50
echo
exit

##
# disk, lvm table, empty
function test_60()
{
	echo -n "$FUNCNAME: "
	error
}

##
# disk, lvm table, unknown
function test_61()
{
	echo -n "$FUNCNAME: "
	error
}

##
# disk, lvm table, partition, empty
function test_62()
{
	echo -n "$FUNCNAME: "
	error
}

##
# disk, lvm table, partition, unknown
function test_63()
{
	echo -n "$FUNCNAME: "
	error
}

##
# disk, lvm table, partition, filesystem
function test_64()
{
	echo -n "$FUNCNAME: "
	error
}

##
# disk, lvm table, (partition, filesystem) * 8
# (include empty space)
function test_65()
{
	echo -n "$FUNCNAME: "
	error
}


function test_other()
{
#	test different filesystem types
#		one of each type - ext2, ext3, ext4, reiser, btrfs, jfs, xfs, swap, fat, fat32, ntfs
#		space before and after each partition
#		able to
#			resize
#			move
#			create
#			delete
#			format
#			info
#			check
#			label
#			uuid
#
#	test different lvm configurations
#		linear
#			1 segment
#			many segments
#				add segment
#				remove segment
#		stripe
#			2 stripes
#			many stripes
#				add stripe
#				remove stripe
#				add mirror
#		mirror
#			2 mirrors
#			many mirrors
#				add mirror
#				remove mirror
#				break mirror
#		raid10 (mirrored stripes)
#			2 mirrors
#			many mirrors
#				add mirror
#				remove mirror
#			2 stripes
#			many stripes
#				add stripe
#				remove stripe
#
#	test different tables
#		empty disk
#		create table
#			msdos
#			gpt
#			lvm
#		convert one table type to another
#
#	misc
#		filesystem smaller than the partition
#			display empty space
#			allow enlarging filesystem
#		filesystem larger  than the partition!
#			whole disk readonly
#			replace block icon with STOP sign
}


exit

cleanup

test_10
test_11
test_12
echo

test_20
test_21
test_22
test_23
test_24
echo

test_30
test_31
test_32
test_33
test_34
echo

test_40
test_41
test_42
test_43
test_44
echo

