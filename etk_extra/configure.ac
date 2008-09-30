dnl Process this file with autoconf to produce a configure script.

AC_INIT(configure.ac)
AM_INIT_AUTOMAKE(etk_extra, 0.0.1.0)
AM_CONFIG_HEADER(src/config.h)

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_C_BIGENDIAN
AC_C_CONST
AM_ENABLE_SHARED
AM_PROG_LIBTOOL

dnl Use -Wall if we have gcc.
							
dnl Set PACKAGE_DATA_DIR in config.h.
if test "x${prefix}" = "xNONE"; then
  AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${ac_default_prefix}/share/${PACKAGE}", [Shared Data Directory])
else
  AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${prefix}/share/${PACKAGE}", [Shared Data Directory])
fi

if test "x${prefix}" = "xNONE"; then
  container_plugin_dir=${ac_default_prefix}/lib/${PACKAGE}/layout
  AC_DEFINE_UNQUOTED(PREFIX, "${ac_default_prefix}", [Installation Prefix])
else
  AC_DEFINE_UNQUOTED(PREFIX, "${prefix}", [Installation Prefix])
  container_plugin_dir=${prefix}/lib/${PACKAGE}/layout
fi

AC_DEFINE_UNQUOTED(CONTAINER_PLUGIN_DIR, "${container_plugin_dir}", [Container Plugin Directory])
AC_SUBST(CONTAINER_PLUGIN_DIR)

changequote(,)dnl
if test "x$GCC" = "xyes"; then
  case " $CFLAGS " in
  *[\ \	]-Wall[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wall " ;;
  esac
fi
changequote([,])dnl

AC_CHECK_HEADER(ltdl.h,,
                AC_MSG_ERROR(Cannot find ltdl.h. Please make sure you have the libltdl-dev package installed.))

#check for ETK
PKG_CHECK_MODULES(ETK, [etk >= 0.1.0.001],
   [
     AC_DEFINE(HAVE_ETK, 1, have ETK)
     etk="ETK "
     AM_CONDITIONAL(HAVE_ETK, true)
     have_etk="yes"
     etk_libs=$ETK_LIBS
     etk_cflags=$ETK_CFLAGS
   ],
   [
     AC_MSG_ERROR([No Etk, can't build.])
   ]
)

#check for Emotion
PKG_CHECK_MODULES(EMOTION, [emotion >= 0.0.1.004],
   [
     AC_DEFINE(HAVE_EMOTION, 1, have Emotion)
     engrave="Emotion "
     AM_CONDITIONAL(HAVE_EMOTION, true)
     have_emotion="yes"
     emotion_libs=$EMOTION_LIBS
     emotion_cflags=$EMOTION_CFLAGS
   ],
   [
     AC_DEFINE(HAVE_EMOTION, 0, no Emotion)
     AM_CONDITIONAL(HAVE_EMOTION, false)
     AC_MSG_WARN([No Emotion, can't build etk video widget.])
     have_emotion="no"
     emotion_libs=""
     emotion_cflags=""
   ]
)

#check for Cairo
PKG_CHECK_MODULES(CAIRO, [cairo >= 1.1.4],
   [
     AC_DEFINE(HAVE_CAIRO, 1, have Cairo)
     cairo="Cairo "
     AM_CONDITIONAL(HAVE_CAIRO, true)
     have_cairo="yes"
     cairo_libs=$CAIRO_LIBS
     cairo_cflags=$CAIRO_CFLAGS
   ],
   [
     AC_DEFINE(HAVE_CAIRO, 0, no Cairo)
     AM_CONDITIONAL(HAVE_CAIRO, false)
     AC_MSG_WARN([No Cairo, can't build etk cairo widget.])
     have_cairo="no"
     cairo_libs=""
     cairo_cflags=""
   ]
)
     	      
libs="$etk_video_libs -letk_tree_model_wobbly"

AC_SUBST(libs)
AC_SUBST(packagesrcdir)

if test "x$have_emotion" = "xyes" ; then
  etk_extra_video_pc="etk_extra_video.pc"
fi

if test "x$have_cairo" = "xyes" ; then
  etk_extra_cairo_pc="etk_extra_cairo.pc"
fi


AC_OUTPUT([
Makefile
etk_extra_tree_model_wobbly.pc
etk_extra_video.pc
etk_extra_cairo.pc
src/Makefile
src/lib/Makefile
src/lib/etk_video/Makefile
src/lib/etk_tree_model_wobbly/Makefile
src/lib/etk_cairo/Makefile
], [
]
)

echo
echo
echo
echo "------------------------------------------------------------------------"
echo "$PACKAGE $VERSION"
echo "------------------------------------------------------------------------"
echo
echo
echo "Configuration Options Summary:"
echo
echo "  Etk Wobbly Tree Model.......................: $have_etk"
echo "  Etk Video Widget............................: $have_emotion"
echo "  Etk Cairo Widget............................: $have_cairo"
echo
echo "Installation Path.........: $prefix"
echo
echo "Now type 'make' ('gmake' on some systems) to compile $PACKAGE,"
echo "and then afterwards as root (or the user who will install this), type"
echo "'make install'. Change users with 'su' or 'sudo' appropriately."
echo
