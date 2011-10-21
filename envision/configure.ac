AC_INIT(envision, 0.1.0.0, masaki@profusion.mobi)
AC_PREREQ([2.60])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_GNU_SOURCE
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_ISC_POSIX

AM_INIT_AUTOMAKE(1.6 dist-bzip2)
AM_CONFIG_HEADER(config.h)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

define([AC_LIBTOOL_LANG_CXX_CONFIG], [:])
define([AC_LIBTOOL_LANG_GCJ_CONFIG], [:])
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])
AC_PROG_LIBTOOL

VMAJ=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $1);}'`
VMIN=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $2);}'`
VMIC=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $3);}'`
SNAP=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $4);}'`
version_info=`expr $VMAJ + $VMIN`":$VMIC:$VMIN"
AC_SUBST(VMAJ)
AC_SUBST(version_info)

AC_C_BIGENDIAN
AC_PROG_CC_C99
AM_PROG_CC_C_O
AC_C_CONST
AC_FUNC_ALLOCA
AC_C___ATTRIBUTE__

ALL_LINGUAS=`cat po/LINGUAS | grep -v '^[ ]*#'`
AC_SUBST(ALL_LINGUAS)

m4_ifdef([AM_GNU_GETTEXT_VERSION], [
AM_GNU_GETTEXT_VERSION([0.12.1])
])

m4_ifdef([AM_GNU_GETTEXT], [
AM_GNU_GETTEXT([external])
po_makefile_in=po/Makefile.in
have_po="yes"
],[
have_po="no"
])
AC_SUBST(LTLIBINTL)

if test "x${POSUB}" = "x" ; then
   have_po="no"
fi

AM_CONDITIONAL([HAVE_PO], [test "x${have_po}" = "xyes"])

PKG_CHECK_MODULES([ELEMENTARY], [elementary epdf eina ecore evas edje])

AC_ARG_WITH([edje-cc],
   [AC_HELP_STRING([--with-edje-cc=PATH],
                   [specify a specific path to edje_cc])],
   [edje_cc=$withval;
    AC_MSG_NOTICE([edje_cc explicitly set to $edje_cc])
   ],[edje_cc=$(pkg-config --variable=prefix edje)/bin/edje_cc])
AC_SUBST(edje_cc)

want_quicklaunch="auto"
AC_ARG_ENABLE([quicklaunch],
   [AC_HELP_STRING([--disable-quicklaunch],
                   [disable build of quicklaunch (default=auto)])],
   [if test "x${enableval}" = "xno"; then
       want_quicklaunch="no"
    elif test "x${enableval}" = "xyes"; then
       want_quicklaunch="yes"
    else
       want_quicklaunch="auto"
    fi
   ],
   [want_quicklaunch="auto"])

if test "x${want_quicklaunch}" = "xauto"; then
   AC_MSG_CHECKING([checking for elementary_quicklaunch binary...])
   if test -x $(pkg-config --variable=prefix elementary)/bin/elementary_quicklaunch; then
      AC_MSG_RESULT([found, enable quicklaunch.])
      want_quicklaunch="yes"
   else
      AC_MSG_RESULT([not found, disable quicklaunch.])
      want_quicklaunch="no"
   fi
fi

if test "x${want_quicklaunch}" = "xyes"; then
   AC_ARG_WITH([quicklauncher-libdir],
      [AC_HELP_STRING([--with-quicklauncher-libdir=PATH],
                      [specify a specific path to install quicklauncher binaries])],
      [quicklauncher_libdir=$withval;
      AC_MSG_NOTICE([quicklauncher_libdir explicitly set to $quicklauncher_libdir])
      ],
      [quicklauncher_libdir=$(pkg-config --variable=libdir elementary)])
   AC_SUBST(quicklauncher_libdir)
fi

AM_CONDITIONAL(BUILD_QUICKLAUNCH, test "x${want_quicklaunch}" = "xyes")

AC_OUTPUT([
Makefile
src/Makefile
src/bin/Makefile
data/Makefile
data/desktop/Makefile
data/themes/Makefile
$po_makefile_in
])


cat << EOF

envision configured with:

Flags:
   CFLAGS.....(C): $CFLAGS
   CXXFLAGS.(C++): $CXXFLAGS
   CPPFLAGS.(CPP): $CPPFLAGS
   LDFLAGS...(LD): $LDFLAGS

Installation:
   PREFIX..............: $prefix

Quick Launcher: ${want_quicklaunch}
EOF

if test "x${want_quicklaunch}" = "xyes"; then
cat << EOF_QL
   quicklauncher_libdir: $quicklauncher_libdir

EOF_QL
fi

cat << EOF2

Now type 'make' ('gmake' on some systems) to compile envision, if it
builds successfully then you can 'make install', acquiring required
permissions with 'su' or 'sudo'.

EOF2
