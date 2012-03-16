#!/bin/bash

IMAGE17="disk17"
LOOP17="/dev/loop17"

losetup -d $LOOP17

rm -f $IMAGE17

