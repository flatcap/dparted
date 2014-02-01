#!/bin/bash

[ -n "$1" ] || exit 1

for i in "$@"; do
	[ -f "$1" ] || continue

	ORIG_FILE="$i"
	TMP_FILE=${ORIG_FILE%.png}-$$.png

	convert -gravity North -crop 0x0+0-15 -negate "$ORIG_FILE" "$TMP_FILE"

	[ -f "$TMP_FILE" ] || exit 1

	mv "$TMP_FILE" "$ORIG_FILE"

	echo "Cropped and negated: $ORIG_FILE"
done

