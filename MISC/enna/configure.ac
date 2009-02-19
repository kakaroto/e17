# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT([enna],[0.3.0],[www.digital-corner.org])
AC_PREREQ(2.61)
AC_CONFIG_SRCDIR(configure.ac)
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_ISC_POSIX

AM_INIT_AUTOMAKE(1.6 dist-bzip2)
AC_CONFIG_HEADERS([config.h])


AC_PROG_CC
AC_PROG_CC
AC_DIAGNOSE([obsolete],[AM_PROG_CC_STDC:
	your code should no longer depend upon `am_cv_prog_cc_stdc', but upon
	`ac_cv_prog_cc_stdc'.  Remove this warning and the assignment when
	you adjust the code.  You can also remove the above call to
	AC_PROG_CC if you already called it elsewhere.])
am_cv_prog_cc_stdc=$ac_cv_prog_cc_stdc

AC_HEADER_STDC
AC_C_CONST
AM_PROG_CC_C_O
AC_PROG_GCC_TRADITIONAL

define([AC_LIBTOOL_LANG_CXX_CONFIG], [:])dnl
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl

AC_PROG_LIBTOOL


PKG_CHECK_MODULES([ENNA],
  [
   eina-0 >= 0.0.1
   eet >= 1.0.0
   evas >= 0.9.9
   ecore >= 0.9.9.40
   ecore-evas
   ecore-txt
   ecore-file
   embryo >= 0.9.1
   edje >= 0.5.0
  ]
)

#########
# Theme #
#########

AC_ARG_ENABLE(theme,
	[AC_HELP_STRING([--disable-theme], [disable theme.
        [[default=enabled]]])],
	[build_theme=$enableval],
	[build_theme="yes"]
)
AM_CONDITIONAL([ENNA_BUILD_THEME], test "x${build_theme}" = "xyes")

####################
# Activity Modules #
####################

# Music
want_activity_music="yes"
AC_MSG_CHECKING(whether music activity module is to be built)
AC_ARG_ENABLE(activity-music,
  AS_HELP_STRING([--enable-activity-music],[Enable Music Activity Module]),
  [ want_activity_music=$enableval ]
)
AC_MSG_RESULT($want_activity_music)
AM_CONDITIONAL(BUILD_ACTIVITY_MUSIC, test "x$want_activity_music" = "xyes")
if test "x$want_activity_music" = "xyes"; then
  AC_DEFINE(BUILD_ACTIVITY_MUSIC, 1, [Music Activity])
fi

# Photo
want_activity_photo="yes"
AC_MSG_CHECKING(whether photo activity module is to be built)
AC_ARG_ENABLE(activity-photo,
  AS_HELP_STRING([--enable-activity-photo],[Enable Photo Activity Module]),
  [ want_activity_photo=$enableval ]
)
AC_MSG_RESULT($want_activity_photo)
AM_CONDITIONAL(BUILD_ACTIVITY_PHOTO, test "x$want_activity_photo" = "xyes")
if test "x$want_activity_photo" = "xyes"; then
  AC_DEFINE(BUILD_ACTIVITY_PHOTO, 1, [Photo Activity])
fi

# Video
want_activity_video="yes"
AC_MSG_CHECKING(whether video activity module is to be built)
AC_ARG_ENABLE(activity-video,
  AS_HELP_STRING([--enable-activity-video],[Enable Video Activity Module]),
  [ want_activity_video=$enableval ]
)
AC_MSG_RESULT($want_activity_video)
AM_CONDITIONAL(BUILD_ACTIVITY_VIDEO, test "x$want_activity_video" = "xyes")
if test "x$want_activity_video" = "xyes"; then
  AC_DEFINE(BUILD_ACTIVITY_VIDEO, 1, [Video Activity])
fi

###################
# Backend Modules #
###################

# Emotion
want_backend_emotion="auto"
have_backend_emotion="no"
AC_MSG_CHECKING(whether emotion backend is to be built)
AC_ARG_ENABLE(backend-emotion,
  AS_HELP_STRING([--enable-backend-emotion],[Enable Emotion media backend]),
  [ want_backend_emotion=$enableval ]
)
AC_MSG_RESULT($want_backend_emotion)
if test "x$want_backend_emotion" = "xyes" -o "x$want_backend_emotion" = "xauto"; then
  PKG_CHECK_MODULES(
    [EMOTION],
    [emotion],
    [
      AC_DEFINE(BUILD_BACKEND_EMOTION, 1, [Emotion media backend])
      have_backend_emotion="yes"
    ],
    [
      if test "x$want_backend_emotion" = "xyes" -a "x$use_strict" = "xyes" ; then
        AC_MSG_ERROR([Emotion not found (strict dependencies checking)])
      fi
    ]
  )
fi
AM_CONDITIONAL(BUILD_BACKEND_EMOTION, test "x$have_backend_emotion" = "xyes")

# libplayer
want_backend_libplayer="auto"
have_backend_libplayer="no"
AC_MSG_CHECKING(whether libplayer backend is to be built)
AC_ARG_ENABLE(backend-libplayer,
    AS_HELP_STRING([--enable-backend-libplayer],[Enable libplayer media backend]),
    [ want_backend_libplayer=$enableval ]
)
AC_MSG_RESULT($want_backend_libplayer)
if test "x$want_backend_libplayer" = "xyes" -o "x$want_backend_libplayer" = "xauto"; then
  PKG_CHECK_MODULES(
    [LIBPLAYER],
    [libplayer],
    [
      AC_DEFINE(BUILD_BACKEND_LIBPLAYER, 1, [libplayer media backend])
      have_backend_libplayer="yes"
    ],
    [
      if test "x$want_backend_libplayer" = "xyes" -a "x$use_strict" = "xyes" ; then
        AC_MSG_ERROR([libplayer not found (strict dependencies checking)])
      fi
    ]
  )
fi
AM_CONDITIONAL(BUILD_BACKEND_LIBPLAYER, test "x$have_backend_libplayer" = "xyes")

###################
# Browser Modules #
###################

# Light Media Scanner (LMS)
want_browser_lms="auto"
have_browser_lms="no"
AC_MSG_CHECKING(whether lms browser is to be built)
AC_ARG_ENABLE(browser-lms,
  AS_HELP_STRING([--enable-browser-lms],[Enable Light Media Scanner Browser Module]),
  [ want_browser_lms=$enableval ]
)
AC_MSG_RESULT($want_browser_lms)
if test "x$want_browser_lms" = "xyes" -o "x$want_browser_lms" = "xauto"; then
  PKG_CHECK_MODULES(
    [LMS],
    [lightmediascanner],
    [
      AC_DEFINE(BUILD_BROWSER_LMS, 1, [Light media scanner Module])
      have_browser_lms="yes"
    ],
    [
      if test "x$want_browser_lms" = "xyes" -a "x$use_strict" = "xyes" ; then
        AC_MSG_ERROR([Light media scanner not found (strict dependencies checking)])
      fi
    ]
  )
fi
AM_CONDITIONAL(BUILD_BROWSER_LMS, test "x$have_browser_lms" = "xyes")

# Local Files
want_browser_localfiles="yes"
AC_MSG_CHECKING(whether localfiles browser module is to be built)
AC_ARG_ENABLE(browser-localfiles,
  AS_HELP_STRING([--enable-browser-localfiles],[Enable Local Files Browser Module]),
  [ want_browser_localfiles=$enableval ]
)
AC_MSG_RESULT($want_browser_localfiles)
AM_CONDITIONAL(BUILD_BROWSER_LOCALFILES, test "x$want_browser_localfiles" = "xyes")
if test "x$want_browser_localfiles" = "xyes"; then
  AC_DEFINE(BUILD_BROWSER_LOCALFILES, 1, [Localfiles Browser])
fi

# NetStreams
want_browser_netstreams="yes"
AC_MSG_CHECKING(whether netstreams browser module is to be built)
AC_ARG_ENABLE(browser-netstreams,
  AS_HELP_STRING([--enable-browser-netstreams],[Enable NetStreams Browser Module]),
  [ want_browser_netstreams=$enableval ]
)
AC_MSG_RESULT($want_browser_netstreams)
AM_CONDITIONAL(BUILD_BROWSER_NETSTREAMS, test "x$want_browser_netstreams" = "xyes")
if test "x$want_browser_netstreams" = "xyes"; then
  AC_DEFINE(BUILD_BROWSER_NETSTREAMS, 1, [NetStreams Browser])
fi

# SHOUTcast
want_browser_shoutcast="yes"
AC_MSG_CHECKING(whether shoutcast browser module is to be built)
AC_ARG_ENABLE(browser-shoutcast,
  AS_HELP_STRING([--enable-browser-shoutcast],[Enable SHOUTcast Browser Module]),
  [ want_browser_shoutcast=$enableval ]
)
AC_MSG_RESULT($want_browser_shoutcast)
AM_CONDITIONAL(BUILD_BROWSER_SHOUTCAST, test "x$want_browser_shoutcast" = "xyes")
if test "x$want_browser_shoutcast" = "xyes"; then
  AC_DEFINE(BUILD_BROWSER_SHOUTCAST, 1, [SHOUTcast Browser])
fi

# UPnP/DLNA
want_browser_upnp="no"
have_browser_upnp="no"
AC_MSG_CHECKING(whether UPnP/DLNA browser is to be built)
AC_ARG_ENABLE(browser-upnp,
  AS_HELP_STRING([--enable-browser-upnp],[Enable UPnP/DLNA Browser Module]),
  [ want_browser_upnp=$enableval ]
)
AC_MSG_RESULT($want_browser_upnp)
if test "x$want_browser_upnp" = "xyes" -o "x$want_browser_upnp" = "xauto"; then
  PKG_CHECK_MODULES(
    [GUPNP],
    [gssdp-1.0 gupnp-1.0 gupnp-av-1.0],
    [
      AC_DEFINE(BUILD_BROWSER_UPNP, 1, [UPnP/DLNA Browser Module])
      have_browser_upnp="yes"
    ],
    [
      if test "x$want_browser_upnp" = "xyes" -a "x$use_strict" = "xyes" ; then
        AC_MSG_ERROR([gupnp-1.0 not found (strict dependencies checking)])
      fi
    ]
  )
fi
AM_CONDITIONAL(BUILD_BROWSER_UPNP, test "x$have_browser_upnp" = "xyes")

# DVD Video
want_browser_dvd="yes"
AC_MSG_CHECKING(whether dvd video browser module is to be built)
AC_ARG_ENABLE(browser-dvd,
  AS_HELP_STRING([--enable-browser-dvd],[Enable DVD Video Browser Module]),
  [ want_browser_dvd=$enableval ]
)
AC_MSG_RESULT($want_browser_dvd)
AM_CONDITIONAL(BUILD_BROWSER_DVD, test "x$want_browser_dvd" = "xyes")
if test "x$want_browser_dvd" = "xyes"; then
  AC_DEFINE(BUILD_BROWSER_DVD, 1, [Dvd Video Browser])
fi


#################
# Input Modules #
#################

# LIRC
have_input_lirc=no
AC_CHECK_HEADER(lirc/lirc_client.h, have_input_lirc=yes)
if test "x$have_input_lirc" = "xyes"; then
  AC_DEFINE([BUILD_INPUT_LIRC], [1], [Define if LIRC client lib is present])
  have_input_lirc="yes"
  LIRC_LIBS=-llirc_client
  AC_SUBST(LIRC_LIBS)
fi
AM_CONDITIONAL(BUILD_INPUT_LIRC, test "x$have_input_lirc" = "xyes")

####################
# Metadata Modules #
####################

# Amazon.com
want_metadata_amazon="yes"
AC_MSG_CHECKING(whether amazon.com metadata module is to be built)
AC_ARG_ENABLE(metadata-amazon,
  AS_HELP_STRING([--enable-metadata-amazon],[Enable Amazon Metadata Module]),
  [ want_metadata_amazon=$enableval ]
)
AC_MSG_RESULT($want_metadata_amazon)
AM_CONDITIONAL(BUILD_METADATA_AMAZON, test "x$want_metadata_amazon" = "xyes")
if test "x$want_metadata_amazon" = "xyes"; then
  AC_DEFINE(BUILD_METADATA_AMAZON, 1, [Amazon.com Metadata])
fi

# libplayer
want_metadata_libplayer="auto"
have_metadata_libplayer="no"
AC_MSG_CHECKING(whether libplayer metadata is to be built)
AC_ARG_ENABLE(metadata-libplayer,
    AS_HELP_STRING([--enable-metadata-libplayer],[Enable libplayer Metadata module]),
    [ want_metadata_libplayer=$enableval ]
)
AC_MSG_RESULT($want_metadata_libplayer)
if test "x$want_metadata_libplayer" = "xyes" -o "x$want_metadata_libplayer" = "xauto"; then
  PKG_CHECK_MODULES(
    [LIBPLAYER],
    [libplayer],
    [
      AC_DEFINE(BUILD_METADATA_LIBPLAYER, 1, [libplayer metadata module])
      have_metadata_libplayer="yes"
    ],
    [
      if test "x$want_metadata_libplayer" = "xyes" -a "x$use_strict" = "xyes" ; then
        AC_MSG_ERROR([libplayer not found (strict dependencies checking)])
      fi
    ]
  )
fi
AM_CONDITIONAL(BUILD_METADATA_LIBPLAYER, test "x$have_metadata_libplayer" = "xyes")

# Local Files
want_metadata_localfiles="yes"
AC_MSG_CHECKING(whether local files metadata module is to be built)
AC_ARG_ENABLE(metadata-localfiles,
  AS_HELP_STRING([--enable-metadata-localfiles],[Enable Local Files Metadata Module]),
  [ want_metadata_localfiles=$enableval ]
)
AC_MSG_RESULT($want_metadata_localfiles)
AM_CONDITIONAL(BUILD_METADATA_LOCALFILES, test "x$want_metadata_localfiles" = "xyes")
if test "x$want_metadata_localfiles" = "xyes"; then
  AC_DEFINE(BUILD_METADATA_LOCALFILES, 1, [Local Files Metadata])
fi

# TheMovieDB.org (TMDB)
want_metadata_tmdb="yes"
AC_MSG_CHECKING(whether themoviedb.org metadata module is to be built)
AC_ARG_ENABLE(metadata-tmdb,
  AS_HELP_STRING([--enable-metadata-tmdb],[Enable TheMovieDB Metadata Module]),
  [ want_metadata_tmdb=$enableval ]
)
AC_MSG_RESULT($want_metadata_tmdb)
AM_CONDITIONAL(BUILD_METADATA_TMDB, test "x$want_metadata_tmdb" = "xyes")
if test "x$want_metadata_tmdb" = "xyes"; then
  AC_DEFINE(BUILD_METADATA_TMDB, 1, [TheMovieDB.org Metadata])
fi

##################
# Volume Modules #
##################

# HAL
want_volume_hal="auto"
have_volume_hal="no"
AC_MSG_CHECKING(whether hal volume module is to be built)
AC_ARG_ENABLE(volume-hal,
  AS_HELP_STRING([--enable-volume-hal],[Enable HAL Volume]),
  [ want_volume_hal=$enableval ]
)
AC_MSG_RESULT($want_volume_hal)
if test "x$want_volume_hal" = "xyes" -o "x$want_volume_hal" = "xauto"; then
  PKG_CHECK_MODULES(
    [EHAL],
    [ehal hal-storage],
    [
      AC_DEFINE(BUILD_VOLUME_HAL, 1, [HAL Volume])
      have_volume_hal="yes"
    ],
    [
      if test "x$want_volume_hal" = "xyes" -a "x$use_strict" = "xyes" ; then
        AC_MSG_ERROR([ehal or hal-storage not found (strict dependencies checking)])
      fi
    ]
  )
fi
AM_CONDITIONAL(BUILD_VOLUME_HAL, test "x$have_volume_hal" = "xyes")

# mtab
want_volume_mtab="yes"
AC_MSG_CHECKING(whether mtab volume module is to be built)
AC_ARG_ENABLE(volume-mtab,
  AS_HELP_STRING([--enable-volume-mtab],[Enable mtab Volume Module]),
  [ want_volume_mtab=$enableval ]
)
AC_MSG_RESULT($want_volume_mtab)
AM_CONDITIONAL(BUILD_VOLUME_MTAB, test "x$want_volume_mtab" = "xyes")
if test "x$want_volume_mtab" = "xyes"; then
  AC_DEFINE(BUILD_VOLUME_MTAB, 1, [mtab Volume])
fi

##############
# Librairies #
##############

# libcddb
want_libcddb="auto"
have_libcddb="no"
AC_MSG_CHECKING(whether libcddb support is to be built)
AC_ARG_ENABLE(libcddb,
    AS_HELP_STRING([--enable-libcddb],[Enable libcddb support]),
    [ want_libcddb=$enableval ]
)
AC_MSG_RESULT($want_libcddb)
if test "x$want_libcddb" = "xyes" -o "x$want_libcddb" = "xauto"; then
  PKG_CHECK_MODULES(
    [LIBCDDB],
    [libcddb],
    [
      AC_DEFINE(BUILD_LIBCDDB, 1, [libcddb support])
      have_libcddb="yes"
    ],
    [
      if test "x$want_libcddb" = "xyes" -a "x$use_strict" = "xyes" ; then
        AC_MSG_ERROR([libcddb not found (strict dependencies checking)])
      fi
    ]
  )
fi
AM_CONDITIONAL(BUILD_LIBCDDB, test "x$have_libcddb" = "xyes")

# libcurl
want_libcurl="auto"
have_libcurl="no"
AC_MSG_CHECKING(whether libcurl support is to be built)
AC_ARG_ENABLE(libcurl,
    AS_HELP_STRING([--enable-libcurl],[Enable libcurl support]),
    [ want_libcurl=$enableval ]
)
AC_MSG_RESULT($want_libcurl)
if test "x$want_libcurl" = "xyes" -o "x$want_libcurl" = "xauto"; then
  PKG_CHECK_MODULES(
    [LIBCURL],
    [libcurl],
    [
      AC_DEFINE(BUILD_LIBCURL, 1, [lib support])
      have_libcurl="yes"
    ],
    [
      if test "x$want_libcurl" = "xyes" -a "x$use_strict" = "xyes" ; then
        AC_MSG_ERROR([libcurl not found (strict dependencies checking)])
      fi
    ]
  )
fi
AM_CONDITIONAL(BUILD_LIBCURL, test "x$have_libcurl" = "xyes")

# libexif
want_libexif="auto"
have_libexif="no"
AC_MSG_CHECKING(whether libexif support is to be built)
AC_ARG_ENABLE(libexif,
    AS_HELP_STRING([--enable-libexif],[Enable libexif support]),
    [ want_libexif=$enableval ]
)
AC_MSG_RESULT($want_libexif)
if test "x$want_libexif" = "xyes" -o "x$want_libexif" = "xauto"; then
  PKG_CHECK_MODULES(
    [LIBEXIF],
    [libexif],
    [
      AC_DEFINE(BUILD_LIBEXIF, 1, [libexif support])
      have_libexif="yes"
    ],
    [
      if test "x$want_libexif" = "xyes" -a "x$use_strict" = "xyes" ; then
        AC_MSG_ERROR([libexif not found (strict dependencies checking)])
      fi
    ]
  )
fi
AM_CONDITIONAL(BUILD_LIBEXIF, test "x$have_libexif" = "xyes")

# libxml2
want_libxml2="auto"
have_libxml2="no"
AC_MSG_CHECKING(whether libxml2 support is to be built)
AC_ARG_ENABLE(libxml2,
    AS_HELP_STRING([--enable-libxml2],[Enable libxml2 support]),
    [ want_libxml2=$enableval ]
)
AC_MSG_RESULT($want_libxml2)
if test "x$want_libxml2" = "xyes" -o "x$want_libxml2" = "xauto"; then
  PKG_CHECK_MODULES(
    [LIBXML2],
    [libxml-2.0],
    [
      AC_DEFINE(BUILD_LIBXML2, 1, [libxml2 support])
      have_libxml2="yes"
    ],
    [
      if test "x$want_libxml2" = "xyes" -a "x$use_strict" = "xyes" ; then
        AC_MSG_ERROR([libxml2 not found (strict dependencies checking)])
      fi
    ]
  )
fi
AM_CONDITIONAL(BUILD_LIBXML2, test "x$have_libxml2" = "xyes")

if test "x$GCC" = "xyes"; then
  CFLAGS="$CFLAGS -Wall -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -D_REENTRANT"
fi


AC_CONFIG_FILES([
Makefile
src/Makefile
src/bin/Makefile
src/modules/Makefile
src/modules/activity/Makefile
src/modules/activity/music/Makefile
src/modules/activity/photo/Makefile
src/modules/activity/video/Makefile
src/modules/backend/Makefile
src/modules/backend/emotion/Makefile
src/modules/backend/libplayer/Makefile
src/modules/browser/Makefile
src/modules/browser/lms/Makefile
src/modules/browser/localfiles/Makefile
src/modules/browser/netstreams/Makefile
src/modules/browser/shoutcast/Makefile
src/modules/browser/upnp/Makefile
src/modules/browser/dvd/Makefile
src/modules/input/Makefile
src/modules/input/lirc/Makefile
src/modules/metadata/Makefile
src/modules/metadata/amazon/Makefile
src/modules/metadata/libplayer/Makefile
src/modules/metadata/localfiles/Makefile
src/modules/metadata/tmdb/Makefile
src/modules/volume/Makefile
src/modules/volume/hal/Makefile
src/modules/volume/mtab/Makefile
data/Makefile
data/other/Makefile
data/theme/Makefile
data/theme/default/Makefile
])

AC_OUTPUT

echo
echo "$PACKAGE $VERSION"
echo
echo "Build theme........................ : $build_theme"
echo
echo
echo "Supported Activity Modules:"
echo
echo "  music............................ : $want_activity_music"
echo "  photo............................ : $want_activity_photo"
echo "  video............................ : $want_activity_video"
echo
echo "Supported Backends Modules:"
echo
echo "  emotion.......................... : $have_backend_emotion"
echo "  libplayer........................ : $have_backend_libplayer"
echo
echo "Supported Browser Modules:"
echo
echo "  Light Media Scanner.............. : $have_browser_lms"
echo "  Local files...................... : $want_browser_localfiles"
echo "  Network Streams.................. : $want_browser_netstreams"
echo "  SHOUTcast........................ : $want_browser_shoutcast"
echo "  UPnP/DLNA........................ : $have_browser_upnp"
echo "  DVD Video........................ : $want_browser_dvd"
echo
echo "Supported Input Modules:"
echo
echo "  LIRC client...................... : $have_input_lirc"
echo
echo "Supported Metadata Modules:"
echo
echo "  Amazon.com....................... : $want_metadata_amazon"
echo "  libplayer........................ : $have_metadata_libplayer"
echo "  Local files...................... : $want_metadata_localfiles"
echo "  TheMovieDB.org................... : $want_metadata_tmdb"
echo
echo "Supported Volume Modules:"
echo
echo "  HAL.............................. : $have_volume_hal"
echo "  mtab............................. : $want_volume_mtab"
echo
echo "Optional linked-in librairies:"
echo
echo "  libcddb.......................... : $have_libcddb"
echo "  libcurl.......................... : $have_libcurl"
echo "  libexif.......................... : $have_libexif"
echo "  libxml2.......................... : $have_libxml2"
echo
echo "Now type 'make' ('gmake' on some systems) to compile $PACKAGE."
echo

