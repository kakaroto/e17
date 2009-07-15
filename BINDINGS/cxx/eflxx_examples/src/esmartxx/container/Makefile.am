
bin_PROGRAMS = \
	esmartxx_example_container

esmartxx_example_container_SOURCES = \
	main.cpp

esmartxx_example_container_LDADD = \
	$(EDJEXX_LIBS) \
	$(ESMARTXX_LIBS) \
	$(EFL_LIBS) \
	$(PACKAGE_SOURCE_DIR)/src/common/libeflxx_example_common.la \
	$(ECOREXX_LIBS) \
	$(ESMARTXX_CONTAINER_LIBS)

esmartxx_example_container_DEPENDENCIES = \
	$(PACKAGE_SOURCE_DIR)/src/common/libeflxx_example_common.la

AM_CPPFLAGS = \
	$(EDJEXX_CFLAGS) \
	$(ESMARTXX_CFLAGS) \
	$(EFL_CFLAGS) \
	$(ECOREXX_CFLAGS) \
	$(ESMARTXX_CONTAINER_CFLAGS)

## File created by the gnome-build tools

