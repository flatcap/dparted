#!/bin/bash

IMAGE_SIZE="5G"

source common.sh

##
# 80 disk, lvm table, volume group, selection of volumes
function test_80()
{
	local IMAGE
	local LOOP
	local LOOP_LIST
	local VOLUME=$FUNCNAME

	echo -n "$FUNCNAME: "

	for i in {0..2}; do
		IMAGE="$(create_image ${FUNCNAME}$i)"
		[ -n "$IMAGE" -a -f "$IMAGE" ] || error || return

		LOOP="$(create_loop $IMAGE)"
		[ -n "$LOOP" -a -b "$LOOP" ] || error || return

		pvcreate "$LOOP" &> /dev/null
		[ $? = 0 ] || error || return

		LOOP_LIST="$LOOP_LIST $LOOP"
	done

	vgcreate $VOLUME $LOOP_LIST > /dev/null;			[ $? = 0 ] || error || return

	lvcreate -i2 --size 400m --name stripe2 $VOLUME > /dev/null;	[ $? = 0 ] || error || return
	lvcreate     --size 200m --name root    $VOLUME > /dev/null;	[ $? = 0 ] || error || return
	lvcreate     --size  52m --name junk    $VOLUME > /dev/null;	[ $? = 0 ] || error || return
	lvcreate -m2 --size 200m --name mirror3 $VOLUME > /dev/null;	[ $? = 0 ] || error || return
	lvcreate -i3 --size 300m --name stripe3 $VOLUME > /dev/null;	[ $? = 0 ] || error || return
	lvcreate     --size 352m --name home    $VOLUME > /dev/null;	[ $? = 0 ] || error || return
	lvcreate     --size 400m --name tv      $VOLUME > /dev/null;	[ $? = 0 ] || error || return
	lvcreate -m1 --size 200m --name mirror2 $VOLUME > /dev/null;	[ $? = 0 ] || error || return
	lvcreate     --size 300m --name data    $VOLUME > /dev/null;	[ $? = 0 ] || error || return

	lvremove -f /dev/$VOLUME/junk > /dev/null;					[ $? = 0 ] || error || return

	mke2fs -q -t ext4 -L "$VOLUME stripe2" /dev/$VOLUME/stripe2 > /dev/null;	[ $? = 0 ] || error || return
	mke2fs -q -t ext4 -L "$VOLUME root"    /dev/$VOLUME/root > /dev/null;		[ $? = 0 ] || error || return
	mkntfs -Q         -L "$VOLUME mirror3" /dev/$VOLUME/mirror3 > /dev/null;	[ $? = 0 ] || error || return
	mke2fs -q -t ext4 -L "$VOLUME home"    /dev/$VOLUME/home > /dev/null;		[ $? = 0 ] || error || return
	mkntfs -Q         -L "$VOLUME stripe3" /dev/$VOLUME/stripe3 > /dev/null;	[ $? = 0 ] || error || return
	mkntfs -Q         -L "$VOLUME tv"      /dev/$VOLUME/tv > /dev/null;		[ $? = 0 ] || error || return
	mke2fs -q -t ext4 -L "$VOLUME mirror2" /dev/$VOLUME/mirror2 > /dev/null;	[ $? = 0 ] || error || return
	mke2fs -q -t ext4 -L "$VOLUME data"    /dev/$VOLUME/data > /dev/null;		[ $? = 0 ] || error || return

	#lvs --all -o lv_name,vg_name,lv_attr,lv_size,lv_uuid,lv_path,lv_kernel_major,lv_kernel_minor,seg_count $VOLUME

	populate_ext4 /dev/$VOLUME/stripe2 320M &> /dev/null;			[ $? = 0 ] || error || return # 80% of 400
	populate_ext4 /dev/$VOLUME/root     70M &> /dev/null;			[ $? = 0 ] || error || return # 35% of 200
	populate      /dev/$VOLUME/mirror3 150  &> /dev/null;			[ $? = 0 ] || error || return # 75% of 200
	populate_ext4 /dev/$VOLUME/home    150M &> /dev/null;			[ $? = 0 ] || error || return # 50% of 300
	populate      /dev/$VOLUME/stripe3 240  &> /dev/null;			[ $? = 0 ] || error || return # 60% of 400
	populate      /dev/$VOLUME/tv      220  &> /dev/null;			[ $? = 0 ] || error || return # 55% of 400
	populate_ext4 /dev/$VOLUME/mirror2  20M &> /dev/null;			[ $? = 0 ] || error || return # 10% of 200
	populate_ext4 /dev/$VOLUME/data    240M &> /dev/null;			[ $? = 0 ] || error || return # 80% of 300

	ok $LOOP_LIST
}


if [ $# = 0 ]; then
	cleanup
	test_80
elif [ $# = 1 -a $1 = "-d" ]; then
	cleanup
else
	usage
fi

