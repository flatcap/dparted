#!/bin/bash

source common.sh

lvremove -f /dev/mapper/test_* > /dev/null 2>&1
LOOPS="$(losetup -a | cut -d: -f1)"
if [ -n "$LOOPS" ]; then
	for i in $LOOPS; do
		losetup -d $i
	done
fi

rm -f test_*.img

