
SESSIONINFO: DEFAULT
let SessionLoad = 1
if &cp | set nocp | endif
let s:so_save = &so | let s:siso_save = &siso | set so=0 siso=0
let v:this_session=expand("<sfile>:p")
let C_Dictionary_File = "/home/saturn_vk/.vim/wordlists/c-c++-keywords.list,/home/saturn_vk/.vim/wordlists/k+r.list,/home/saturn_vk/.vim/wordlists/stl_index.list"
let CVSsortoutput =  1 
let Grep_Default_Filelist = "*"
let Sesscolor = "desert"
let Sessionaltbuf = "/home/saturn_vk/cvs/e17/e_modules/winselector/e_mod_main.c"
let CVSreloadaftercommit =  1 
let Grep_Xargs_Path = "xargs"
let Agrep_Path = "agrep"
let Tlist_Show_One_File =  0 
let Tlist_Auto_Highlight_Tag =  1 
let Tlist_Ctags_Cmd = "ctags"
let Tlist_Compact_Format =  0 
let CVSleavediff =  0 
let Grep_Find_Path = "find"
let Tlist_Sort_Type = "name"
let GetLatestVimScripts_wget = "wget"
let Tlist_Use_Horiz_Window =  0 
let CVSorgdiff =  0 
let Perl_Version = "2.9.2"
let CVSstatusline =  1 
let SesscolorGUI = "desert"
let Tlist_Enable_Fold_Column =  1 
let CVSorgwrap =  1 
let CVSofferrevision =  1 
let TagsExplorer_title = "[Tag List]"
let Tlist_Process_File_Always =  0 
let Tlist_Use_SingleClick =  0 
let CVSdumpandclose =  2 
let CVSallowemptymsg = "a"
let Grep_Find_Use_Xargs =  1 
let Tlist_Display_Tag_Scope =  1 
let FileExplorer_title = "[File List]"
let Grep_Shell_Quote_Char = "'"
let TagList_title = "__Tag_List__"
let CVSorgfoldlevel =  0 
let Grep_Null_Device = "/dev/null"
let CVSdontconvertfor = ""
let Sesswinpos = "winpos 361 479"
let Egrep_Path = "egrep"
let CVSdifforgbuf =  0 
let Sesscwd = "/home/saturn_vk/cvs/e17/e_modules/winselector"
let Tlist_Max_Submenu_Items =  20 
let CVSsavediff =  1 
let Grep_Skip_Files = "*~ *,v s.*"
let MRUList = ",1,"
let Sesswinsize = "set lines=28 columns=80"
let Tlist_WinHeight =  10 
let EnhCommentifyTraditionalMode = "Yes"
let CVSusedefaultmsg = "aj"
let CVStitlebar =  1 
let Tlist_Inc_Winwidth =  1 
let CVSfullstatus =  0 
let Grep_Shell_Escape_Char = "\\"
let CVSautocheck =  0 
let GetLatestVimScripts_allowautoinstall =  1 
let CVSorgscrollbind =  0 
let Grep_OpenQuickfixWindow =  1 
let GetLatestVimScripts_options = "-q -O"
let CVSorgfoldenable =  1 
let BufExplorer_title = "[Buf List]"
let Tlist_Exit_OnlyWindow =  0 
let Tlist_Display_Prototype =  0 
let CVSorgfoldcolumn =  0 
let CVSforcedirectory =  0 
let CVSqueryrevision =  0 
let Perl_Dictionary_File = "/home/saturn_vk/.vim/wordlists/perl.list"
let CVSspacesinannotate =  1 
let Tlist_Max_Tag_Length =  10 
let CVSdontswitch =  0 
let DidEnhancedCommentify =  1 
let Tlist_WinWidth =  30 
let Tlist_File_Fold_Auto_Close =  0 
let Tlist_Auto_Open =  0 
let Perl_Debugger = "ddd"
let CVScvsoutputencoding = ""
let Fgrep_Path = "fgrep"
let TagsExplorerSkipError =  0 
let Grep_Path = "grep"
let Grep_Cygwin_Find =  0 
let Grep_Skip_Dirs = "RCS CVS SCCS"
let CVSorgfoldmethod = "manual"
let CVScompressoutput =  1 
let Grep_Default_Options = ""
let C_Version = "3.8.2"
let CVSdefaultmsg = ""
let Tlist_Show_Menu =  1 
let GetLatestVimScripts_mv = "mv"
let Tlist_Use_Right_Window =  0 
silent only
cd ~/.vim/sessions
if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''
  let s:wipebuf = bufnr('%')
endif
set shortmess=aoO
badd +0 ~/cvs/e17/e_modules/winselector/e_mod_main.c
silent! argdel *
edit ~/cvs/e17/e_modules/winselector/e_mod_main.c
set splitbelow splitright
set nosplitbelow
set nosplitright
wincmd t
set winheight=1 winwidth=1
argglobal
let s:cpo_save=&cpo
set cpo&vim
imap <buffer> <S-F9> :call C_Arguments()
imap <buffer> <C-F9> :call C_Run()
imap <buffer> <F9> :call C_Link()
imap <buffer> <M-F9> :call C_Compile()

map <buffer> <S-F9> :call C_Arguments()
map <buffer> <C-F9> :call C_Run()
map <buffer> <F9> :call C_Link()

map <buffer> <M-F9> :call C_Compile()

let &cpo=s:cpo_save
unlet s:cpo_save
setlocal autoindent
setlocal autoread
setlocal balloonexpr=
setlocal nobinary
setlocal bufhidden=
setlocal buflisted
setlocal buftype=
setlocal cindent
setlocal cinkeys=0{,0},0),:,0#,!^F,o,O,e
setlocal cinoptions=>5n-3f0^-2{2
setlocal cinwords=if,else,while,do,for,switch
setlocal comments=sO:*\ -,mO:*\ \ ,exO:*/,s1:/*,mb:*,ex:*/,://
setlocal commentstring=/*%s*/
setlocal complete=.,w,b,u,t,i
setlocal completefunc=
setlocal nocopyindent
setlocal nocursorcolumn
setlocal cursorline
setlocal define=
setlocal dictionary=~/.vim/wordlists/c-c++-keywords.list,~/.vim/wordlists/k+r.list,~/.vim/wordlists/stl_index.list
setlocal nodiff
setlocal equalprg=
setlocal errorformat=
setlocal noexpandtab
if &filetype != 'c'
setlocal filetype=c
endif
setlocal foldcolumn=0
setlocal foldenable
setlocal foldexpr=0
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldmarker={{{,}}}
setlocal foldmethod=manual
setlocal foldminlines=1
setlocal foldnestmax=20
setlocal foldtext=foldtext()
setlocal formatexpr=
setlocal formatoptions=croql
setlocal formatlistpat=^\\s*\\d\\+[\\]:.)}\\t\ ]\\s*
setlocal grepprg=
setlocal iminsert=0
setlocal imsearch=2
setlocal include=
setlocal includeexpr=
setlocal indentexpr=
setlocal indentkeys=0{,0},:,0#,!^F,o,O,e
setlocal noinfercase
setlocal iskeyword=@,48-57,_,192-255
setlocal keywordprg=
setlocal nolinebreak
setlocal nolisp
setlocal nolist
setlocal makeprg=
setlocal matchpairs=(:),{:},[:]
setlocal modeline
setlocal modifiable
setlocal nrformats=octal,hex
setlocal nonumber
setlocal numberwidth=4
setlocal omnifunc=ccomplete#Complete
setlocal path=
setlocal nopreserveindent
setlocal nopreviewwindow
setlocal quoteescape=\\
setlocal noreadonly
setlocal noscrollbind
setlocal shiftwidth=3
setlocal noshortname
setlocal nosmartindent
setlocal softtabstop=8
setlocal nospell
setlocal spellcapcheck=[.?!]\\_[\\])'\"\	\ ]\\+
setlocal spellfile=
setlocal spelllang=en
setlocal statusline=
setlocal suffixesadd=
setlocal swapfile
setlocal synmaxcol=3000
if &syntax != 'c'
setlocal syntax=c
endif
setlocal tabstop=8
setlocal tags=
setlocal textwidth=0
setlocal thesaurus=
setlocal nowinfixheight
setlocal nowinfixwidth
setlocal wrap
setlocal wrapmargin=0
silent! normal! zE
let s:l = 300 - ((12 * winheight(0) + 13) / 26)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
300
normal! 03l
if exists('s:wipebuf')
  silent exe 'bwipe ' . s:wipebuf
endif
unlet! s:wipebuf
set winheight=1 winwidth=20 shortmess=filnxtToO
let s:sx = expand("<sfile>:p:r")."x.vim"
if file_readable(s:sx)
  exe "source " . s:sx
endif
let &so = s:so_save | let &siso = s:siso_save
doautoall SessionLoadPost
unlet SessionLoad
" vim: set ft=vim :
VIMINFO:
# This viminfo file was generated by Vim 7.0.
# You may edit it if you're careful!

# Value of 'encoding' when this file was written
*encoding=utf-8


# hlsearch on (H) or off (h):
~h
# Last Search Pattern:
~MSle0/^SESSIONINFO:\s*DEFAULT

# Last Substitute Search Pattern:
~MSle0~&^SESSIONINFO:\s*DEFAULT

# Last Substitute String:
$

# Command Line History (newest to oldest):
:q
:300
:e
:e enterm.c
:e term.c
:Grep find_user
:grep accuweather
:grep
:h :vimgrep
:vimgrep accuweather *js
:grep accuweather *js
:!echo $TERM
:dis
:'<,'>q
:/
:w``
:%s/default/darkness/gc
:h :s
:%s/maxVersion>.*</em:maxVersion>/maxVersion>2.5</em:maxVersion>/gc
:display
:%s/maxVersion>.*<\/em:maxVersion>/maxVersion>2.5<\/em:maxVersion>/gc
:w!
:%s/maxVersion=".*"/maxVersion="2.5"/gc
:bd
:ls
:tabe ~/cvs/e17/apps/e/data/themes/default_button.edc
:tabe ~
:tabe ../Carbon-14/carbon_border.edc
:e Carbon-14.edc
:e default_mem.edc
:e default_border.edc
:set ic
:set noic
:e ~/cvs/e17/apps/e/data/themes/default_border.edc
:e background_stars.edc
:e background_trees.edc
:e cvs/e17/apps/e/src/bin/e_border.c
:5100
:tabe carbon_start.edc
:e carbon_ibar.edc
:x
:tabe ~/.vimrc
:let g:session_none
:let g:session_none 0
:let g:session_dir = ~
:let g:session_dir = /home
:let g:session_dir = "/home"
:let g:session_dir
:w
:Quit

# Search String History (newest to oldest):
? ^SESSIONINFO:\s*DEFAULT
?/edj
?/manager
? \<_manager\>
?/url
? ^.*\%29c
?/options.
? \<locid\>
?/ff-text-code
?/mouse
?/term
?/constructor
? \<textblock\>
?/sw
?/swat
?/446
?/445
?/text {
? default
?/edje-vera
?/cpu
?/maxVersion
?/maxversion>.*<
?/maxversion
? maxVersion>.*<\/em:maxVersion>
? maxVersion=".*"
?/lineakd
?/Segmen
?/reflection_shadow_overlay
?/default\/border
?/button-plus
?/CARBON_WHITE
?/e17_border1.png
?/shaped
?/title_text
?/nvidia
? \<BORDER_BUTTON\>
?/name:\s*"t2"
? \<t1\>
?/BORDER
?/t3
?/t1
?/SWALLOW
? ^\d\{3}\/
? \<entry\>
? \<entry_focus\>
? \<e17_frame_label_over\>
? \<e17_frame_label_under\>
?/active
? ^C 

# Expression History (newest to oldest):

# Input Line History (newest to oldest):
@1.1.1.5
@*pm
@1.196
@1.195
@check access for edit button
@*pl
@INFILE
@/dev/urandom
@open
@free
@/home/saturn_vk/cvs/rhythmbox
@bd->layer
@*h
@*c
@1.9
@1.8
@1.10
@*edc
@/home/saturn_vk/cvs/e17/apps/evfs
@*
@darkness

# Input Line History (newest to oldest):

# Registers:
"0	LINE	0
		     if (zone->deskshow_toggle)
		       edje_object_signal_emit(inst->o_button, "passive", "");
		     else
		       edje_object_signal_emit(inst->o_button, "active", "");
""1	LINE	0
	SESSIONINFO: DEFAULT
	let SessionLoad = 1
	if &cp | set nocp | endif
	let s:so_save = &so | let s:siso_save = &siso | set so=0 siso=0
	let v:this_session=expand("<sfile>:p")
	let C_Dictionary_File = "/home/saturn_vk/.vim/wordlists/c-c++-keywords.list,/home/saturn_vk/.vim/wordlists/k+r.list,/home/saturn_vk/.vim/wordlists/stl_index.list"
	let CVSsortoutput =  1 
	let Grep_Default_Filelist = "*"
	let Sessionaltbuf = "/home/saturn_vk/.vimfavourites"
	let CVSreloadaftercommit =  1 
	let Grep_Xargs_Path = "xargs"
	let Agrep_Path = "agrep"
	let Tlist_Show_One_File =  0 
	let Tlist_Auto_Highlight_Tag =  1 
	let Tlist_Ctags_Cmd = "ctags"
	let Tlist_Compact_Format =  0 
	let CVSleavediff =  0 
	let Grep_Find_Path = "find"
	let Tlist_Sort_Type = "name"
	let Tlist_Use_Horiz_Window =  0 
	let CVSorgdiff =  0 
	let Grep_Path = "grep"
	let Perl_Version = "2.9.2"
	let Sesscolor = "desert"
	let TagsExplorer_title = "[Tag List]"
	let CVSstatusline =  1 
	let Tlist_Enable_Fold_Column =  1 
	let CVSorgwrap =  1 
	let CVSofferrevision =  1 
	let Tlist_Process_File_Always =  0 
	let Tlist_Use_SingleClick =  0 
	let CVSdumpandclose =  2 
	let CVSallowemptymsg = "a"
	let Grep_Find_Use_Xargs =  1 
	let Tlist_Display_Tag_Scope =  1 
	let FileExplorer_title = "[File List]"
	let Grep_Shell_Quote_Char = "'"
	let TagList_title = "__Tag_List__"
	let CVSorgfoldlevel =  0 
	let Grep_Null_Device = "/dev/null"
	let CVSdontconvertfor = ""
	let Egrep_Path = "egrep"
	let CVSdifforgbuf =  0 
	let Sesscwd = "/home/saturn_vk/.vim/sessions"
	let Tlist_Max_Submenu_Items =  20 
	let CVSsavediff =  1 
	let Grep_Skip_Files = "*~ *,v s.*"
	let MRUList = ",1,"
	let Tlist_WinHeight =  10 
	let EnhCommentifyTraditionalMode = "Yes"
"2	LINE	0
	nmap ,cf :em 
	nmap ,cgf :em 
	nmap ,cgm :em 
	nmap ,cgae :em 
	nmap ,cgad :em 
	nmap ,cgtb :em 
	nmap ,cgtd :em 
	nmap ,cgdo :em 
	nmap ,cgdd :em 
	nmap ,cgda :em 
	nmap ,cgdn :em 
	nmap ,cgds :em 
	nmap ,cgqe :em 
	nmap ,cgqd :em 
	nmap ,cgqo :em 
	nmap ,cgqh :em 
	nmap ,cgoos :em 
	nmap ,cgool :em 
	nmap ,cgoot :em 
	nmap ,cgoob :em 
	nmap ,cgob :em 
	nmap ,cgon :em 
	nmap ,cgoa :em 
	nmap ,cgoc :em 
	nmap ,cgof :em 
	nmap ,cgos :em 
	nmap ,cgou :em 
	nmap ,cgii :em 
	nmap ,cgid :em 
	nmap ,cgih :em 
	nmap ,cgip :em 
	nmap ,cka :em 
	nmap ,ckd :em 
	nmap ,ckh :em 
	nmap ,cki :em 
	nmap ,ckn :em 
	nmap ,ckk :em 
	nmap ,ckl :em 
	nmap ,ckf :em 
	nmap ,ckr :em 
	nmap ,cks :em 
	nmap ,ckt :em 
	nmap ,cyl :em 
	nmap ,cys :em 
	nmap ,cyh :em 
	nmap ,cyc :em 
	nmap ,cyq :em 
	nmap ,cyu :em 
	nmap ,cya :em 
	nmap ,cyi :em 
"3	LINE	0
	amenu &CVS.-SEP4-						:
"4	LINE	0
	amenu &CVS.-SEP3-						:
"5	LINE	0
	amenu &CVS.-SEP2-						:
"6	LINE	0
	amenu &CVS.&Watch/Edit\ .-SEP1-					:
"7	LINE	0
	amenu &CVS.&Tag\ .-SEP1-					:
"8	LINE	0
	amenu &CVS.-SEP1-						:
"9	LINE	0
	amenu &CVS.E&xtra\ .-SEP3-					:
"a	LINE	0
	iptables -A INPUT -p tcp --dport 143 -j ACCEPT
"b	LINE	0
		    max: 8 8;
		    min: 8 8;
"k	LINE	0
	nmap ,cf :em 
	nmap ,cgf :em 
	nmap ,cgm :em 
	nmap ,cgae :em 
	nmap ,cgad :em 
	nmap ,cgtb :em 
	nmap ,cgtd :em 
	nmap ,cgdo :em 
	nmap ,cgdd :em 
	nmap ,cgda :em 
	nmap ,cgdn :em 
	nmap ,cgds :em 
	nmap ,cgqe :em 
	nmap ,cgqd :em 
	nmap ,cgqo :em 
	nmap ,cgqh :em 
	nmap ,cgoos :em 
	nmap ,cgool :em 
	nmap ,cgoot :em 
	nmap ,cgoob :em 
	nmap ,cgob :em 
	nmap ,cgon :em 
	nmap ,cgoa :em 
	nmap ,cgoc :em 
	nmap ,cgof :em 
	nmap ,cgos :em 
	nmap ,cgou :em 
	nmap ,cgii :em 
	nmap ,cgid :em 
	nmap ,cgih :em 
	nmap ,cgip :em 
	nmap ,cka :em 
	nmap ,ckd :em 
	nmap ,ckh :em 
	nmap ,cki :em 
	nmap ,ckn :em 
	nmap ,ckk :em 
	nmap ,ckl :em 
	nmap ,ckf :em 
	nmap ,ckr :em 
	nmap ,cks :em 
	nmap ,ckt :em 
	nmap ,cyl :em 
	nmap ,cys :em 
	nmap ,cyh :em 
	nmap ,cyc :em 
	nmap ,cyq :em 
	nmap ,cyu :em 
	nmap ,cya :em 
	nmap ,cyi :em 
"m	LINE	0
	&CVS.In&fo\ 
	&CVS.Settin&gs\ .In&fo\ (buffer)\ 
	&CVS.Settin&gs\ .Show\ &mappings\ 
	&CVS.Settin&gs\ .&Autocheck\ .&Enable\ 
	&CVS.Settin&gs\ .&Autocheck\ .&Disable\ 
	&CVS.Settin&gs\ .&Target\ .File\ in\ &buffer\ 
	&CVS.Settin&gs\ .&Target\ .&Directory\ 
	&CVS.Settin&gs\ .&Diff\ .Stay\ in\ &original\ 
	&CVS.Settin&gs\ .&Diff\ .Switch\ to\ &diffed\ 
	&CVS.Settin&gs\ .&Diff\ .&Autorestore\ prev\.mode\ 
	&CVS.Settin&gs\ .&Diff\ .&No\ autorestore\ 
	&CVS.Settin&gs\ .&Diff\ .Re&store\ pre-diff\ mode\ 
	&CVS.Settin&gs\ .Revision\ &queries\ .&Enable\ 
	&CVS.Settin&gs\ .Revision\ &queries\ .&Disable\ 
	&CVS.Settin&gs\ .Revision\ &queries\ .&Offer\ current\ rev\ 
	&CVS.Settin&gs\ .Revision\ &queries\ .&Hide\ current\ rev\ 
	&CVS.Settin&gs\ .&Output\ .N&otifcation.Enable\ &statusline\ 
	&CVS.Settin&gs\ .&Output\ .N&otifcation.Disable\ status&line\ 
	&CVS.Settin&gs\ .&Output\ .N&otifcation.Enable\ &titlebar\ 
	&CVS.Settin&gs\ .&Output\ .N&otifcation.Disable\ title&bar\ 
	&CVS.Settin&gs\ .&Output\ .To\ new\ &buffer\ 
	&CVS.Settin&gs\ .&Output\ .&Notify\ only\ 
	&CVS.Settin&gs\ .&Output\ .&Autoswitch\ 
	&CVS.Settin&gs\ .&Output\ .&Compressed\ 
	&CVS.Settin&gs\ .&Output\ .&Full\ 
	&CVS.Settin&gs\ .&Output\ .&Sorted\ 
	&CVS.Settin&gs\ .&Output\ .&Unsorted\ 
	&CVS.Settin&gs\ .&Install\ .&Install\ updates\ 
	&CVS.Settin&gs\ .&Install\ .&Download\ updates\ 
	&CVS.Settin&gs\ .&Install\ .Install\ buffer\ as\ &help\ 
	&CVS.Settin&gs\ .&Install\ .Install\ buffer\ as\ &plugin\ 
	&CVS.&Keyword\ .&Author\ 
	&CVS.&Keyword\ .&Date\ 
	&CVS.&Keyword\ .&Header\ 
	&CVS.&Keyword\ .&Id\ 
	&CVS.&Keyword\ .&Name\ 
	&CVS.&Keyword\ .Loc&ker\ 
	&CVS.&Keyword\ .&Log\ 
	&CVS.&Keyword\ .RCS&file\ 
	&CVS.&Keyword\ .&Revision\ 
	&CVS.&Keyword\ .&Source\ 
	&CVS.&Keyword\ .S&tate\ 
	&CVS.Director&y\ .&Log\ 
	&CVS.Director&y\ .&Status\ 
	&CVS.Director&y\ .S&hort\ status\ 
	&CVS.Director&y\ .Lo&cal\ status\ 
	&CVS.Director&y\ .&Query\ update\ 
	&CVS.Director&y\ .&Update\ 
	&CVS.Director&y\ .&Add\ 
	&CVS.Director&y\ .Comm&it\ 
"q	CHAR	0
	ЂkdЂkdЂkdЂkdЂkdЂkuЂkuЂkuЂkuЂku
"s	CHAR	0
	\<perlMethod\>HiLink
"-	CHAR	0
	yes

# global variables:
!NETRW_DIRHIST_CNT	NUM	1
!ST_UNIX	NUM	2
!NETRW_DIRHIST_1	STR	/home/saturn_vk/cvs/e17/misc/engage/data/themes/gentoo
!ST_WIN_SH	NUM	1
!ST_WIN_CMD	NUM	0

# File marks:
'Z  4  0  ~/.e/e/themes/Carbon-14/[BufExplorer]
'0  232  0  ~/cvs/e17/e_modules/winselector/sessions.vim
'1  300  3  ~/cvs/e17/e_modules/winselector/e_mod_main.c
'2  704  0  ~/.vim/sessions/sessions.vim
'3  2  0  ~/.vim/sessions/sessions.vim
'4  20  0  ~/.vim/plugin/sessmgr.vim
'5  3  70  /tmp/cvseiv4Fh
'6  2  18  /tmp/cvs6lfZ3O
'7  2  66  /tmp/cvsjnlLrt
'8  136  0  ~/cvs/e17/e_modules/deskshow/e_mod_main.c
'9  1  0  ~/.e/e/themes/Carbon-14/carbon_battery.edc

# Jumplist (newest first):
-'  232  0  ~/cvs/e17/e_modules/winselector/sessions.vim
-'  300  3  ~/cvs/e17/e_modules/winselector/e_mod_main.c
-'  88  21  ~/cvs/e17/e_modules/winselector/e_mod_main.c
-'  1  0  ~/cvs/e17/e_modules/winselector/e_mod_main.c
-'  704  0  ~/.vim/sessions/sessions.vim
-'  1  0  ~/.vim/sessions/sessions.vim
-'  20  0  ~/.vim/plugin/sessmgr.vim
-'  379  1  ~/.vim/plugin/sessmgr.vim
-'  404  1  ~/.vim/plugin/sessmgr.vim
-'  19  12  ~/.vim/plugin/sessmgr.vim
-'  14  5  ~/.vim/plugin/sessmgr.vim
-'  13  11  ~/.vim/plugin/sessmgr.vim
-'  1  0  ~/.vim/plugin/sessmgr.vim
-'  374  0  ~/.vim/plugin/sessmgr.vim
-'  3  70  /tmp/cvseiv4Fh
-'  1  0  /tmp/cvseiv4Fh
-'  2  18  /tmp/cvs6lfZ3O
-'  1  0  /tmp/cvs6lfZ3O
-'  2  66  /tmp/cvsjnlLrt
-'  1  0  /tmp/cvsjnlLrt
-'  136  0  ~/cvs/e17/e_modules/deskshow/e_mod_main.c
-'  126  31  ~/cvs/e17/e_modules/deskshow/e_mod_main.c
-'  120  8  ~/cvs/e17/e_modules/deskshow/e_mod_main.c
-'  1  0  ~/cvs/e17/e_modules/deskshow/e_mod_main.c
-'  1  0  ~/.e/e/themes/Carbon-14/carbon_battery.edc
-'  72  20  ~/cvs/e17/apps/e/data/themes/default_ibar.edc
-'  1  0  ~/cvs/e17/apps/e/data/themes/default_ibar.edc
-'  122  6  ~/.e/e/themes/Carbon-14/carbon_ibar.edc
-'  101  22  ~/.e/e/themes/Carbon-14/carbon_entry.edc
-'  5  28  ~/.e/e/themes/Carbon-14/carbon_entry.edc
-'  4  14  ~/.e/e/themes/Carbon-14/carbon_entry.edc
-'  120  16  ~/.e/e/themes/Carbon-14/carbon_entry.edc
-'  85  24  ~/.e/e/themes/Carbon-14/carbon_entry.edc
-'  71  16  ~/.e/e/themes/Carbon-14/carbon_entry.edc
-'  65  24  ~/.e/e/themes/Carbon-14/carbon_entry.edc
-'  3  12  ~/.e/e/themes/Carbon-14/carbon_entry.edc
-'  43  16  ~/.e/e/themes/Carbon-14/carbon_entry.edc
-'  24  8  ~/.e/e/themes/Carbon-14/carbon_entry.edc
-'  19  8  ~/.e/e/themes/Carbon-14/carbon_entry.edc
-'  9  18  ~/.e/e/themes/Carbon-14/carbon_entry.edc
-'  2  14  ~/.e/e/themes/Carbon-14/carbon_entry.edc
-'  89  0  ~/.e/e/themes/Carbon-14/carbon_entry.edc
-'  129  12  ~/cvs/e17/misc/engage/data/themes/gentoo.edc
-'  290  8  ~/cvs/e17/misc/engage/data/themes/gentoo.edc
-'  8  0  ~/cvs/e17/misc/engage/data/themes/gentoo
-'  1  0  ~/cvs/e17/misc/engage/data/themes/gentoo
-'  12  12  ~/cvs/e17/misc/engage/data/themes/gentoo.edc
-'  1  0  ~/cvs/e17/misc/engage/data/themes/gentoo.edc
-'  428  3  /var/log/nvidia-installer.log
-'  1  0  /var/log/nvidia-installer.log
-'  131  0  ~/.e/e/themes/Carbon-14/carbon_start.edc
-'  181  0  ~/.e/e/themes/Carbon-14/carbon_start.edc
-'  1  0  ~/.e/e/themes/Carbon-14/carbon_entry.edc
-'  4709  6  ~/cvs/e17/apps/e/src/bin/e_border.c
-'  1  0  ~/cvs/e17/apps/e/src/bin/e_border.c
-'  40  0  ~/.e/e/themes/blue_eyed_0.6.5.8/edjes/parts/background_trees.edc
-'  39  0  ~/.e/e/themes/blue_eyed_0.6.5.8/edjes/parts/background_stars.edc
-'  12  0  ~/.e/e/themes/blue_eyed_0.6.5.8/edjes/parts/background_background.edc
-'  1  0  ~/.e/e/themes/blue_eyed_0.6.5.8/edjes/parts/background_background.edc
-'  8  0  ~/.e/e/themes/blue_eyed_0.6.5.8/edjes/background.edc
-'  1  0  ~/.e/e/themes/blue_eyed_0.6.5.8/edjes/background.edc
-'  65  0  ~/.e/e/themes/blue_eyed_0.6.5.8/LISCENSE.edc
-'  1  0  ~/.e/e/themes/blue_eyed_0.6.5.8/LISCENSE.edc
-'  1016  17  ~/cvs/e17/apps/e/data/themes/default_border.edc
-'  424  11  ~/cvs/e17/apps/e/data/themes/default_border.edc
-'  342  19  ~/cvs/e17/apps/e/data/themes/default_border.edc
-'  301  18  ~/cvs/e17/apps/e/data/themes/default_border.edc
-'  221  28  ~/cvs/e17/apps/e/data/themes/default_border.edc
-'  1  0  ~/cvs/e17/apps/e/data/themes/default_border.edc
-'  194  25  ~/.e/e/themes/Milky-0.9.8.5-m/default_border.edc
-'  296  11  ~/.e/e/themes/Milky-0.9.8.5-m/default_border.edc
-'  417  19  ~/.e/e/themes/Milky-0.9.8.5-m/default_border.edc
-'  379  18  ~/.e/e/themes/Milky-0.9.8.5-m/default_border.edc
-'  367  19  ~/.e/e/themes/Milky-0.9.8.5-m/default_border.edc
-'  244  28  ~/.e/e/themes/Milky-0.9.8.5-m/default_border.edc
-'  1  0  ~/.e/e/themes/Milky-0.9.8.5-m/default_border.edc
-'  967  17  ~/.e/e/themes/Milky-0.9.8.5-m/default_border.edc
-'  820  0  ~/.e/e/themes/Milky-0.9.8.5-m/default_border.edc
-'  698  17  ~/.e/e/themes/Milky-0.9.8.5-m/default_border.edc
-'  933  23  ~/.e/e/themes/Milky-0.9.8.5-m/default_border.edc
-'  833  30  ~/.e/e/themes/Milky-0.9.8.5-m/default_border.edc
-'  849  19  ~/.e/e/themes/Milky-0.9.8.5-m/default_border.edc
-'  824  19  ~/.e/e/themes/Milky-0.9.8.5-m/default_border.edc
-'  656  19  ~/.e/e/themes/Milky-0.9.8.5-m/default_border.edc
-'  631  19  ~/.e/e/themes/Milky-0.9.8.5-m/default_border.edc
-'  605  19  ~/.e/e/themes/Milky-0.9.8.5-m/default_border.edc
-'  585  19  ~/.e/e/themes/Milky-0.9.8.5-m/default_border.edc
-'  362  19  ~/.e/e/themes/Milky-0.9.8.5-m/default_border.edc
-'  321  19  ~/.e/e/themes/Milky-0.9.8.5-m/default_border.edc
-'  799  21  ~/.e/e/themes/Milky-0.9.8.5-m/default_border.edc
-'  223  6  ~/.e/e/themes/Milky-0.9.8.5-m/default_border.edc
-'  1646  5  ~/.e/e/themes/Milky-0.9.8.5-m/default_border.edc
-'  1  0  ~/.Xdefaults
-'  90  4  /usr/aw/maya7.0/bin/Maya7.0
-'  1  0  /usr/aw/maya7.0/bin/Maya7.0
-'  1  0  /usr/aw/maya7.0/bin/maya
-'  301  8  ~/cvs/e17/e_modules/winselector/e_mod_main.c

# History of marks within files (newest to oldest):

> ~/cvs/e17/e_modules/winselector/e_mod_main.c
	"	300	3
	^	88	64
	.	88	0
	+	142	0
	+	130	0
	+	190	0
	+	130	0
	+	137	36
	+	201	20
	+	35	0
	+	140	0
	+	137	34
	+	201	28
	+	35	44
	+	137	34
	+	206	60
	+	217	0
	+	220	0
	+	252	0
	+	246	34
	+	252	16
	+	258	7
	+	251	3
	+	252	0
	+	257	0
	+	258	0
	+	265	0
	+	260	19
	+	246	168
	+	247	64
	+	252	19
	+	260	19
	+	246	55
	+	247	62
	+	249	64
	+	251	0
	+	249	63
	+	251	0
	+	249	63
	+	251	62
	+	301	0
	+	88	0
	a	145	6
	b	246	1

> ~/.vimfavourites
	"	6	0
	^	6	61
	.	6	0
	+	2	0
	+	3	0
	+	4	0
	+	5	0
	+	6	0

> ~/cvs/e17/e_modules/winselector/sessions.vim
	"	232	0
	.	232	0
	+	232	0
