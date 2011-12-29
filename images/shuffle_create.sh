#!/bin/bash

IMAGE3="disk3"
VOLUME="shuffle"

source common.sh

rm -f $IMAGE3

fallocate --length 200M $IMAGE3

losetup /dev/loop3 $IMAGE3

pvcreate /dev/loop3

vgcreate $VOLUME /dev/loop3

lvcreate --size 52m --name data  $VOLUME
lvcreate --size 52m --name junk1 $VOLUME
lvcreate --size 52m --name junk2 $VOLUME
lvcreate --size 40m --name junk3 $VOLUME

lvremove -f /dev/$VOLUME/junk2
lvextend --size +52m /dev/$VOLUME/data
lvremove -f /dev/$VOLUME/junk1
lvextend --size +52m /dev/$VOLUME/data
lvremove -f /dev/$VOLUME/junk3
lvextend --size +40m /dev/$VOLUME/data

mke2fs -t ext4 -L shuffle_data /dev/$VOLUME/data

lvs --all -o lv_name,vg_name,lv_attr,lv_size,lv_uuid,lv_path,lv_kernel_major,lv_kernel_minor,seg_count $VOLUME

populate_ext4 /dev/$VOLUME/data 100M

rm -f $IMAGE3

