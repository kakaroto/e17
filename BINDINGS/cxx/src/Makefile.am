## Process this file with automake to produce Makefile.in
## Created by Anjuta - will *NOT* be overwritten

SUBDIRS = common ecore edje esmart evas eet emotion etk ewl misc

INCLUDES = 

AM_CXXFLAGS = @PACKAGE_CFLAGS@ \
	 -Wall

lib_LTLIBRARIES = libeflpp.la

libeflpp_la_SOURCES = 

libeflpp_la_LDFLAGS =	common/libeflpp_common.la\
			ecore/libeflpp_ecore.la\
			edje/libeflpp_edje.la\
			eet/libeflpp_eet.la\
			emotion/libeflpp_emotion.la\
			esmart/libeflpp_esmart.la\
			etk/libeflpp_etk.la\
			evas/libeflpp_evas.la\
			ewl/libeflpp_ewl.la\
			misc/libeflpp_misc.la
 
libeflpp_la_DEPENDENCIES = common/libeflpp_common.la\
                        ecore/libeflpp_ecore.la\
                        edje/libeflpp_edje.la\
                        eet/libeflpp_eet.la\
                        emotion/libeflpp_emotion.la\
                        esmart/libeflpp_esmart.la\
                        etk/libeflpp_etk.la\
                        evas/libeflpp_evas.la\
                        ewl/libeflpp_ewl.la\
                        misc/libeflpp_misc.la
 


libeflpp_la_LIBADD = @PACKAGE_LIBS@
