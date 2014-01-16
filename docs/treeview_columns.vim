" XXX script this from a list of numbers
" XXX script this from the header line

syntax clear

" set colorcolumn=1,11,28,39,51,63,76,103,140

syntax match rich_col1 "\%1c.\{,10\}"
syntax match rich_col2 "\%11c.\{,17\}"
syntax match rich_col1 "\%28c.\{,11\}"
syntax match rich_col2 "\%39c.\{,12\}"
syntax match rich_col1 "\%51c.\{,12\}"
syntax match rich_col2 "\%63c.\{,13\}"
syntax match rich_col1 "\%76c.\{,27\}"
syntax match rich_col2 "\%103c.\{,37\}"
syntax match rich_col1 "\%140c.\{,50\}"

highlight rich_col1 ctermfg=230
highlight rich_col2 ctermfg=225

syntax match rich_row1 "\%1l.*"
syntax match rich_row1 "^[^ 	].\+$"

highlight rich_row1 cterm=bold

set cursorline

