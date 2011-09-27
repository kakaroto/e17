##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_define([v_maj], [0])
m4_define([v_min], [1])
m4_define([v_mic], [1])
m4_define([v_rev], m4_esyscmd([(svnversion "${SVN_REPO_PATH:-.}" | grep -v export || echo 0) | awk -F : '{printf("%s\n", $1);}' | tr -d ' :MSP\n']))
m4_if(v_rev, [0], [m4_define([v_rev], m4_esyscmd([git log 2> /dev/null | (grep -m1 git-svn-id || echo 0) | sed -e 's/.*@\([0-9]*\).*/\1/' | tr -d '\n']))])
##--   When released, remove the dnl on the below line
# m4_undefine([v_rev])
##--   When doing snapshots - change soname. remove dnl on below line
m4_define([relname], [ver-pre-svn-08])
m4_define([v_rel], [-release relname])
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_ifdef([v_rev], [m4_define([v_ver], [v_maj.v_min.v_mic.v_rev])], [m4_define([v_ver], [v_maj.v_min.v_mic])])
m4_define([lt_rev], m4_eval(v_maj + v_min))
m4_define([lt_cur], v_mic)
m4_define([lt_age], v_min)
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##

AC_INIT([ephoto], [v_ver], [enlightenment-devel@lists.sourceforge.net])
AC_PREREQ([2.60])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_GNU_SOURCE
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_ISC_POSIX

AM_INIT_AUTOMAKE(1.6 dist-bzip2)
AM_CONFIG_HEADER(src/bin/config.h)
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
m4_ifdef([v_rev], , [m4_define([v_rev], [0])])
AC_DEFINE_UNQUOTED(VREV, [v_rev], [Revison])
AC_SUBST(VMAJ)
AC_SUBST(version_info)

AC_C_BIGENDIAN
AC_PROG_CC_C99
AM_PROG_CC_C_O
AC_C_CONST
AC_FUNC_ALLOCA
AC_C___ATTRIBUTE__

m4_ifdef([v_mic],
   [
    EFL_COMPILER_FLAG([-Wall -Wextra])
    EFL_COMPILER_FLAG([-W])
   ])

EFL_COMPILER_FLAG([-Wshadow])
EFL_COMPILER_FLAG([-fvisibility=hidden])
EFL_COMPILER_FLAG([-ffunction-sections -fdata-sections])
EFL_LINKER_FLAG([-fvisibility=hidden])
EFL_LINKER_FLAG([-Wl,--as-needed])
EFL_LINKER_FLAG([-Wl,--gc-sections])

ALL_LINGUAS=`cat po/LINGUAS | grep -v '^[ ]*#'`
AC_SUBST(ALL_LINGUAS)

m4_ifdef([AM_GNU_GETTEXT_VERSION], [
AM_GNU_GETTEXT_VERSION([0.17])
])

m4_ifdef([AM_GNU_GETTEXT], [
AM_GNU_GETTEXT([external])
po_makefile_in=po/Makefile.in
AM_CONDITIONAL([HAVE_PO], [true])
],[
AM_CONDITIONAL([HAVE_PO], [false])
])
AC_SUBST(LTLIBINTL)

PKG_CHECK_MODULES([ELEMENTARY], [elementary ecore-file ethumb_client])
PKG_CHECK_MODULES([EFREET_MIME], [efreet-mime])
PKG_CHECK_MODULES([EIO], [eio])
PKG_CHECK_MODULES([EET], [eet])

AC_ARG_ENABLE([libexif],
   [AC_HELP_STRING([--enable-libexif], [enable libexif support @<:@default=yes@:>@])],
   [
    if test "x${enableval}" = "xyes" ; then
       _ephoto_enable_libexif="yes"
    else
       _ephoto_enable_libexif="no"
    fi
   ],
   [_ephoto_enable_libexif="yes"]
)

AC_MSG_CHECKING([whether ethumb is built])
AC_MSG_RESULT([${_ephoto_enable_libexif}])

have_libexif="No"

if test "x${_ephoto_enable_libexif}" = "xyes" ; then
    PKG_CHECK_MODULES(EXIF, libexif, [
        have_libexif="Yes"
        AC_DEFINE(HAVE_LIBEXIF, 1, [have libexif support])
    ],[
       have_libexif="No"
    ])
fi

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
ephoto.spec
data/Makefile
data/desktop/Makefile
data/images/Makefile
data/themes/Makefile
data/themes/default/Makefile
data/themes/default/images/Makefile
src/Makefile
src/bin/Makefile
$po_makefile_in
])



cat << EOF
------------------------------------------------------------------------
$PACKAGE $VERSION
------------------------------------------------------------------------

    ________   ________  ___  ___  ________  ________  ________
   /  _____/  /  __   / /  / /  / /  __   / /__  ___/ /  __   /
  /  /____   /  /_/  / /  /_/  / /  / /  /    / /    /  / /  /
 /  _____/  /  _____/ /  __   / /  / /  /    / /    /  / /  /
/  /_____  /  /      /  / /  / /  /_/  /    / /    /  /_/  /
________/ /__/      /__/ /__/ /_______/    /_/    /_______/


ephoto configured with:

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

Now type 'make' ('gmake' on some systems) to compile ephoto, if it
builds successfully then you can 'make install', acquiring required
permissions with 'su' or 'sudo'.

EOF2
