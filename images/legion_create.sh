#!/bin/bash

FILE17="disk17"
LOOP17="/dev/loop17"
COUNT=1

source common.sh

rm -f $FILE17
fallocate --length 6G $FILE17

gpt_init $FILE17

gpt_create  $LOOP17  $FILE17  a     1  49  a  1
gpt_create  $LOOP17  $FILE17  a    50  50  a  1
gpt_create  $LOOP17  $FILE17  a   100  50  a  1
gpt_create  $LOOP17  $FILE17  a   150  50  a  1
gpt_create  $LOOP17  $FILE17  a   200  50  a  1
gpt_create  $LOOP17  $FILE17  a   250  50  a  1
gpt_create  $LOOP17  $FILE17  a   300  50  a  1
gpt_create  $LOOP17  $FILE17  a   350  50  a  1
gpt_create  $LOOP17  $FILE17  a   400  50  a  1
gpt_create  $LOOP17  $FILE17  a   450  50  a  1
gpt_create  $LOOP17  $FILE17  a   500  50  a  1
gpt_create  $LOOP17  $FILE17  a   550  50  a  1
gpt_create  $LOOP17  $FILE17  a   600  50  a  1
gpt_create  $LOOP17  $FILE17  a   650  50  a  1
gpt_create  $LOOP17  $FILE17  a   700  50  a  1
gpt_create  $LOOP17  $FILE17  a   750  50  a  1
gpt_create  $LOOP17  $FILE17  a   800  50  a  1
gpt_create  $LOOP17  $FILE17  a   850  50  a  1
gpt_create  $LOOP17  $FILE17  a   900  50  a  1
gpt_create  $LOOP17  $FILE17  a   950  50  a  1
gpt_create  $LOOP17  $FILE17  a  1000  50  a  1
gpt_create  $LOOP17  $FILE17  a  1050  50  a  1
gpt_create  $LOOP17  $FILE17  a  1100  50  a  1
gpt_create  $LOOP17  $FILE17  a  1150  50  a  1
gpt_create  $LOOP17  $FILE17  a  1200  50  a  1
gpt_create  $LOOP17  $FILE17  a  1250  50  a  1
gpt_create  $LOOP17  $FILE17  a  1300  50  a  1
gpt_create  $LOOP17  $FILE17  a  1350  50  a  1
gpt_create  $LOOP17  $FILE17  a  1400  50  a  1
gpt_create  $LOOP17  $FILE17  a  1450  50  a  1
gpt_create  $LOOP17  $FILE17  a  1500  50  a  1
gpt_create  $LOOP17  $FILE17  a  1550  50  a  1
gpt_create  $LOOP17  $FILE17  a  1600  50  a  1
gpt_create  $LOOP17  $FILE17  a  1650  50  a  1
gpt_create  $LOOP17  $FILE17  a  1700  50  a  1
gpt_create  $LOOP17  $FILE17  a  1750  50  a  1
gpt_create  $LOOP17  $FILE17  a  1800  50  a  1
gpt_create  $LOOP17  $FILE17  a  1850  50  a  1
gpt_create  $LOOP17  $FILE17  a  1900  50  a  1
gpt_create  $LOOP17  $FILE17  a  1950  50  a  1
gpt_create  $LOOP17  $FILE17  a  2000  50  a  1
gpt_create  $LOOP17  $FILE17  a  2050  50  a  1
gpt_create  $LOOP17  $FILE17  a  2100  50  a  1
gpt_create  $LOOP17  $FILE17  a  2150  50  a  1
gpt_create  $LOOP17  $FILE17  a  2200  50  a  1
gpt_create  $LOOP17  $FILE17  a  2250  50  a  1
gpt_create  $LOOP17  $FILE17  a  2300  50  a  1
gpt_create  $LOOP17  $FILE17  a  2350  50  a  1
gpt_create  $LOOP17  $FILE17  a  2400  50  a  1
gpt_create  $LOOP17  $FILE17  a  2450  50  a  1
gpt_create  $LOOP17  $FILE17  a  2500  50  a  1
gpt_create  $LOOP17  $FILE17  a  2550  50  a  1
gpt_create  $LOOP17  $FILE17  a  2600  50  a  1
gpt_create  $LOOP17  $FILE17  a  2650  50  a  1
gpt_create  $LOOP17  $FILE17  a  2700  50  a  1
gpt_create  $LOOP17  $FILE17  a  2750  50  a  1
gpt_create  $LOOP17  $FILE17  a  2800  50  a  1
gpt_create  $LOOP17  $FILE17  a  2850  50  a  1
gpt_create  $LOOP17  $FILE17  a  2900  50  a  1
gpt_create  $LOOP17  $FILE17  a  2950  50  a  1
gpt_create  $LOOP17  $FILE17  a  3000  50  a  1
gpt_create  $LOOP17  $FILE17  a  3050  50  a  1
gpt_create  $LOOP17  $FILE17  a  3100  50  a  1
gpt_create  $LOOP17  $FILE17  a  3150  50  a  1
gpt_create  $LOOP17  $FILE17  a  3200  50  a  1
gpt_create  $LOOP17  $FILE17  a  3250  50  a  1
gpt_create  $LOOP17  $FILE17  a  3300  50  a  1
gpt_create  $LOOP17  $FILE17  a  3350  50  a  1
gpt_create  $LOOP17  $FILE17  a  3400  50  a  1
gpt_create  $LOOP17  $FILE17  a  3450  50  a  1
gpt_create  $LOOP17  $FILE17  a  3500  50  a  1
gpt_create  $LOOP17  $FILE17  a  3550  50  a  1
gpt_create  $LOOP17  $FILE17  a  3600  50  a  1
gpt_create  $LOOP17  $FILE17  a  3650  50  a  1
gpt_create  $LOOP17  $FILE17  a  3700  50  a  1
gpt_create  $LOOP17  $FILE17  a  3750  50  a  1
gpt_create  $LOOP17  $FILE17  a  3800  50  a  1
gpt_create  $LOOP17  $FILE17  a  3850  50  a  1
gpt_create  $LOOP17  $FILE17  a  3900  50  a  1
gpt_create  $LOOP17  $FILE17  a  3950  50  a  1
gpt_create  $LOOP17  $FILE17  a  4000  50  a  1
gpt_create  $LOOP17  $FILE17  a  4050  50  a  1
gpt_create  $LOOP17  $FILE17  a  4100  50  a  1
gpt_create  $LOOP17  $FILE17  a  4150  50  a  1
gpt_create  $LOOP17  $FILE17  a  4200  50  a  1
gpt_create  $LOOP17  $FILE17  a  4250  50  a  1
gpt_create  $LOOP17  $FILE17  a  4300  50  a  1
gpt_create  $LOOP17  $FILE17  a  4350  50  a  1
gpt_create  $LOOP17  $FILE17  a  4400  50  a  1
gpt_create  $LOOP17  $FILE17  a  4450  50  a  1
gpt_create  $LOOP17  $FILE17  a  4500  50  a  1
gpt_create  $LOOP17  $FILE17  a  4550  50  a  1
gpt_create  $LOOP17  $FILE17  a  4600  50  a  1
gpt_create  $LOOP17  $FILE17  a  4650  50  a  1
gpt_create  $LOOP17  $FILE17  a  4700  50  a  1
gpt_create  $LOOP17  $FILE17  a  4750  50  a  1
gpt_create  $LOOP17  $FILE17  a  4800  50  a  1
gpt_create  $LOOP17  $FILE17  a  4850  50  a  1
gpt_create  $LOOP17  $FILE17  a  4900  50  a  1
gpt_create  $LOOP17  $FILE17  a  4950  50  a  1

gpt_print $FILE17

losetup $LOOP17 $FILE17
rm -f $FILE17

