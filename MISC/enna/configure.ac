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

have_lirc_module=no
AC_CHECK_HEADER(lirc/lirc_client.h, have_lirc_module=yes)
if test "x$have_lirc_module" = "xyes"; then
  AC_DEFINE([BUILD_LIRC_MODULE], [1], [Define if LIRC client lib is present])
  have_lirc_module="yes"
  LIRC_LIBS=-llirc_client
  AC_SUBST(LIRC_LIBS)
fi
AM_CONDITIONAL(BUILD_LIRC_MODULE, test "x$have_lirc_module" = "xyes")

#######################################
## Check if we should build the emotion backend
want_emotion_backend="auto"
have_emotion_backend="no"
AC_MSG_CHECKING(whether emotion backend is to be built)
AC_ARG_ENABLE(emotion-backend,
  AS_HELP_STRING([--enable-emotion-backend],[Enable Emotion media backend]),
  [ want_emotion_backend=$enableval ]
)
AC_MSG_RESULT($want_emotion_backend)
if test "x$want_emotion_backend" = "xyes" -o "x$want_emotion_backend" = "xauto"; then
  PKG_CHECK_MODULES(
    [EMOTION],
    [emotion],
    [
      AC_DEFINE(BUILD_EMOTION_BACKEND, 1, [Emotion media backend])
      have_emotion_backend="yes"
    ],
    [
      if test "x$want_emotion_backend" = "xyes" -a "x$use_strict" = "xyes" ; then
        AC_MSG_ERROR([Emotion not found (strict dependencies checking)])
      fi
    ]
  )
fi
AM_CONDITIONAL(BUILD_EMOTION_BACKEND, test "x$have_emotion_backend" = "xyes")

#######################################
## Check if we should build the libplayer backend
want_libplayer_backend="auto"
have_libplayer_backend="no"

AC_MSG_CHECKING(whether libplayer backend is to be built)
AC_ARG_ENABLE(libplayer-backend,
    AS_HELP_STRING([--enable-libplayer-backend],[Enable libplayer media backend]),
    [ want_libplayer_backend=$enableval ]
)
AC_MSG_RESULT($want_libplayer_backend)
if test "x$want_libplayer_backend" = "xyes" -o "x$want_libplayer_backend" = "xauto"; then
  PKG_CHECK_MODULES(
    [LIBPLAYER],
    [libplayer],
    [
      AC_DEFINE(BUILD_LIBPLAYER_BACKEND, 1, [Libplayer media backend])
      have_libplayer_backend="yes"
    ],
    [
      if test "x$want_libplayer_backend" = "xyes" -a "x$use_strict" = "xyes" ; then
        AC_MSG_ERROR([libplayer not found (strict dependencies checking)])
      fi
    ]
  )
fi
AM_CONDITIONAL(BUILD_LIBPLAYER_BACKEND, test "x$have_libplayer_backend" = "xyes")

#######################################
## Check if we should build with cURL support
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

#######################################
## Check if we should build with libxml2 support
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


#######################################
## Check if we should build light media scanner module
want_lms_module="auto"
have_lms_module="no"
AC_MSG_CHECKING(whether lms is to be built)
AC_ARG_ENABLE(lms-module,
  AS_HELP_STRING([--enable-lms-module],[Enable Light Media Scanner Module]),
  [ want_lms_module=$enableval ]
)
AC_MSG_RESULT($want_lms_module)
if test "x$want_lms_module" = "xyes" -o "x$want_lms_module" = "xauto"; then
  PKG_CHECK_MODULES(
    [LMS],
    [lightmediascanner],
    [
      AC_DEFINE(BUILD_LMS_MODULE, 1, [Light media scanner Module])
      have_lms_module="yes"
    ],
    [
      if test "x$want_lms_module" = "xyes" -a "x$use_strict" = "xyes" ; then
        AC_MSG_ERROR([Light media scanner not found (strict dependencies checking)])
      fi
    ]
  )
fi
AM_CONDITIONAL(BUILD_LMS_MODULE, test "x$have_lms_module" = "xyes")

#######################################
## Check if we should build AMazon module
want_amazon_module="yes"
AC_MSG_CHECKING(whether amazon is to be built)
AC_ARG_ENABLE(amazon-module,
  AS_HELP_STRING([--enable-amazon-module],[Enable Amazon Module]),
  [ want_amazon_module=$enableval ]
)
AC_MSG_RESULT($want_amazon_module)
AM_CONDITIONAL(BUILD_AMAZON_MODULE, test "x$want_amazon_module" = "xyes")

#######################################
## Check if we should build SHOUTcast module
want_shoutcast_module="yes"
AC_MSG_CHECKING(whether SHOUTcast is to be built)
AC_ARG_ENABLE(shoutcast-module,
  AS_HELP_STRING([--enable-shoutcast-module],[Enable SHOUTcast Module]),
  [ want_shoutcast_module=$enableval ]
)
AC_MSG_RESULT($want_shoutcast_module)
AM_CONDITIONAL(BUILD_SHOUTCAST_MODULE, test "x$want_shoutcast_module" = "xyes")

AC_CONFIG_FILES([
Makefile
src/Makefile
src/bin/Makefile
src/modules/Makefile
src/modules/amazon/Makefile
src/modules/music/Makefile
src/modules/video/Makefile
src/modules/photo/Makefile
src/modules/browser/Makefile
src/modules/browser/localfiles/Makefile
src/modules/browser/lms/Makefile
src/modules/browser/shoutcast/Makefile
src/modules/backend/Makefile
src/modules/backend/emotion/Makefile
src/modules/backend/libplayer/Makefile
src/modules/input/Makefile
src/modules/input/lirc/Makefile
data/Makefile
data/theme/Makefile
data/theme/default/Makefile
])

AC_OUTPUT

echo
echo "$PACKAGE $VERSION"
echo
echo "Built-in librairies Modules:"
echo
echo "  libcurl.......................... : $have_libcurl"
echo "  libxml2.......................... : $have_libxml2"
echo
echo "Optional Playback Backends:"
echo
echo "  emotion backend.................. : $have_emotion_backend"
echo "  libplayer backend................ : $have_libplayer_backend"
echo
echo "Optional Browser Modules:"
echo
echo "  Light Media Scanner.............. : $have_lms_module"
echo "  SHOUTcast........................ : $want_shoutcast_module"
echo
echo "Miscellaneous Modules:"
echo
echo "  lirc client...................... : $have_lirc_module"
echo "  Amazon.com....................... : $want_amazon_module"
echo
echo "Now type 'make' ('gmake' on some systems) to compile $PACKAGE."
echo
