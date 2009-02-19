## Process this file with automake to produce Makefile.in

INCLUDES = \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DPACKAGE_DATA_DIR=\""$(datadir)"\"

AM_CFLAGS = @ENNA_CFLAGS@ @LIBXML2_CFLAGS@ @LIBCURL_CFLAGS@ @LIBCDDB_CFLAGS@

bin_PROGRAMS = enna

enna_SOURCES = \
enna.c\
enna_config.c\
utils.c\
metadata.c\
box.c\
mainmenu.c\
background.c\
module.c\
activity.c\
vfs.c\
volumes.c\
image.c\
list.c\
browser.c\
content.c\
pan.c\
scrollframe.c\
wall.c\
button.c\
switcher.c\
location.c\
mediaplayer.c\
event_key.c\
list_item.c\
xml_utils.c\
url_utils.c\
md5.c\
logs.c\
parser_cdda.c\
slideshow.c\
mediacontrol.c\
carousel.c\
reflection.c

enna_LDADD = @ENNA_LIBS@ @LIBXML2_LIBS@ @LIBCURL_LIBS@ @LIBCDDB_LIBS@
enna_LDFLAGS = -rdynamic

EXTRA_DIST= \
enna.h\
enna_inc.h\
enna_config.h\
utils.h\
metadata.h\
box.h\
mainmenu.h\
background.h\
module.h\
vfs.h\
volumes.h\
activity.h\
image.h\
list.h\
browser.h\
content.h\
pan.h\
scrollframe.h\
wall.h\
button.h\
switcher.h\
location.h\
mediaplayer.h\
event_key.h\
list_item.h\
xml_utils.h\
url_utils.h\
md5.h\
logs.h\
parser_cdda.h\
slideshow.h\
mediacontrol.h\
carousel.h\
reflection.h


