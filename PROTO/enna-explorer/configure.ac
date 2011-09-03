# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT([enna-explorer],[0.0.1],[http://enna.geexbox.org])
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

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.18.1])
CFLAGS="$CFLAGS -DLOCALEDIR=\"\\\"$datadir/locale\\\"\""

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

AC_CONFIG_FILES([ po/Makefile.in
enna-explorer.spec
Makefile
src/Makefile
src/bin/Makefile
data/Makefile
data/other/Makefile
data/theme/Makefile
data/theme/phone/Makefile
data/theme/tablet/Makefile
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
echo "NLS................................ : $USE_NLS"
echo
echo "Suppported Themes:"
echo
echo "  phone............................ : $want_theme_phone"
echo
echo "Now type 'make' ('gmake' on some systems) to compile $PACKAGE."
echo

