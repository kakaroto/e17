##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_define([v_maj], [1])
m4_define([v_min], [1])
m4_define([v_mic], [99])
m4_define([v_rev], m4_esyscmd([(svnversion "${SVN_REPO_PATH:-.}" | grep -v export || echo 0) | awk -F : '{printf("%s\n", $1);}' | tr -d ' :MSP\n']))
m4_if(v_rev, [0], [m4_define([v_rev], m4_esyscmd([git log 2> /dev/null | (grep -m1 git-svn-id || echo 0) | sed -e 's/.*@\([0-9]*\).*/\1/' | tr -d '\n']))])
##--   When released, remove the dnl on the below line
dnl m4_undefine([v_rev])
##--   When doing snapshots - change soname. remove dnl on below line
dnl m4_define([relname], [ver-pre-svn-07])
dnl m4_define([v_rel], [-release relname])
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_ifdef([v_rev], [m4_define([v_ver], [v_maj.v_min.v_mic.v_rev])], [m4_define([v_ver], [v_maj.v_min.v_mic])])
m4_define([lt_cur], m4_eval(v_maj + v_min))
m4_define([lt_rev], v_mic)
m4_define([lt_age], v_min)
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##

AC_INIT([exotic], [v_ver], [enlightenment-devel@lists.sourceforge.net])
AC_PREREQ([2.59])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_HEADERS([config.h])
AH_TOP([
#ifndef EFL_CONFIG_H__
#define EFL_CONFIG_H__
])
AH_BOTTOM([
#endif /* EFL_CONFIG_H__ */
])

AM_INIT_AUTOMAKE([1.6 dist-bzip2])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_GNU_SOURCE
AC_SYS_LARGEFILE

AC_LIBTOOL_WIN32_DLL
define([AC_LIBTOOL_LANG_CXX_CONFIG], [:])dnl
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl
AC_PROG_LIBTOOL

##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_ifdef([v_rev], , [m4_define([v_rev], [0])])
m4_ifdef([v_rel], , [m4_define([v_rel], [])])
AC_DEFINE_UNQUOTED(VMAJ, [v_maj], [Major version])
AC_DEFINE_UNQUOTED(VMIN, [v_min], [Minor version])
AC_DEFINE_UNQUOTED(VMIC, [v_mic], [Micro version])
AC_DEFINE_UNQUOTED(VREV, [v_rev], [Revison])
version_info="lt_cur:lt_rev:lt_age"
release_info="v_rel"
AC_SUBST(version_info)
AC_SUBST(release_info)
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
VMAJ=v_maj
AC_SUBST(VMAJ)

AC_C_CONST
AC_C_BIGENDIAN
AC_C_INLINE
AC_PROG_CC_STDC
AC_PROG_CXX
EFL_ATTRIBUTE_UNUSED

# pkg-config
PKG_PROG_PKG_CONFIG

# Check whether pkg-config supports Requires.private
if $PKG_CONFIG --atleast-pkgconfig-version 0.22; then
   pkgconfig_requires_private="Requires.private"
else
   pkgconfig_requires_private="Requires"
fi
AC_SUBST(pkgconfig_requires_private)

### Needed information

AC_CANONICAL_BUILD
AC_CANONICAL_HOST

requirement_exotic=""

use_coyote="no"
use_ps3="no"
use_mingw32ce="no"
use_mingw="no"
use_darwin="no"

case "$host_os" in
   mingw32ce*) use_mingw32ce="yes"
      ;;
   mingw*) use_mingw="yes"
      ;;
   darwin*) use_darwin="yes"
      ;;
   *)
      ;;
esac

case "$host_vendor" in
   ps3*) use_ps3="yes"
      ;;
esac

AC_ARG_ENABLE([coyote],
  AC_HELP_STRING([--enable-coyote], [use coyote backend]),
  [use_coyote="yes"],
  [use_coyote="no"])

have_fnmatch="no"
have_basename="no"
have_dirname="no"
if test "x${use_coyote}" = "xyes"; then
   PKG_CHECK_MODULES([POINT_CORE], [point-core])
   EXOTIC_CONFIGURE_USE_COYOTE="#define EXOTIC_USE_COYOTE"
   AC_DEFINE(EXOTIC_USE_COYOTE, 1, [Use coyote backend])
elif test "x${use_ps3}" = "xyes"; then
   EXOTIC_CONFIGURE_USE_PS3="#define EXOTIC_USE_PS3"
   AC_DEFINE(EXOTIC_USE_PS3, 1, [Use PS3 backend])
elif test "x${use_mingw32ce}" = "xyes"; then
   EXOTIC_CONFIGURE_USE_MINGW32CE="#define EXOTIC_USE_MINGW32CE"
   AC_DEFINE(EXOTIC_USE_MINGW32CE, 1, [Use Mingw32ce backend])
elif test "x${use_mingw}" = "xyes"; then
   EXOTIC_CONFIGURE_USE_MINGW="#define EXOTIC_USE_MINGW"
   AC_DEFINE(EXOTIC_USE_MINGW, 1, [Use mingw backend])
elif test "x${use_darwin}" = "xyes"; then
   EXOTIC_CONFIGURE_USE_DARWIN="#define EXOTIC_USE_DARWIN"
   AC_DEFINE(EXOTIC_USE_DARWIN, 1, [Use darwin backend])
fi
AM_CONDITIONAL(EXOTIC_USE_COYOTE, [test "x${use_coyote}" = "xyes"])
AC_SUBST([EXOTIC_CONFIGURE_USE_COYOTE])
AM_CONDITIONAL(EXOTIC_USE_PS3, [test "x${use_ps3}" = "xyes"])
AC_SUBST([EXOTIC_CONFIGURE_USE_PS3])
AM_CONDITIONAL(EXOTIC_USE_MINGW32CE, [test "x${use_mingw32ce}" = "xyes"])
AC_SUBST([EXOTIC_CONFIGURE_USE_MINGW32CE])
AM_CONDITIONAL(EXOTIC_USE_MINGW, [test "x${use_mingw}" = "xyes"])
AC_SUBST([EXOTIC_CONFIGURE_USE_MINGW])
AM_CONDITIONAL(EXOTIC_USE_DARWIN, [test "x${use_darwin}" = "xyes"])
AC_SUBST([EXOTIC_CONFIGURE_USE_DARWIN])

AM_CONDITIONAL([EXOTIC_FNMATCH], [test "x${have_fnmatch}" = "xno"])
AC_SUBST([EXOTIC_FNMATCH])
AM_CONDITIONAL([EXOTIC_BASENAME], [test "x${have_basename}" = "xno"])
AC_SUBST([EXOTIC_BASENAME])
AM_CONDITIONAL([EXOTIC_DIRNAME], [test "x${have_dirname}" = "xno"])
AC_SUBST([EXOTIC_DIRNAME])

# doxygen program for documentation building
EFL_CHECK_DOXYGEN([build_doc="yes"], [build_doc="no"])

AC_SUBST(requirement_exotic)

#####################################################################
## Output

AC_OUTPUT([
Makefile
exotic.pc
doc/Makefile
doc/Doxyfile
src/Makefile
src/include/Makefile
src/include/exotic_config.h
src/lib/Makefile
src/lib/coyote/Makefile
src/lib/common/Makefile
exotic.spec
])
dnl src/lib/ps3/Makefile
dnl src/lib/windows/Makefile

echo
echo
echo
echo "------------------------------------------------------------------------"
echo "$PACKAGE $VERSION"
echo "------------------------------------------------------------------------"
echo
echo "Configuration Options Summary:"
echo
echo "  Backend:"
echo "    Coyote........: ${use_coyote}"
echo "    PS3...........: ${use_ps3}"
echo "    MingW32CE.....: ${use_mingw32ce}"
echo "    MingW.........: ${use_mingw}"
echo "    Darwin........: ${use_darwin}"
echo
echo "  Documentation........: ${build_doc}"
echo
