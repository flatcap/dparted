#!/bin/bash

IMAGE3="disk3"
VOLUME="shuffle"

lvremove -f /dev/$VOLUME/*
vgremove -f $VOLUME

losetup -d /dev/loop3

rm -f $IMAGE3

