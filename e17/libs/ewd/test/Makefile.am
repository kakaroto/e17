AUTOMAKE_OPTIONS		=	1.4 foreign
MAINTAINERCLEANFILES 		=	Makefile.in

CFLAGS				=	-g -W -Wall -static
INCLUDES			=	-I$(top_srcdir)/src

bin_PROGRAMS			=	ewd_tree_test ewd_list_test \
					ewd_dlist_test

ewd_tree_test_DEPENDENCIES	=	$(top_builddir)/src/libewd.la
ewd_tree_test_SOURCES		=	ewd_tree_test.c
ewd_tree_test_LDADD		=	$(top_builddir)/src/libewd.la

ewd_list_test_DEPENDENCIES	=	$(top_builddir)/src/libewd.la
ewd_list_test_SOURCES		=	ewd_list_test.c
ewd_list_test_LDADD		=	$(top_builddir)/src/libewd.la

ewd_dlist_test_DEPENDENCIES	=	$(top_builddir)/src/libewd.la
ewd_dlist_test_SOURCES		=	ewd_dlist_test.c
ewd_dlist_test_LDADD		=	$(top_builddir)/src/libewd.la
