#!/usr/bin/sed -f

# Strip leading two spaces
s/^  //

# Duplicate the first line and turn LVM2_IDENT=VALUE => IDENT
1{h;s/=[^	]*//g;s/LVM2_//g;p;x}

# For the remaining lines, strips the LVM2_IDENT=
s/LVM2_[^=]\+=//g

# Replace any repeated empty columns with dots
s/		/	.	/g
s/		/	.	/g

# Simplify UUIDs to the first clause
s/\([0-9a-zA-Z]\{6\}\)-[0-9a-zA-Z]\{4\}-[0-9a-zA-Z]\{4\}-[0-9a-zA-Z]\{4\}-[0-9a-zA-Z]\{4\}-[0-9a-zA-Z]\{4\}-[0-9a-zA-Z]\{6\}/\1/g

