# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT(configure.ac)
AC_CANONICAL_TARGET
AC_CANONICAL_BUILD
AC_CANONICAL_HOST

AM_INIT_AUTOMAKE(Photo Manager, 0.1)
AM_CONFIG_HEADER(config.h)

AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_C_CONST
AM_PROG_CC_C_O

AM_PROG_LIBTOOL

AC_SUBST(version_info)
AC_SUBST(requirements)
AC_SUBST(packagesrcdir)
AC_SUBST(packagedatadir)

PKG_PROG_PKG_CONFIG

define([AC_LIBTOOL_LANG_CXX_CONFIG], [:])dnl
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl

requirements="ecore eet ecore-file evas ethumb_client"
AC_SUBST(requirements)

# Find edje_cc
PKG_CHECK_MODULES(EDJE, [edje >= 0.5.0])
AC_ARG_WITH(edje-cc,
  AC_HELP_STRING([--with-edje-cc=PATH], [specify a specific path to edje_cc]),
  [
    v=$withval;
    EDJE_CC=$v
  ],[
    EDJE_CC=$(pkg-config --variable=prefix edje)/bin/edje_cc
  ]
)

# doxygen program for documentation building

EFL_CHECK_DOXYGEN([build_doc="yes"], [build_doc="no"])

PKG_CHECK_MODULES(EPHOTO_MANAGER, [
                             ecore >= 0.1
                             ecore-file >= 0.1
                             eet >= 0.1
                             ethumb_client >= 0.1
                        ])
# required libs / cflags
ephoto_manager_libs=$EPHOTO_MANAGER_LIBS
ephoto_manager_cflags=$EPHOTO_MANAGER_CFLAGS
AC_SUBST(ephoto_manager_libs)
AC_SUBST(ephoto_manager_cflags)

AC_SUBST(EDJE_CC)
AC_MSG_CHECKING([Which edje_cc to use])
AC_MSG_RESULT(${EDJE_CC})

AC_ARG_ENABLE([shave],
              [  --disable-shave                  Use shave output cleaner when building.],
              [ case "${enableval}" in
               yes)    enable_shave=yes;;
               no)    enable_shave=no;;
               *)    AC_MSG_ERROR([bad value ${enableval} for --disable-shave]);;
               esac],
               [enable_shave=yes])

if test x$enable_shave = xyes; then
    SHAVE_INIT([./])
fi

AC_CONFIG_FILES([
    shave
    shave-libtool
])

AC_OUTPUT([
Makefile
src/Makefile
src/photo_manager/Makefile
src/bin_pm_load_sync/Makefile
src/bin_pm_sync/Makefile
src/bin_pm_print/Makefile
ephoto_manager.pc
doc/Makefile
])


echo
echo
echo "--------------------------------------------------------"
echo "$PACKAGE $VERSION"
echo "--------------------------------------------------------"
echo
echo
echo "  Installation Path...................: $prefix"
echo
echo "Now type 'make' ('gmake' on some systems) to compile $PACKAGE,"
echo "and then afterwards as root (or the user who will install this), type"
echo "'make install'. Change users with 'su' or 'sudo' appropriately."
echo
