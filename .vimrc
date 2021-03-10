" Spaces and Tabs
syntax enable

set tabstop=2
set softtabstop=2
set expandtab
set smartindent
set cindent
"set nocindent

" UI Config
set showcmd
set number relativenumber
set cursorline


" autocompletes things like filename when you run something.
set wildmenu

set lazyredraw

"Highlight maching [{()}]
set showmatch

" Searching
set incsearch
set hlsearch

" turn of search highlight : Vim will keep highlighted matches from searches until you either run a new one or manually stop highlighting the old search with :nohlsearch
nnoremap <leader><space> :nohlsearch<CR>
" leader key is \ by default, you can remap the leader key by setting ":let mapleader=","

" Folding (I don't understand what this is about yet)

" Movement

" move to beginning/end of line
nnoremap B ^
nnoremap E $

" highlight last inserted text
nnoremap gV `[v`]

" Leader Shortcuts
let mapleader=","

" jk is escape
inoremap jk <esc>

" toggle gundo
nnoremap <leader>u :GundoToggle<CR>

nnoremap <leader>ev :vsp $MYVIMRC<CR>
nnoremap <leader>sv :source $MYVIMRC<CR>

" save session (super save)
nnoremap <leader>s :mksession<CR>
" open ag.vim
nnoremap <leader>a : Ag

colorscheme jellybeans
set mouse=an
