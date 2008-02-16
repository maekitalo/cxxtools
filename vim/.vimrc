" We use a vim
set autoindent
set expandtab
set shiftwidth=2
set bs=2
set smartindent
" set mousehide
" set mousefocus
" set mouse=a
" set mousem=popup
set nohlsearch
set nocompatible
set incsearch
set swapfile

" remove all auto-commands
autocmd!

" C autocmds
autocmd BufRead         *.c     source ~/.vim/vimrc.c
autocmd BufNewFile      *.c     source ~/.vim/vimrc.c
" C++ autocmds
autocmd BufRead         *.cc    source ~/.vim/vimrc.cpp
autocmd BufNewFile      *.cc    source ~/.vim/vimrc.cpp
autocmd BufRead         *.cpp    source ~/.vim/vimrc.cpp
autocmd BufNewFile      *.cpp    source ~/.vim/vimrc.cpp
autocmd BufNewFile      *.cpp     0r ~/.vim/skeletons/skel.cpp
" H autocmds
autocmd BufRead         *.h     source ~/.vim/vimrc.h
autocmd BufNewFile      *.h     source ~/.vim/vimrc.h
autocmd BufNewFile      *.h     0r ~/.vim/skeletons/skel.h
" HTML autocmds
autocmd BufRead         *.htm,*.html    source ~/.vim/vimrc.html
autocmd BufNewFile      *.htm,*.html    source ~/.vim/vimrc.html
autocmd BufNewFile      *.htm,*.html    0r ~/.vim/skeletons/skel.html
" HTML::Mason autocmds
autocmd BufRead         *.hms   source ~/.vim/vimrc.mason
autocmd BufNewFile      *.hms   source ~/.vim/vimrc.mason
autocmd BufNewFile      *.hms   0r ~/.vim/skeletons/skel.mason
" Perl autocmds
autocmd BufRead         *.pl    source ~/.vim/vimrc.perl
autocmd BufNewFile      *.pl    0r ~/.vim/skeletons/skel.pl
autocmd BufNewFile      *.pl    source ~/.vim/vimrc.perl
" Java autocmds
autocmd BufNewFile      *.java  0r ~/.vim/skeletons/skel.java
" docbook autocmds
autocmd BufRead         *.docbook  source ~/.vim/vimrc.docbook
autocmd BufNewFile      *.docbook  source ~/.vim/vimrc.docbook
autocmd BufNewFile      *.docbook  0r ~/.vim/skeletons/skel.docbook
" Java::Mason autocmds
autocmd BufRead         *.jms   source ~/.vim/vimrc.mason
autocmd BufNewFile      *.jms   source ~/.vim/vimrc.mason
" ECPP
autocmd BufRead         *.ecpp   source ~/.vim/vimrc.ecpp
autocmd BufNewFile      *.ecpp   source ~/.vim/vimrc.ecpp

map <S-F3>  :set ft=mason <CR>
map <F2>    :if exists("syntax_on") <Bar> syntax off <Bar> else <Bar> syntax on <Bar> endif <CR>
map <F4>    :if &hls <Bar> set nohls <Bar> else <Bar> set hls <Bar> endif <CR>
map <F6>    :if &list <Bar> set nolist <Bar> else <Bar> set list <Bar> endif <CR>
map <F7>    :if &wrap <Bar> set nowrap <Bar> else <Bar> set wrap <Bar> endif <CR>
map <F8>    @@
map <F9>    <C-w>w<C-w>_
map <S-F9>  <C-w>W<C-w>_
" jump to next/prev line with same indentation
map <F11>   :call search ("^". matchstr (getline (line (".")), '\(\s*\)') ."\\S")<CR>^
map <S-F11> k:call search ("^". matchstr (getline (line (".")+ 1), '\(\s*\)') ."\\S", 'b')<CR>^
map <C-Tab> :bnext<CR>
map <C-l>   :ls<CR>
map <C-q>   :bdelete<CR>

"
" Colo(u)red or not colo(u)red
" If you want color you should set this to true
"
let color = "true"
"
if has("syntax")
    if color == "true"
	" This will switch colors ON
	so ${VIMRUNTIME}/syntax/syntax.vim
    else
	" this switches colors OFF
	syntax off
	set t_Co=0
    endif
endif

autocmd FileType  mason   source ~/.vim/vimrc.mason
set ts=4
set guioptions-=T " get rid of toolbar
set guioptions-=m " get rid of menu

set cmdheight=1
set hidden

" ~/.vimrc ends here
