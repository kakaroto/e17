AUTOMAKE_OPTIONS		=	1.4 foreign
MAINTAINERCLEANFILES 		=	Makefile.in

CFLAGS				=	-g -W -Wall -static
					
INCLUDES			=	-I$(top_srcdir)/src \
					`glib-config --cflags`

bin_PROGRAMS			=	ewd_tree_test ewd_list_test \
					ewd_dlist_test ewd_threads_test \
					ewd_hash_test ewd_index_test

ewd_tree_test_DEPENDENCIES	=	$(top_builddir)/src/libewd.la
ewd_tree_test_SOURCES		=	ewd_tree_test.c
ewd_tree_test_LDADD		=	$(top_builddir)/src/libewd.la

ewd_list_test_DEPENDENCIES	=	$(top_builddir)/src/libewd.la
ewd_list_test_SOURCES		=	ewd_list_test.c
ewd_list_test_LDADD		=	$(top_builddir)/src/libewd.la

ewd_dlist_test_DEPENDENCIES	=	$(top_builddir)/src/libewd.la
ewd_dlist_test_SOURCES		=	ewd_dlist_test.c
ewd_dlist_test_LDADD		=	$(top_builddir)/src/libewd.la

ewd_hash_test_DEPENDENCIES	=	$(top_builddir)/src/libewd.la
ewd_hash_test_SOURCES		=	ewd_hash_test.c
ewd_hash_test_LDADD		=	$(top_builddir)/src/libewd.la
ewd_hash_test_LDFLAGS		=	`glib-config --libs`

ewd_threads_test_DEPENDENCIES	=	$(top_builddir)/src/libewd.la
ewd_threads_test_SOURCES	=	ewd_threads_test.c
ewd_threads_test_LDADD		=	$(top_builddir)/src/libewd.la -lpthread

ewd_index_test_DEPENDENCIES	=	$(top_builddir)/src/libewd.la
ewd_index_test_SOURCES	=	ewd_index_test.c
ewd_index_test_LDADD		=	$(top_builddir)/src/libewd.la -lpthread
