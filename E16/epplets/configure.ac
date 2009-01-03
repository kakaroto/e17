m4_define([pkg_version],  [0.12])
dnl m4_define([pkg_revision], [0.00])
m4_define([pkg_version],  m4_ifdef([pkg_revision], [pkg_version-pkg_revision], [pkg_version]))
m4_define([rpm_revision], m4_ifdef([pkg_revision], [pkg_revision], [1]))

AC_INIT(epplets, [pkg_version], [enlightenment-devel@lists.sourceforge.net])
AM_INIT_AUTOMAKE([foreign])

AC_SUBST(E_RPM_REVISION, [rpm_revision])

dnl reasonable guesses for where stuff is installed
if test "x$prefix" = "xNONE"; then
  prefix=$ac_default_prefix
fi
if test "x$exec_prefix" = "xNONE"; then
  exec_prefix=$prefix
fi

AC_PROG_CC
AC_PROG_CPP

define([AC_LIBTOOL_LANG_CXX_CONFIG], [:])dnl
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl
define([AC_LIBTOOL_LANG_GCJ_CONFIG], [:])dnl
AC_ENABLE_SHARED
AC_PROG_LIBTOOL

AC_GCC_TRADITIONAL

AC_PROG_INSTALL

dnl# Check the sanity of what we've done so far
AM_SANITY_CHECK

dnl# Most people don't want the developer-only clutter
AM_MAINTAINER_MODE

dnl# Look for needed programs
AC_CHECK_PROG(RM, rm, rm, true)
AC_CHECK_PROG(CP, cp, cp, false)
AC_CHECK_PROG(CHMOD, chmod, chmod, true)
AC_CHECK_PROG(TAR, tar, tar, tar)
AC_CHECK_PROG(MKDIR, mkdir, mkdir, false)
AC_CHECK_PROG(AR, ar, ar, false)
AC_CHECK_PROG(MV, mv, mv, true)

AC_C_CONST
AC_C_INLINE

AC_PATH_XTRA

dnl# Checks for header files.
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS(unistd.h time.h math.h pwd.h sys/types.h varargs.h stdarg.h)
AC_HEADER_TIME

dnl# Missing typedefs and replacements
AC_TYPE_MODE_T
AC_TYPE_PID_T
AC_TYPE_UID_T

dnl# Checks for library functions.
AC_TYPE_SIGNAL
AC_CHECK_FUNCS(snprintf, x=1, x=0)
AC_SUBST(HAVE_SNPRINTF, $x)
AM_CONDITIONAL(BUILD_SNPRINTF, test "x$x" = "x0")

AC_ARG_ENABLE(glx,
  [  --enable-glx            build with GLX support @<:@default=yes@:>@],,
  enable_glx=yes)
if test "x$enable_glx" = "xyes"; then
  AC_CHECK_HEADERS(GL/gl.h GL/glx.h,, enable_glx=no)
  AC_CHECK_LIB(GL, glXChooseVisual, enable_glx=yes, enable_glx=no)
fi
if test "x$enable_glx" = "xyes"; then
  GLX_LIBS="-lGL"
  GLX_PROGS="E-OpenGL-Demo.epplet"
  AC_DEFINE(HAVE_GLX, 1, [GLX support])
else
  AC_MSG_WARN([*** GL epplets will not be built ***])
fi
AC_SUBST(GLX_LIBS)
AC_SUBST(GLX_PROGS)
AM_CONDITIONAL(BUILD_GLX, test "x$enable_glx" = "xyes")

PKG_CHECK_MODULES(IMLIB2, imlib2 >= 1.2.0)

AC_CHECK_LIB(kstat, kstat_open, KSTAT_LIBS="-lkstat")
AC_SUBST(KSTAT_LIBS)

AC_MSG_CHECKING(for Linux)
if test "`uname -s`" != "Linux"; then
  AC_MSG_RESULT(no)
  AC_CHECK_PROG(LIBGTOP_CONFIG, libgtop-config, libgtop-config, no)
else
  AC_MSG_RESULT(yes)
  LIBGTOP_CONFIG=no
fi

if test "$LIBGTOP_CONFIG" != "no"; then
  
  LIBGTOP_CONF_SH="`$LIBGTOP_CONFIG --config`"
  if test ! -z "$LIBGTOP_CONF_SH"; then
    . $LIBGTOP_CONF_SH

    AC_MSG_CHECKING(libgtop-config for the value of CFLAGS)
    CFLAGS="$CFLAGS $LIBGTOP_INCS"
    AC_DEFINE(HAVE_LIBGTOP, 1, [Have libgtop])
    AC_MSG_RESULT($CFLAGS)

    AC_MSG_CHECKING(libgtop-config for the value of LIBS)
    GTLIBS="$GTLIBS $LIBGTOP_LIBS $LIBGTOP_EXTRA_LIBS"
    AC_MSG_RESULT($GTLIBS)

    SAVE_LIBS="$LIBS"
    LIBS="$GTLIBS"

    AC_MSG_CHECKING(the sanity of new compile/link flags)
    AC_TRY_LINK(, , echo "yes", echo "no" ; LIBGTOP_CONFIG="no" ; 
                AC_WARN(Compile/link failed.)
    )

    LIBS="$SAVE_LIBS"
  fi
else
  GTLIBS=""
fi
AC_SUBST(GTLIBS)


AC_ARG_ENABLE(auto-respawn,
  [  --disable-auto-respawn  don't respawn epplets through e by default @<:@default=no@:>@],
          no_respawn=yes,)
if test ! -z "$no_respawn"; then
  AC_DEFINE(NO_AUTO_RESPAWN, 1, [Disable auto-respawning])
fi

AC_ARG_ENABLE(old-remember,
  [  --enable-old-remember   use if you have a version of enlightenment < 0.16.8 @<:@default=no@:>@],
  enable_old_remember=no)
if test "x$enable_old_remember" = "xno"; then
  AC_DEFINE(USE_OLD_REMEMBER, 1, [Use old remember mode (e16 < 0.16.8)])
fi


# Build test programs?
AC_ARG_WITH(test-programs,
  [  --with-test-programs    builds the test programs @<:@default=no@:>@],
  TEST_PROGS="EppletTest.epplet EppletConfigTest.epplet", TEST_PROGS="")
AC_SUBST(TEST_PROGS)

# Should we install E-Mixer?
# Probably not ;-) But we may as well check!
P_MIXER=
AC_CHECK_HEADERS(linux/soundcard.h,[P_MIXER=mixer],[
   AC_CHECK_HEADERS(machine/soundcard.h, [P_MIXER=mixer],[
     AC_CHECK_HEADERS(dmedia/audio.h, [P_MIXER=mixer],[
      AC_CHECK_HEADERS(sys/soundcard.h, [P_MIXER=mixer]) ]) ]) ])
if test "$P_MIXER" = "mixer"; then
  MIXER_PROGS="E-Mixer.epplet Emix.epplet"
else
  AC_MSG_WARN([*** Mixer epplets will not be built ***])
  MIXER_PROGS=""
fi
AC_SUBST(MIXER_PROGS)

SGI=
AC_CHECK_HEADERS(dmedia/audio.h,[SGI=sgi])
if test "$SGI" = "sgi"; then
  SGI_LIBS="-laudio"
  SGI_DEFINES="-DSGI_AUDIO"
else
  SGI_LIBS=""
  SGI_DEFINES=""
fi
AC_SUBST(SGI_LIBS)
AC_SUBST(SGI_DEFINES)


AC_ARG_ENABLE(cdaudio,
  [  --enable-cdaudio        build epplets using libcdaudio @<:@default=yes@:>@],,
  enable_cdaudio=yes)
if test "x$enable_cdaudio" = "xyes"; then
  AC_CHECK_LIB(cdaudio, cd_advance, PLAYCD_PROGS=E-PlayCD.epplet, PLAYCD_PROGS="")
fi
AC_SUBST(PLAYCD_PROGS)

AC_ARG_ENABLE(esd,
  [  --enable-esd            build epplets using libesd @<:@default=yes@:>@],,
  enable_esd=yes)
if test "x$enable_esd" = "xyes"; then
  AC_CHECK_LIB(esd, esd_open_sound, ESD_PROGS=E-SD.epplet, ESD_PROGS="")
fi
AC_SUBST(ESD_PROGS)

# Stuff for E-Mountbox
AC_PATH_PROG(MOUNT, mount)
AC_PATH_PROG(UMOUNT, umount)
AC_PATH_PROG(EJECT, eject)
AC_DEFINE_UNQUOTED(MOUNT, "$MOUNT", [Location of mount])
AC_DEFINE_UNQUOTED(UMOUNT, "$UMOUNT", [Location of umount])
AC_DEFINE_UNQUOTED(EJECT, "$EJECT", [Location of eject])
if test -n "$EJECT"; then
    AC_DEFINE(HAVE_EJECT, 1, [Have eject])
fi

AM_CONFIG_HEADER(config.h)

AC_OUTPUT([
Makefile
api/Makefile
epplets/Makefile
api/epplet.h
epplets.spec
])

echo
echo "Now type 'make' to build $PACKAGE $VERSION."
echo "Then type 'make install' to install."
echo
echo "If you are installing epplets for the first time, don't forget"
echo "to regenerate e16 menus to build your epplets menu"
