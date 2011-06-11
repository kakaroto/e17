set tags=./tags,../tags,../../tags,../../../tags,../../../../tags,${PWD}/tags,../../../../../tags,../../../.../../../tags
set nu
set ci
set ai
set hls
set tabstop=4
set shiftwidth=4
set paste
set ruler
colorscheme evening

" indent for e17  Ref:http://trac.enlightenment.org/e/wiki/ECoding
nnoremap <silent> <F9> :set ts=8 sw=3 sts=3 expandtab cino=>5n-3f0^-2{2(0W1st0<CR>

" Show trailing whitespace and spaces before tabs
hi link localWhitespaceError Error
au Syntax * syn match localWhitespaceError /\(\zs\%#\|\s\)\+$/ display
au Syntax * syn match localWhitespaceError / \+\ze\t/ display

" cscope
set csprg=/usr/bin/cscope
set csto=0
set cst
set nocsverb
if filereadable("./cscope.out")
	cs add cscope.out
else
	cs add /usr/src/linux/cscope.out
endif
set csverb
