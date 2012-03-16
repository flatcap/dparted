#!/bin/bash

for i in $(enum 0 255); do
	[ -b /dev/loop$i ] || mknod --mode=660 /dev/loop$i b 7 $i
	chown root.disk /dev/loop$i
done

