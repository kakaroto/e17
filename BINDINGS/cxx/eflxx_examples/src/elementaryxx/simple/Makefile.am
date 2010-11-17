
bin_PROGRAMS = \
	elementaryxx_example_simple

elementaryxx_example_simple_SOURCES = \
	main.cpp

elementaryxx_example_simple_LDADD = \
	$(ELEMENTARYXX_LIBS) \
	$(EVASXX_LIBS) \
	$(PACKAGE_SOURCE_DIR)/src/common/libeflxx_example_common.la

elementaryxx_example_simple_DEPENDENCIES = \
        $(PACKAGE_SOURCE_DIR)/src/common/libeflxx_example_common.la

AM_CPPFLAGS = \
	$(ELEMENTARYXX_CFLAGS) \
	$(EVASXX_CFLAGS)

## File created by the gnome-build tools

