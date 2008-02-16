augroup filetypedetect
au BufNewFile,BufRead *.bhtm    setf html
au BufNewFile,BufRead *.rhtml,*.rhtm   setf eruby
au BufNewFile,BufRead *.ecpp    setf ecpp
augroup END

