" Vim syntax file
" Language:    eruby (Ruby embedded in HTML)
" Maintainer:  Tommi Mäkitalo <tommi@maekitalo.de>
" Last change: 2003 Jan 19
" URL:         http://www.maekitalo.de/
"
" Clear previous syntax settings unless this is v6 or above, in which case just
" exit without doing anything.
"
if version < 600
	syn clear
elseif exists("b:current_syntax")
	finish
endif

" The HTML syntax file included below uses this variable.
"
if !exists("main_syntax")
	let main_syntax = 'eruby'
endif

" First pull in the HTML syntax.
"
if version < 600
	so <sfile>:p:h/html.vim
else
	runtime! syntax/html.vim
	unlet b:current_syntax
endif

syn cluster htmlPreproc add=@erubyTop

" Now pull in the Ruby syntax.
"
if version < 600
	syn include @rubyTop <sfile>:p:h/ruby.vim
else
	syn include @rubyTop syntax/ruby.vim
endif

" It's hard to reduce down to the correct sub-set of Perl to highlight in some
" of these cases so I've taken the safe option of just using perlTop in all of
" them. If you have any suggestions, please let me know.
"
syn region erubyDoc matchgroup=Delimiter start="<%#" end="%>" contains=@rubyTop
syn region erubyRuby matchgroup=Delimiter start="<%[^#=]" end="%>" contains=@rubyTop
syn region erubyExpr matchgroup=Delimiter start="<%=" end="</%perl>" contains=@rubyTop

syn cluster erubyTop contains=erubyExpr,erubyRuby,erubyDoc

" Set up default highlighting. Almost all of this is done in the included
" syntax files.
"
if version >= 508 || !exists("did_eruby_syn_inits")
	if version < 508
		let did_eruby_syn_inits = 1
		com -nargs=+ HiLink hi link <args>
	else
		com -nargs=+ HiLink hi def link <args>
	endif

	HiLink erubyDoc Comment

	delc HiLink
endif

let b:current_syntax = "eruby"

if main_syntax == 'eruby'
	unlet main_syntax
endif
