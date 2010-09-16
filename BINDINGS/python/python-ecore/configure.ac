##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_define([v_maj], [0])
m4_define([v_min], [7])
m4_define([v_mic], [2])
m4_define([v_rev], m4_esyscmd([(svnversion "${SVN_REPO_PATH:-.}" | grep -v export || echo 0) | awk -F : '{printf("%s\n", $1);}' | tr -d ' :MSP\n']))
m4_if(v_rev, [0], [m4_define([v_rev], m4_esyscmd([git log 2> /dev/null | (grep -m1 git-svn-id || echo 0) | sed -e 's/.*@\([0-9]*\).*/\1/' | tr -d '\n']))])
##--   When released, remove the dnl on the below line
dnl m4_undefine([v_rev])
##--   When doing snapshots - change soname. remove dnl on below line
m4_define([relname], [ver-pre-svn-07])
m4_define([v_rel], [-release relname])
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_ifdef([v_rev], [m4_define([v_ver], [v_maj.v_min.v_mic.v_rev])],
[m4_define([v_ver], [v_maj.v_min.v_mic])])
m4_define([lt_rev], m4_eval(v_maj + v_min))
m4_define([lt_cur], v_mic)
m4_define([lt_age], v_min)
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##

AC_INIT([python-ecore], [v_ver], [enlightenment-devel@lists.sourceforge.net])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_ISC_POSIX

AM_INIT_AUTOMAKE([1.6 dist-bzip2])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

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
version_info="lt_rev:lt_cur:lt_age"
release_info="v_rel"
AC_SUBST(version_info)
AC_SUBST(release_info)
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
VMAJ=v_maj
AC_SUBST(VMAJ)

AC_PROG_CC

PKG_PROG_PKG_CONFIG
if $PKG_CONFIG --atleast-pkgconfig-version 0.22; then
   pkgconfig_requires_private="Requires.private"
else
   pkgconfig_requires_private="Requires"
fi
AC_SUBST(pkgconfig_requires_private)

pkgconfig_ecore_requires_libs="ecore >= 1.0.0"
AC_SUBST(pkgconfig_ecore_requires_libs)
PKG_CHECK_MODULES([ECORE], [$pkgconfig_ecore_requires_libs])
wanted_cython_files="${srcdir}/ecore/ecore.c_ecore.pyx"

have_ecore_x="no"
want_ecore_x="auto"
pkgconfig_ecore_x_requires_libs="ecore-x >= 1.0.0"
AC_SUBST(pkgconfig_ecore_x_requires_libs)

AC_ARG_ENABLE([ecore-x],
   [AC_HELP_STRING([--disable-ecore-x],
      [disable ecore-x support. @<:@default=detect@:>@])],
   [want_ecore_x=$enableval], [])

if test "x$want_ecore_x" != "xno"; then
   PKG_CHECK_MODULES([ECORE_X],
      [$pkgconfig_ecore_x_requires_libs],
      [have_ecore_x="yes"], [have_ecore_x="no"]
   )
else
    have_ecore_x="no"
fi
if test "x$want_ecore_x" = "xyes" -a "x$have_ecore_x" = "xno"; then
    AC_MSG_ERROR([ecore-x support requested, but not found by pkg-config: $pkgconfig_ecore_x_requires_libs])
fi
if test "x$want_ecore_x" != "xno" -a "x${have_ecore_x}" = "xyes"; then
   wanted_cython_files="${wanted_cython_files} ${srcdir}/ecore/x/ecore.x.c_ecore_x.pyx ${srcdir}/ecore/x/ecore.x.screensaver.pyx"
fi
AM_CONDITIONAL(WANT_ECORE_X,
   [test "x$want_ecore_x" != "xno" -a "x${have_ecore_x}" = "xyes"])

have_ecore_imf="no"
want_ecore_imf="auto"
pkgconfig_ecore_imf_requires_libs="ecore-imf >= 1.0.0"
AC_SUBST(pkgconfig_ecore_imf_requires_libs)
AC_ARG_ENABLE([ecore-imf],
   [AC_HELP_STRING([--disable-ecore-imf],
      [disable ecore-imf support. @<:@default=detect@:>@])],
   [want_ecore_imf=$enableval], [])

if test "x$want_ecore_imf" != "xno"; then
   PKG_CHECK_MODULES([ECORE_IMF],
      [$pkgconfig_ecore_imf_requires_libs],
      [have_ecore_imf="yes"], [have_ecore_imf="no"]
   )
else
    have_ecore_imf="no"
fi
if test "x$want_ecore_imf" = "xyes" -a "x$have_ecore_imf" = "xno"; then
    AC_MSG_ERROR([ecore-imf support requested, but not found by pkg-config: $pkgconfig_ecore_imf_requires_libs])
fi
if test "x$want_ecore_imf" != "xno" -a "x${have_ecore_imf}" = "xyes"; then
   wanted_cython_files="${wanted_cython_files} ${srcdir}/ecore/imf/ecore.imf.c_ecore_imf.pyx"
fi
AM_CONDITIONAL(WANT_ECORE_IMF,
   [test "x$want_ecore_imf" != "xno" -a "x${have_ecore_imf}" = "xyes"])

have_ecore_win32="no"
want_ecore_win32="auto"
pkgconfig_ecore_win32_requires_libs="ecore-win32 >= 1.0.0"
AC_SUBST(pkgconfig_ecore_win32_requires_libs)
AC_ARG_ENABLE([ecore-win32],
   [AC_HELP_STRING([--disable-ecore-win32],
      [disable ecore-win32 support. @<:@default=detect@:>@])],
   [want_ecore_win32=$enableval], [])

if test "x$want_ecore_win32" != "xno"; then
   PKG_CHECK_MODULES([ECORE_WIN32],
      [$pkgconfig_ecore_win32_requires_libs],
      [have_ecore_win32="yes"], [have_ecore_win32="no"]
   )
else
    have_ecore_win32="no"
fi
if test "x$want_ecore_win32" = "xyes" -a "x$have_ecore_win32" = "xno"; then
    AC_MSG_ERROR([ecore-win32 support requested, but not found by pkg-config: $pkgconfig_ecore_win32_requires_libs])
fi
if test "x$want_ecore_win32" != "xno" -a "x${have_ecore_win32}" = "xyes"; then
   wanted_cython_files="${wanted_cython_files} ${srcdir}/ecore/win32/ecore.win32.c_ecore_win32.pyx"
fi
AM_CONDITIONAL(WANT_ECORE_WIN32,
   [test "x$want_ecore_win32" != "xno" -a "x${have_ecore_win32}" = "xyes"])

python_evas_version="0.7.2"
have_ecore_evas="no"
want_ecore_evas="auto"
pkgconfig_ecore_evas_requires_libs="ecore-evas >= 1.0.0 python-evas >= ${python_evas_version}"
AC_SUBST(pkgconfig_ecore_evas_requires_libs)
AC_ARG_ENABLE([ecore-evas],
   [AC_HELP_STRING([--disable-ecore-evas],
      [disable ecore-evas support. @<:@default=detect@:>@])],
   [want_ecore_evas=$enableval], [])

if test "x$want_ecore_evas" != "xno"; then
   PKG_CHECK_MODULES([ECORE_EVAS],
      [$pkgconfig_ecore_evas_requires_libs],
      [have_ecore_evas="yes"], [have_ecore_evas="no"]
   )
else
    have_ecore_evas="no"
fi
if test "x$want_ecore_evas" = "xyes" -a "x$have_ecore_evas" = "xno"; then
    AC_MSG_ERROR([ecore-evas support requested, but not found by pkg-config: $pkgconfig_ecore_evas_requires_libs])
fi
if test "x$want_ecore_evas" != "xno" -a "x${have_ecore_evas}" = "xyes"; then
   wanted_cython_files="${wanted_cython_files} ${srcdir}/ecore/evas/ecore.evas.c_ecore_evas.pyx"
fi
AM_CONDITIONAL(WANT_ECORE_EVAS,
   [test "x$want_ecore_evas" != "xno" -a "x${have_ecore_evas}" = "xyes"])
CYTHON_ECORE_EVAS_INCLUDEDIR=`pkg-config --variable=includedir "python-evas >= ${python_evas_version}"`
AC_SUBST(CYTHON_ECORE_EVAS_INCLUDEDIR)

AM_PATH_PYTHON(2.5)
AM_CHECK_PYTHON_HEADERS(,[AC_MSG_ERROR(could not find Python headers)])

cython_required_version=0.13
AM_CHECK_CYTHON([$cython_required_version], [:], [:])

have_cython_precompiled=no
AM_CHECK_CYTHON_PRECOMPILED([$wanted_cython_files],
   [have_cython_precompiled=yes], [have_cython_precompiled=no])
if test "x$CYTHON_FOUND" != "xyes"; then
   if test v_rev -ne 0; then
      if test "x$have_cython_precompiled" = "xyes"; then
         AC_MSG_WARN([Cannot find Cython>=${cython_required_version}, however found a pre-generated file. YOU MAY FIND PROBLEMS, USE A RELEASE TARBALL OR INSTALL CYTHON>=${cython_required_version}])
      else
         AC_MSG_ERROR([Cannot find Cython>=${cython_required_version}, and not a release build])
      fi
   else
      if test "x$have_cython_precompiled" = "xyes"; then
         AC_MSG_NOTICE([Using pre-generated files from release tarball. Install Cython>=${cython_required_version} if you want to re-generate them.])
      else
         AC_MSG_ERROR([PACKAGING ERROR RELEASE BUILD WITHOUT PRE-GENERATED FILES])
      fi
   fi
fi
AM_CONDITIONAL(HAVE_CYTHON, [test "x$CYTHON_FOUND" = "xyes"])

AC_PATH_PROG([EPYDOC], [epydoc], [no])
AC_SUBST(EPYDOC)
AM_CONDITIONAL(HAVE_EPYDOC, [test "x$EPYDOC" != "xno"])

AC_ARG_ENABLE([examples],
   [AC_HELP_STRING([--disable-examples], [Do not install examples])],
   [if test "x${enableval}" != "xno"; then
       want_examples=yes
    else
       want_examples=no
    fi
   ], [want_examples=yes])
AM_CONDITIONAL([WANT_EXAMPLES], [test "x${want_examples}" != "xno"])

AC_OUTPUT([
Makefile
python-ecore.pc
python-ecore-x.pc
python-ecore-imf.pc
python-ecore-win32.pc
python-ecore-evas.pc
])

if test "x$CYTHON_FOUND" = "xyes"; then
   cython_info="$CYTHON_VERSION (CYTHONFLAGS=$CYTHONFLAGS)"
else
   cython_info="no"
fi
AC_SUBST(CYTHONFLAGS)

echo
echo
echo
echo "------------------------------------------------------------------------"
echo "$PACKAGE $VERSION"
echo "------------------------------------------------------------------------"
echo
echo "Configuration Options Summary:"
echo
echo "  Python...............: $PYTHON ($PYTHON_VERSION)"
echo "  Cython...............: $cython_info"
echo
echo "Compilation............: make (or gmake)"
echo "  CPPFLAGS.............: $CPPFLAGS"
echo "  CFLAGS...............: $CFLAGS"
echo "  LDFLAGS..............: $LDFLAGS"
echo "  PYTHONPATH...........: $PYTHONPATH"
echo
echo "Modules:"
echo "  X....................: ${want_ecore_x} (found: ${have_ecore_x})"
echo "  IMF..................: ${want_ecore_imf} (found: ${have_ecore_imf})"
echo "  Win32................: ${want_ecore_win32} (found: ${have_ecore_win32})"
echo "  Evas.................: ${want_ecore_evas} (found: ${have_ecore_evas})"
echo
echo "Installation...........: make install (as root if needed, with 'su' or 'sudo')"
echo "  prefix...............: $prefix"
echo
echo
echo "To use the local version (as opposed to the installed version) use:"
echo
echo "  export PYTHONPATH=\"$PWD:$PWD/.libs:\$PYTHONPATH\""
echo "  python -c 'import ecore; print ecore' # test load"
echo


