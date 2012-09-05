##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_define([v_maj], [1])
m4_define([v_min], [7])
m4_define([v_mic], [99])
m4_define([v_rev], m4_esyscmd([(svnversion "${SVN_REPO_PATH:-.}" | grep -v '\(export\|Unversioned directory\)' || echo 0) | awk -F : '{printf("%s\n", $1);}' | tr -d ' :MSP\n']))
m4_if(v_rev, [0], [m4_define([v_rev], m4_esyscmd([git log 2> /dev/null | (grep -m1 git-svn-id || echo 0) | sed -e 's/.*@\([0-9]*\).*/\1/' | tr -d '\n']))])
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_define([v_ver], [v_maj.v_min.v_mic])
m4_define([lt_rev], m4_eval(v_maj + v_min))
m4_define([lt_cur], v_mic)
m4_define([lt_age], v_min)
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##

AC_PREREQ([2.57])
AC_INIT([efx], [v_ver],[michael.blumenkrantz@gmail.com], [efx])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADER([config.h])
AC_GNU_SOURCE

##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_ifdef([v_rev], , [m4_define([v_rev], [0])])
m4_ifdef([v_rel], , [m4_define([v_rel], [])])
AC_DEFINE_UNQUOTED([VMAJ], [v_maj], [Major version])
AC_DEFINE_UNQUOTED([VMIN], [v_min], [Minor version])
AC_DEFINE_UNQUOTED([VMIC], [v_mic], [Micro version])
AC_DEFINE([VREV], ["v_rev"], [Revison])
version_info="lt_rev:lt_cur:lt_age"
release_info="v_rel"
AC_SUBST([version_info])
AC_SUBST([release_info])
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
VMAJ=v_maj
AC_SUBST([VMAJ])


### Needed information

AC_CANONICAL_HOST

want_tests="auto"

# Additional options to configure

AC_ARG_ENABLE([tests],
   [AC_HELP_STRING([--disable-tests], [disable test binaries. @<:@default=detect@:>@])],
   [
    if test "x${enableval}" = "xyes" ; then
       want_tests="yes"
    else
       want_tests="no"
    fi
   ],
   [want_tests="auto"])


### Checks for programs

# Autotools
AM_INIT_AUTOMAKE([foreign])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_LIBTOOL_WIN32_DLL
define([AC_LIBTOOL_LANG_CXX_CONFIG], [:])dnl
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl
AC_PROG_LIBTOOL

# Compiler
AC_PROG_CC

# pkg-config
PKG_PROG_PKG_CONFIG

# Check whether pkg-config supports Requires.private
if $PKG_CONFIG --atleast-pkgconfig-version 0.22; then
   pkgconfig_requires_private="Requires.private"
else
   pkgconfig_requires_private="Requires"
fi
AC_SUBST([pkgconfig_requires_private])

# doxygen program for documentation building
#EFL_CHECK_DOXYGEN([build_doc="yes"], [build_doc="no"])


### Checks for libraries

requirements_pc_efx="eina >= 1.1.999 eet >= 1.5.0 evas >= 1.1.999 ecore >= 1.1.999"
AC_SUBST([requirements_pc_efx])

EFL_EFX_BUILD=""
case "$host_os" in
   mingw*)
      EFL_EFX_BUILD="-DEFL_EFX_BUILD"
   ;;
esac
AC_SUBST([EFL_EFX_BUILD])

PKG_CHECK_MODULES([EFX], [${requirements_pc_efx}])

if test "x$want_tests" != "xno"; then
   PKG_CHECK_MODULES([EFX_TEST],
      [${requirements_pc_efx} ecore-evas >= 1.1.999],
      [want_tests="yes"],
      [want_tests="no"])
   PKG_CHECK_MODULES([EFX_TEST_ELM],
      [elementary edbus efreet ${requirements_pc_efx}],
      [want_test_elm="yes"],
      [want_test_elm="no"])
fi

AM_CONDITIONAL([BUILD_TESTS], [test "x${want_tests}" = "xyes"])
AM_CONDITIONAL([BUILD_TEST_ELM], [test "x${want_test_elm}" = "xyes"])


### Checks for header files


### Checks for types
AC_TYPE_SIZE_T


### Checks for structures


### Checks for compiler characteristics
AC_C_INLINE
AC_C___ATTRIBUTE__

m4_ifdef([v_mic],
   [
    EFL_COMPILER_FLAG([-Wshadow])
    EFL_COMPILER_FLAG([-Wall])
    EFL_COMPILER_FLAG([-Wextra])
   ])


### Checks for linker characteristics
lt_enable_auto_import=""
case "${host_os}" in
   mingw*)
      lt_enable_auto_import="-Wl,--enable-auto-import"
   ;;
esac
AC_SUBST([lt_enable_auto_import])


### Checks for library functions

AC_FUNC_ALLOCA

# write output
AC_CONFIG_FILES([
  Makefile
  data/Makefile
  src/Makefile
  src/lib/Makefile
  src/bin/Makefile
  efx.pc
])

AC_OUTPUT

#####################################################################
## Info

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
echo "  Tests................: ${want_tests} (Elementary: ${want_test_elm})"
echo
echo "Compilation............: make (or gmake)"
echo "  CPPFLAGS.............: $CPPFLAGS"
echo "  CFLAGS...............: $CFLAGS"
echo "  LDFLAGS..............: $LDFLAGS"
echo
echo "Installation...........: make install (as root if needed, with 'su' or 'sudo')"
echo "  prefix...............: $prefix"
echo
