#!/bin/bash

VOLUME="bigvol"

lvremove -f /dev/$VOLUME/*
vgremove -f $VOLUME

for i in $(enum 8 15); do
	losetup -d /dev/loop$i
	rm -f disk$i
done

