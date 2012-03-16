#!/bin/bash

FILE4="disk4"
LOOP4="/dev/loop4"
COUNT=0

source common.sh

rm -f $FILE4
fallocate --length 1G $FILE4

gpt_init $FILE4

#           loop    disk    part  disk off size filesystem fill
#           device  image   label     MiB  MiB  label       MiB
gpt_create  $LOOP4  $FILE4  first       1   99  first_fs     20
gpt_create  $LOOP4  $FILE4  second    100  100  second_fs    60
gpt_create  $LOOP4  $FILE4  third     200  300  third_fs    180
gpt_create  $LOOP4  $FILE4  fourth    500  300  fourth_fs    90
gpt_create  $LOOP4  $FILE4  fifth     800  220  fifth_fs    180

gpt_print $FILE4

losetup /dev/loop4 $FILE4
rm -f $FILE4

