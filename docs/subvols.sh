#!/bin/bash

mkdir p
mount /dev/loop3p1 p
btrfs subvolume list p
btrfs subvolume get-default p
umount p
rmdir p

