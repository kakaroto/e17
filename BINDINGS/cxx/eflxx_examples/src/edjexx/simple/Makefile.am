
bin_PROGRAMS = \
	edjexx_example_simple

edjexx_example_simple_SOURCES = \
	main.cpp

edjexx_example_simple_LDADD = \
	$(EDJEXX_LIBS) \
	$(ECOREXX_LIBS) \
	$(EFL_LIBS) \
	$(PACKAGE_SOURCE_DIR)/src/common/libeflxx_example_common.la

edjexx_example_simple_DEPENDENCIES = \
	$(PACKAGE_SOURCE_DIR)/src/common/libeflxx_example_common.la

AM_CPPFLAGS = \
	$(EDJEXX_CFLAGS) \
	$(ECOREXX_CFLAGS) \
	$(EFL_CFLAGS)

## File created by the gnome-build tools

