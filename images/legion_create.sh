#!/bin/bash

FILE17="disk17"
LOOP17="/dev/loop17"

source common.sh

rm -f $FILE17
losetup -d $LOOP17 >& /dev/null
fallocate --length 6G $FILE17
losetup $LOOP17 $FILE17

gpt_init $LOOP17

gpt_create  $LOOP17     1  49

for offset in {50..4950..50}; do
	gpt_create $LOOP17 $offset 50
done

gpt_print $LOOP17

rm -f $FILE17

