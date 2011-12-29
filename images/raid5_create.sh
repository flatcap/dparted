#!/bin/bash

source common.sh

fallocate -l 1G disk5
fallocate -l 1G disk6
fallocate -l 1G disk7

losetup /dev/loop5 disk5
losetup /dev/loop6 disk6
losetup /dev/loop7 disk7

mdadm --create /dev/md0 --level=5 --raid-devices=3 /dev/loop{5,6,7}

rm -f disk{5,6,7}

mke2fs -t ext4 -L md_raid /dev/md0

populate_ext4 /dev/md0 1200M

