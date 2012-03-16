#!/bin/bash

FILE17="disk17"
LOOP17="/dev/loop17"
COUNT=1

source common.sh

rm -f $FILE17
fallocate --length 4G $FILE17

gpt_init $FILE17

#           loop     disk     part      disk off size filesystem     fill
#           device   image    label         MiB  MiB  label           MiB
gpt_create  $LOOP17  $FILE17  first           1   99  first_fs         20
gpt_create  $LOOP17  $FILE17  second        100  100  second_fs        60
gpt_create  $LOOP17  $FILE17  third         200  300  third_fs        180
gpt_create  $LOOP17  $FILE17  fourth        500  300  fourth_fs        90
gpt_create  $LOOP17  $FILE17  fifth         800  224  fifth_fs        180
gpt_create  $LOOP17  $FILE17  sixth        1024  200  sixth_fs         40
gpt_create  $LOOP17  $FILE17  seventh      1224  400  seventh_fs      360
gpt_create  $LOOP17  $FILE17  eighth       1624  100  eighth_fs        10
gpt_create  $LOOP17  $FILE17  ninth        1774   50  ninth_fs          1
gpt_create  $LOOP17  $FILE17  tenth        1824  150  tenth_fs         20
gpt_create  $LOOP17  $FILE17  eleventh     1974  150  eleventh_fs      20
gpt_create  $LOOP17  $FILE17  twelfth      2124  150  twelfth_fs       60
gpt_create  $LOOP17  $FILE17  thirteenth   2274   50  thirteenth_fs    20
gpt_create  $LOOP17  $FILE17  fourteenth   2324   50  fourteenth_fs    10
gpt_create  $LOOP17  $FILE17  fifteenth    2374   50  fifteenth_fs      5
gpt_create  $LOOP17  $FILE17  sixteenth    2424  800  sixteenth_fs    600
gpt_create  $LOOP17  $FILE17  seventeenth  3274  100  seventeenth_fs   30
gpt_create  $LOOP17  $FILE17  eighteenth   3374   50  eighteenth_fs    10
gpt_create  $LOOP17  $FILE17  nineteenth   3424  250  nineteenth_fs   200
gpt_create  $LOOP17  $FILE17  twentieth    3674  100  twentieth_fs     20

gpt_print $FILE17

losetup $LOOP17 $FILE17
rm -f $FILE17

