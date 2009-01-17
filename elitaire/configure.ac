#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT(elitaire, 0.1, [Peter Wehrfritz <peter.wehrfritz@web.de>])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PATH_PROGS(EDJE_CC, edje_cc)
AM_PROG_LIBTOOL

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset sqrt])

dnl Set PACKAGE_LOCALE_DIR in config.h.
if test "x${prefix}" = "xNONE"; then
  AC_DEFINE_UNQUOTED(PACKAGE_LOCALE_DIR, "${ac_default_prefix}/share/locale", [Locale-specific data directory])
else
  AC_DEFINE_UNQUOTED(PACKAGE_LOCALE_DIR, "${prefix}/share/locale", [Locale-specific data directory])
fi

dnl Set PACKAGE_DATA_DIR in config.h.
if test "x${prefix}" = "xNONE"; then
  AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${ac_default_prefix}/share/${PACKAGE}", [PACKAGE_DATA_DIR])
else
  AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${prefix}/share/${PACKAGE}", [PACKAGE_DATA_DIR])
fi

dnl Set PACKAGE_BIN_DIR in config.h.
if test "x${datadir}" = 'x${prefix}/bin'; then
  if test "x${prefix}" = "xNONE"; then
    AC_DEFINE_UNQUOTED(PACKAGE_BIN_DIR, "${ac_default_prefix}/bin", [Installation directory for user executables])
  else
    AC_DEFINE_UNQUOTED(PACKAGE_BIN_DIR, "${prefix}/bin", [Installation directory for user executables])
  fi
else
  AC_DEFINE_UNQUOTED(PACKAGE_BIN_DIR, "${bindir}", [Installation directory for user executables])
fi

dnl Set PACKAGE_LOCALSTATE_DIR in config.h.
if test "x${localstatedir}" = 'x${prefix}/var'; then
  if test "x${prefix}" = "xNONE"; then
    AC_DEFINE_UNQUOTED(PACKAGE_LOCALSTATE_DIR, "${ac_default_prefix}/var", [Installation directory for libraries])
  else
    AC_DEFINE_UNQUOTED(PACKAGE_LOCALSTATE_DIR, "${prefix}/var", [Installation directory for libraries])
  fi
else
  AC_DEFINE_UNQUOTED(PACKAGE_LOCALSTATE_DIR, "${localstatedir}", [Installation directory for libraries])
fi

# NLS stuff
ALL_LINGUAS = "de it";
AC_SUBST(ALL_LINGUAS)

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.15])

PKG_CHECK_MODULES(ELITAIRE, [
  eet
  evas
  ecore
  ecore-config
  ecore-evas
  ecore-file
  edje
  esmart_container
  esmart_draggies
  esmart_resize
  ewl
])

dnl set the group and user for the highscore; default is games

AC_ARG_WITH(scores-group,
[  --with-scores-group=group    Group for the high score tables and binaries],
scores_group="$withval",scores_group="games")
AC_SUBST(scores_group)
AC_ARG_WITH(scores-user,
[  --with-scores-user=user    User for the high score tables],
scores_user="$withval",scores_user="games")
AC_SUBST(scores_user)

AC_CONFIG_FILES([Makefile 
                 po/Makefile.in 
                 data/Makefile
                 doc/Makefile
		 data/other/Makefile
		 data/other/elitaire.desktop
                 data/themes/Makefile
                 data/themes/default_cards/Makefile
                 data/themes/default_cards/images/Makefile
                 data/themes/e17/Makefile
                 data/themes/e17/images/Makefile
                 data/themes/e17/fonts/Makefile
                 src/Makefile])

AC_OUTPUT

if grep -q "$scores_group:" /etc/group ; then
:;
else
echo
echo "Warning: The $scores_group group does not exist. Installation will"
echo "         succeed, but high score logging will not work. To specify a"
echo "         different group use the --with-scores-group option."
echo
fi

