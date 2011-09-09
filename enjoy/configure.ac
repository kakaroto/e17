##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_define([v_maj], [0])
m4_define([v_min], [1])
m4_define([v_mic], [0])
m4_define([v_rev], m4_esyscmd([(svnversion "${SVN_REPO_PATH:-.}" | grep -v export || echo 0) | awk -F : '{printf("%s\n", $1);}' | tr -d ' :MSP\n']))
m4_if(v_rev, [0], [m4_define([v_rev], m4_esyscmd([git log 2> /dev/null | (grep -m1 git-svn-id || echo 0) | sed -e 's/.*@\([0-9]*\).*/\1/' | tr -d '\n']))])
##--   When released, remove the dnl on the below line
m4_undefine([v_rev])
##--   When doing snapshots - change soname. remove dnl on below line
dnl m4_define([relname], [ver-pre-svn-07])
dnl m4_define([v_rel], [-release relname])
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_ifdef([v_rev], [m4_define([v_ver], [v_maj.v_min.v_mic.v_rev])], [m4_define([v_ver], [v_maj.v_min.v_mic])])
m4_define([lt_rev], m4_eval(v_maj + v_min))
m4_define([lt_cur], v_mic)
m4_define([lt_age], v_min)
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##

AC_INIT([enjoy], [v_ver.alpha], [enlightenment-devel@lists.sourceforge.net])
AC_PREREQ([2.60])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_GNU_SOURCE
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_ISC_POSIX

AM_INIT_AUTOMAKE(1.6 dist-bzip2)
AM_CONFIG_HEADER(config.h)
_XTERM_COLORS
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
AC_DEFINE_UNQUOTED(VMAJ, [v_maj], [Major version])
AC_DEFINE_UNQUOTED(VMIN, [v_min], [Minor version])
AC_DEFINE_UNQUOTED(VMIC, [v_mic], [Micro version])
AC_DEFINE_UNQUOTED(VREV, [v_rev], [Revison])
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

AM_GNU_GETTEXT_VERSION([0.12.1])
AM_GNU_GETTEXT([external])

PKG_CHECK_MODULES([ECORE], [ecore])
PKG_CHECK_MODULES([EDBUS], [edbus])
PKG_CHECK_MODULES([EFL], [ecore-file eina evas ecore-evas edje])
PKG_CHECK_MODULES([ELEMENTARY], [elementary])
PKG_CHECK_MODULES([EMOTION], [emotion])
PKG_CHECK_MODULES([LMS], [lightmediascanner])
PKG_CHECK_MODULES([SQLITE3], [sqlite3])

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

AC_ENJOY_OPTIONAL_MODULE([FSO], false)
AC_ENJOY_OPTIONAL_MODULE([MPRIS], true)

AC_OUTPUT([
Makefile
enjoy.spec
src/Makefile
src/bin/Makefile
src/plugins/Makefile
src/plugins/fso/Makefile
src/plugins/mpris/Makefile
data/Makefile
data/desktop/Makefile
data/themes/Makefile
po/Makefile.in
])


cat << EOF

enjoy configured with:

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

txt_strip() {
        echo "[$]@" | sed -e 's/^[[ \t]]*\([[^ \t]]*\)[[ \t]]*$/\1/g'
}
MODS=""
for mod in $OPTIONAL_MODULES; do
        MODS="$MODS ${COLOR_HGREEN}+$mod${COLOR_END}"
done
MODS=$(txt_strip $MODS)

UNUSED_MODS=""
for mod in $UNUSED_OPTIONAL_MODULES; do
        UNUSED_MODS="$UNUSED_MODS ${COLOR_HRED}-$mod${COLOR_END}"
done
UNUSED_MODS=$(txt_strip $UNUSED_MODS)

echo
echo -e "Modules: $MODS $UNUSED_MODS"

cat << EOF2

Now type 'make' ('gmake' on some systems) to compile enjoy, if it
builds successfully then you can 'make install', acquiring required
permissions with 'su' or 'sudo'.

EOF2
