
bin_PROGRAMS = \
	esmartxx_example_group

esmartxx_example_group_SOURCES = \
	main.cpp

esmartxx_example_group_LDADD = \
	$(EDJEXX_LIBS) \
	$(EFL_LIBS) \
	$(PACKAGE_SOURCE_DIR)/src/common/libeflxx_example_common.la \
	$(ECOREXX_LIBS) \
	$(ESMARTXX_GROUP_LIBS)

esmartxx_example_group_DEPENDENCIES = \
	$(PACKAGE_SOURCE_DIR)/src/common/libeflxx_example_common.la

AM_CPPFLAGS = \
	$(EDJEXX_CFLAGS) \
	$(EFL_CFLAGS) \
	$(ECOREXX_CFLAGS) \
	$(ESMARTXX_GROUP_CFLAGS)

## File created by the gnome-build tools

