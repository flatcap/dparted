#!/bin/bash

VOLUME="bigvol"

source common.sh

for i in $(enum 8 15); do
	fallocate -l 1G disk$i
	losetup /dev/loop$i disk$i
	rm -f disk$i
done

pvcreate /dev/loop{8,9,10,11,12,13,14,15}

vgcreate $VOLUME /dev/loop{8,9,10,11,12,13,14,15}

lvcreate $VOLUME -i 2 -m 3 --name bob --size 320M

mke2fs -t ext4 -L "$VOLUME bob" /dev/$VOLUME/bob

populate_ext4 /dev/$VOLUME/bob 250M

