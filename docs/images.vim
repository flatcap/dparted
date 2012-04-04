syntax match rich_disk       "disk"
syntax match rich_disk       "file"
syntax match rich_disk       "loop"
syntax match rich_filesystem "filesystem"
syntax match rich_misc       "empty"
syntax match rich_misc       "unknown"
syntax match rich_partition  "partition"
syntax match rich_table      "extended partition"
syntax match rich_table      "gpt table"
syntax match rich_table      "lvm table"
syntax match rich_table      "msdos table"
syntax match rich_table      "protective table"
syntax match rich_table      "volume group"

highlight rich_disk       ctermfg=118	" green
highlight rich_filesystem ctermfg=123	" cyan
highlight rich_misc       ctermfg=214	" orange
highlight rich_partition  ctermfg=227	" yellow
highlight rich_table      ctermfg=198	" red
highlight rich_volume     ctermfg=147	" purple

