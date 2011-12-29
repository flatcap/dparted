#!/bin/bash

IMAGE0="disk0"
IMAGE1="disk1"
IMAGE2="disk2"
VOLUME="tmpvol"

source common.sh

rm -f $IMAGE0
rm -f $IMAGE1
rm -f $IMAGE2

fallocate --length 1G $IMAGE0
fallocate --length 1G $IMAGE1
fallocate --length 1G $IMAGE2

losetup /dev/loop0 $IMAGE0
losetup /dev/loop1 $IMAGE1
losetup /dev/loop2 $IMAGE2

pvcreate /dev/loop0
pvcreate /dev/loop1
pvcreate /dev/loop2

vgcreate $VOLUME /dev/loop0 /dev/loop1 /dev/loop2

lvcreate -i2 --size 400m --name stripe2 $VOLUME
lvcreate     --size 200m --name root    $VOLUME
lvcreate     --size  52m --name junk    $VOLUME
lvcreate -m2 --size 200m --name mirror3 $VOLUME
lvcreate -i3 --size 300m --name stripe3 $VOLUME
lvcreate     --size 352m --name home    $VOLUME
lvcreate     --size 400m --name tv      $VOLUME
lvcreate -m1 --size 200m --name mirror2 $VOLUME
lvcreate     --size 300m --name data    $VOLUME

lvremove -f /dev/$VOLUME/junk

mke2fs -t ext4 -L "$VOLUME stripe2" /dev/$VOLUME/stripe2
mke2fs -t ext4 -L "$VOLUME root"    /dev/$VOLUME/root
mkntfs -Q      -L "$VOLUME mirror3" /dev/$VOLUME/mirror3
mke2fs -t ext4 -L "$VOLUME home"    /dev/$VOLUME/home
mkntfs -Q      -L "$VOLUME stripe3" /dev/$VOLUME/stripe3
mkntfs -Q      -L "$VOLUME tv"      /dev/$VOLUME/tv
mke2fs -t ext4 -L "$VOLUME mirror2" /dev/$VOLUME/mirror2
mke2fs -t ext4 -L "$VOLUME data"    /dev/$VOLUME/data

lvs --all -o lv_name,vg_name,lv_attr,lv_size,lv_uuid,lv_path,lv_kernel_major,lv_kernel_minor,seg_count $VOLUME

populate_ext4 /dev/$VOLUME/stripe2 320M # 80% of 400
populate_ext4 /dev/$VOLUME/root     70M # 35% of 200
populate      /dev/$VOLUME/mirror3 150  # 75% of 200
populate_ext4 /dev/$VOLUME/home    150M # 50% of 300
populate      /dev/$VOLUME/stripe3 240  # 60% of 400
populate      /dev/$VOLUME/tv      220  # 55% of 400
populate_ext4 /dev/$VOLUME/mirror2  20M # 10% of 200
populate_ext4 /dev/$VOLUME/data    240M # 80% of 300

rm -f $IMAGE0
rm -f $IMAGE1
rm -f $IMAGE2

