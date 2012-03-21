#!/bin/bash

function cleanup()
{
	lvremove -f /dev/mapper/test_* > /dev/null 2>&1
	local LOOPS="$(losetup -a | cut -d: -f1 | tac)"
	if [ -n "$LOOPS" ]; then
		for i in $LOOPS; do
			sync
			losetup -d $i
		done
	fi

	rm -f test_*.img
}


