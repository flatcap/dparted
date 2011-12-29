#!/bin/bash

IMAGE4="disk4"

source common.sh

rm -f $IMAGE4
fallocate --length 1G $IMAGE4

cat <<-EOF | parted $IMAGE4
	mklabel gpt

	unit s

	mkpart primary     2048   204799
	mkpart primary   204800   409599
	mkpart primary   409600  1023999
	mkpart primary  1024000  1638399
	mkpart primary  1638400  2097118

	name 1 first
	name 2 second
	name 3 third
	name 4 fourth
	name 5 fifth

	print free

	quit
EOF

losetup /dev/loop4 $IMAGE4 --offset $((512*2048)) --sizelimit $((512*(204800-2048)))
mke2fs -t ext4 -L first_fs /dev/loop4
populate_ext4 /dev/loop4 20M
sync
losetup -d /dev/loop4

losetup /dev/loop4 $IMAGE4 --offset $((512*204800)) --sizelimit $((512*(409600-204800)))
mke2fs -t ext4 -L second_fs /dev/loop4
populate_ext4 /dev/loop4 60M
sync
losetup -d /dev/loop4

losetup /dev/loop4 $IMAGE4 --offset $((512*409600)) --sizelimit $((512*(1024000-409600)))
mke2fs -t ext4 -L third_fs /dev/loop4
populate_ext4 /dev/loop4 180M
sync
losetup -d /dev/loop4

losetup /dev/loop4 $IMAGE4 --offset $((512*1024000)) --sizelimit $((512*(1638400-1024000)))
mke2fs -t ext4 -L fourth_fs /dev/loop4
populate_ext4 /dev/loop4 90M
sync
losetup -d /dev/loop4

losetup /dev/loop4 $IMAGE4 --offset $((512*1638400)) --sizelimit $((512*(2097119-1638400)))
mke2fs -t ext4 -L fifth_fs /dev/loop4
populate_ext4 /dev/loop4 180M
sync
losetup -d /dev/loop4

losetup /dev/loop4 $IMAGE4

rm -f $IMAGE4

