## Process this file with automake to produce Makefile.in

INCLUDES = @ecore_cflags@ @edje_cflags@ @esmart_cflags@ @etox_cflags@ @xml2_cflags@

bin_PROGRAMS = erss

erss_SOURCES = erss.c parse.c parse_config.c tooltip.c net.c ls.c gui.c

erss_LDADD = @ecore_libs@ @edje_libs@ @etox_libs@ @xml2_libs@ \
				 @esmart_libs@ -lesmart_container -lesmart_trans_x11
