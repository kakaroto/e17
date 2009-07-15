
bin_PROGRAMS = \
	etkxx_example_embed

etkxx_example_embed_SOURCES = \
	main.cpp

etkxx_example_embed_LDADD = \
	$(ETKXX_LIBS) \
	$(EFL_LIBS) \
	$(EDJEXX_LIBS) \
	$(ECOREXX_LIBS) \
	$(PACKAGE_SOURCE_DIR)/src/common/libeflxx_example_common.la

etkxx_example_embed_DEPENDENCIES = \
	$(PACKAGE_SOURCE_DIR)/src/common/libeflxx_example_common.la

AM_CPPFLAGS = \
	$(ETKXX_CFLAGS) \
	$(EFL_CFLAGS) \
	$(EDJEXX_CFLAGS) \
	$(ECOREXX_CFLAGS)

## File created by the gnome-build tools

