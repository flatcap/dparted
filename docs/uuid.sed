#!/usr/bin/sed -f

s/\([A-Za-z0-9]\{6\}\)\(-[A-Za-z0-9]\{4\}\)\{5\}-[A-Za-z0-9]\{6\}/\1 /g
s/\(UUID \)                               /\1/g

