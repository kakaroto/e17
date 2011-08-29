# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT([enna],[0.0.1],[http://enna.geexbox.org])
AC_PREREQ([2.61])

AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
AC_USE_SYSTEM_EXTENSIONS

AM_INIT_AUTOMAKE([1.11 dist-bzip2])
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])],[AC_SUBST([AM_DEFAULT_VERBOSITY],[1])])

LT_PREREQ([2.2])
LT_INIT([win32-dll])

AC_CANONICAL_HOST


### Checks for programs

AC_PROG_CC


### Checks for libraries

enna_requirements="
   eina >= 1.0.0
   eet >= 1.4.0
   evas >= 1.0.0
   ecore >= 1.0.0
   ecore-evas >= 1.0.0
   ecore-file >= 1.0.0
   ecore-ipc >= 1.0.0
   ecore-con >= 1.0.0
   efreet >= 1.0.0
   edje >= 1.0.0
   elementary >= 0.7.0
   eio"

PKG_CHECK_MODULES([ENNA], [${enna_requirements}])


### Checks for header files


### Checks for types


### Checks for structures


### Checks for compiler characteristics

AC_C_CONST
AC_PROG_CC_C_O
AC_C___ATTRIBUTE__

AC_SYS_LARGEFILE

EFL_COMPILER_FLAG([-Wall])
AC_DEFINE(_LARGEFILE_SOURCE, 1, [large file source support])
AC_DEFINE(_REENTRANT, 1, [reentrant support])


### Checks for linker characteristics


### Checks for library functions


##################
# Static Modules #
##################

AC_ARG_ENABLE(static_modules,
  [AC_HELP_STRING([--disable-static-modules], [disable static modules.
  [[default=enabled]]])],
  [use_static_modules=$enableval],
  [use_static_modules="yes"]
)
AM_CONDITIONAL([USE_STATIC_MODULES], test "x${use_static_modules}" = "xyes")
if test "x$use_static_modules" = "xyes"; then
  AC_DEFINE(USE_STATIC_MODULES, 1, [Static Modules])
fi

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
# Explorer
want_activity_explorer="yes"
AC_MSG_CHECKING(whether explorer activity module is to be built)
AC_ARG_ENABLE(activity-explorer,
  AS_HELP_STRING([--enable-activity-explorer],[Enable Explorer Activity Module]),
  [ want_activity_explorer=$enableval ]
)
AC_MSG_RESULT($want_activity_explorer)
AM_CONDITIONAL(BUILD_ACTIVITY_EXPLORER, test "x$want_activity_explorer" = "xyes")
if test "x$want_activity_explorer" = "xyes"; then
  AC_DEFINE(BUILD_ACTIVITY_EXPLORER, 1, [Explorer Activity])
fi

# Music
want_activity_music="no"
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

###################
# Browser Modules #
###################
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

# UPnP/DLNA
want_browser_upnp="auto"
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
    [gssdp-1.0 >= 0.7.0 gupnp-1.0 >= 0.13.1 gupnp-av-1.0 >= 0.5.2],
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

##################
# Volume Modules #
##################

# mtab
want_volume_mtab="yes"
ENNA_CHECK_MODULE([volume-mtab], [${want_volume_mtab}], [mtab Volume])

# Udev
want_volume_udev="auto"
ENNA_CHECK_MODULE([volume-udev], [${want_volume_udev}], [Udev Volume])

##########
# THEMES #
##########

# Phone theme
want_theme_phone="yes"
AC_MSG_CHECKING(whether Phone Theme is to be built)
AC_ARG_ENABLE(theme_phone,
  AS_HELP_STRING([--enable-theme-phone],[Enable Phone Theme]),
  [ want_theme_phone=$enableval ]
)
AC_MSG_RESULT($want_theme_phone)
AM_CONDITIONAL(BUILD_THEME_PHONE, test "x$want_theme_phone" = "xyes")
if test "x$want_theme_phone" = "xyes"; then
  AC_DEFINE(BUILD_THEME_PHONE, 1, [Phone Theme])
fi

AC_CONFIG_FILES([
Makefile
src/Makefile
src/bin/Makefile
src/modules/Makefile
src/modules/browser/Makefile
src/modules/browser/localfiles/Makefile
src/modules/browser/upnp/Makefile
src/modules/volume/Makefile
src/modules/volume/mtab/Makefile
src/modules/volume/udev/Makefile
data/Makefile
data/other/Makefile
data/theme/Makefile
data/theme/phone/Makefile
])

AC_OUTPUT

echo
echo "$PACKAGE $VERSION"
echo
eval echo "Installation Path.................. : $exec_prefix"
eval echo "\  Binaries......................... : $exec_prefix/bin"

if test "x$use_static_modules" = "xno"; then
eval echo "\  Modules.......................... : $exec_prefix/lib/enna/modules"
fi

eval echo "\  Theme............................ : $exec_prefix/share/enna/theme"
echo
echo "Build theme........................ : $build_theme"
echo "Static Modules..................... : $use_static_modules"
echo "NLS................................ : $USE_NLS"
echo
echo "Supported Activity Modules:"
echo
echo "  Explorer......................... : $want_activity_explorer"
echo "  Music............................ : $want_activity_music"
echo
echo "Supported Browser Modules:"
echo
echo "  Local files...................... : $want_browser_localfiles"
echo "  UPnP/DLNA........................ : $have_browser_upnp"
echo
echo "Supported Volume Modules:"
echo
echo "  mtab............................. : $have_enna_module_volume_mtab"
echo "  Udev............................. : $have_enna_module_volume_udev"
echo
echo "Suppported Themes:"
echo
echo "  phone............................ : $want_theme_phone"
echo
echo "Now type 'make' ('gmake' on some systems) to compile $PACKAGE."
echo

