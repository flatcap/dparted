#!/bin/bash

IMAGE0="disk0"
IMAGE1="disk1"
IMAGE2="disk2"
VOLUME="tmpvol"

lvremove -f /dev/tmpvol/*
vgremove -f tmpvol

losetup -d /dev/loop0
losetup -d /dev/loop1
losetup -d /dev/loop2

rm -f $IMAGE0
rm -f $IMAGE1
rm -f $IMAGE2
