
bin_PROGRAMS = \
	elementaryxx_example_external

elementaryxx_example_external_SOURCES = \
	main.cpp

elementaryxx_example_external_LDADD = \
	$(ELEMENTARYXX_LIBS) \
	$(ECOREXX_LIBS) \
	$(EDJEXX_LIBS) \
	$(PACKAGE_SOURCE_DIR)/src/common/libeflxx_example_common.la

elementaryxx_example_external_DEPENDENCIES = \
	$(PACKAGE_SOURCE_DIR)/src/common/libeflxx_example_common.la

AM_CPPFLAGS = \
	$(ELEMENTARYXX_CFLAGS) \
	$(ECOREXX_CFLAGS) \
	$(EDJEXX_CFLAGS)

## File created by the gnome-build tools

