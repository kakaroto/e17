## Process this file with automake to produce Makefile.in

INCLUDES = \
-I/usr/local/include \
-I$(top_srcdir)/lib \
@my_includes@

bin_PROGRAMS = enterminus

enterminus_SOURCES = \
main.c \
misc.c \
pty.c  \
term.c \
ui.c \
handlers.c  

enterminus_LDADD = @my_libs@
