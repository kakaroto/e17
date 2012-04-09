##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_define([v_maj], [0])
m4_define([v_min], [0])
m4_define([v_mic], [1])
m4_define([v_rev], m4_esyscmd([(svnversion "${SVN_REPO_PATH:-.}" | grep -v '\(export\|Unversioned directory\)' || echo 0) | awk -F : '{printf("%s\n", $1);}' | tr -d ' :MSP\n']))
m4_if(v_rev, [0], [m4_define([v_rev], m4_esyscmd([git log 2> /dev/null | (grep -m1 git-svn-id || echo 0) | sed -e 's/.*@\([0-9]*\).*/\1/' | tr -d '\n']))])
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_define([v_ver], [v_maj.v_min.v_mic])
m4_define([lt_rev], m4_eval(v_maj + v_min))
m4_define([lt_cur], v_mic)
m4_define([lt_age], v_min)
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##

AC_PREREQ(2.57)
AC_INIT([efx],[v_ver],[michael.blumenkrantz@gmail.com],[efx])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADER([config.h])
AC_GNU_SOURCE
AC_ISC_POSIX
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_ifdef([v_rev], , [m4_define([v_rev], [0])])
m4_ifdef([v_rel], , [m4_define([v_rel], [])])
AC_DEFINE_UNQUOTED(VMAJ, [v_maj], [Major version])
AC_DEFINE_UNQUOTED(VMIN, [v_min], [Minor version])
AC_DEFINE_UNQUOTED(VMIC, [v_mic], [Micro version])
AC_DEFINE(VREV, ["v_rev"], [Revison])
version_info="lt_rev:lt_cur:lt_age"
release_info="v_rel"
AC_SUBST(version_info)
AC_SUBST(release_info)
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
VMAJ=v_maj
AC_SUBST(VMAJ)

AM_INIT_AUTOMAKE([foreign])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_PROG_LIBTOOL
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_MAKE_SET
AC_HEADER_STDC

AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T

AC_FUNC_ALLOCA
AC_C___ATTRIBUTE__
PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([EFX_DEPS], [eina >= 1.1.999 eet >= 1.5.0 evas >= 1.1.999 ecore >= 1.1.999])

want_tests="auto"
AC_ARG_ENABLE([tests],
   [AC_HELP_STRING([--disable-tests], [disable test binaries. @<:@default=detect@:>@])],
   [want_tests=$enableval], [])

if test "x$want_tests" != "xno"; then
   PKG_CHECK_MODULES([ECORE_EVAS], [ecore-evas >= 1.1.999])
   PKG_CHECK_MODULES([ELM], [elementary],,)
fi

AM_CONDITIONAL([BUILD_TESTS], [test "x$want_tests" != "xno"])
AM_CONDITIONAL([BUILD_TEST_ELM], [test -n "${ELM_LIBS}"])

m4_ifdef([v_mic],
   [
    EFL_COMPILER_FLAG([-Wshadow])
    EFL_COMPILER_FLAG([-Wall])
    EFL_COMPILER_FLAG([-Wextra])
   ])

#EFL_CHECK_DOXYGEN([build_doc="yes"], [build_doc="no"])
# write output
AC_CONFIG_FILES([
  Makefile
  data/Makefile
  src/Makefile
  src/lib/Makefile
  src/bin/Makefile
])
AC_OUTPUT
