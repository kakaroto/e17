##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_define([v_maj], [1])
m4_define([v_min], [7])
m4_define([v_mic], [0])
m4_define([v_rev], m4_esyscmd([(svnversion "${SVN_REPO_PATH:-.}" | grep -v '\(export\|Unversioned directory\)' || echo 0) | awk -F : '{printf("%s\n", $1);}' | tr -d ' :MSP\n']))
m4_if(v_rev, [0], [m4_define([v_rev], m4_esyscmd([git log 2> /dev/null | (grep -m1 git-svn-id || echo 0) | sed -e 's/.*@\([0-9]*\).*/\1/' | tr -d '\n']))])
##--   When released, remove the dnl on the below line
m4_undefine([v_rev])
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

AC_INIT([python-ethumb], [v_ver-beta], [enlightenment-devel@lists.sourceforge.net])
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

pkgconfig_ethumb_requires_libs="ethumb >= 1.7.0"
AC_SUBST(pkgconfig_ethumb_requires_libs)
PKG_CHECK_MODULES([ETHUMB], [$pkgconfig_ethumb_requires_libs])
wanted_cython_files="${srcdir}/ethumb/ethumb.c_ethumb.pyx"

have_ethumb_client="no"
want_ethumb_client="auto"
pkgconfig_ethumb_client_requires_libs="ethumb_client >= 1.7.0"
AC_SUBST(pkgconfig_ethumb_client_requires_libs)
AC_ARG_ENABLE([ethumb-client],
   [AC_HELP_STRING([--disable-ethumb-client],
      [disable ethumb-client support. @<:@default=detect@:>@])],
   [want_ethumb_client=$enableval], [])

if test "x$want_ethumb_client" != "xno"; then
   PKG_CHECK_MODULES([ETHUMB_CLIENT],
      [$pkgconfig_ethumb_client_requires_libs],
      [have_ethumb_client="yes"], [have_ethumb_client="no"]
   )
else
    have_ethumb_client="no"
fi
if test "x$want_ethumb_client" = "xyes" -a "x$have_ethumb_client" = "xno"; then
    AC_MSG_ERROR([ethumb-client support requested, but not found by pkg-config: $pkgconfig_ethumb_client_requires_libs])
fi
if test "x$want_ethumb_client" != "xno" -a "x${have_ethumb_client}" = "xyes"; then
   wanted_cython_files="${wanted_cython_files} ${srcdir}/ethumb/ethumb.client.pyx"
fi
AM_CONDITIONAL(WANT_ETHUMB_CLIENT,
   [test "x$want_ethumb_client" != "xno" -a "x${have_ethumb_client}" = "xyes"])

AM_PATH_PYTHON(2.7)
AM_CHECK_PYTHON_HEADERS(,[AC_MSG_ERROR(could not find Python headers)])

cython_required_version=0.15.1
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

FILTER_VAR_REGEXP([CFLAGS], [-fvisibility=hidden])
FILTER_VAR_REGEXP([LDFLAGS], [-fvisibility=hidden])

# Install everything into $pyexecdir
if test "x$pythondir" != "x$pyexecdir"; then
    AC_SUBST([pythondir], [$pyexecdir])
fi

AM_CHECK_PYTHONPATH

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
python-ethumb.pc
python-ethumb_client.pc
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
echo "  Client...............: ${want_ethumb_client} (found: ${have_ethumb_client})"
echo
echo "Installation...........: make install (as root if needed, with 'su' or 'sudo')"
echo "  prefix...............: $prefix"
echo "  pythondir............: $pythondir"
echo "  pyexecdir............: $pyexecdir"
echo
echo

if test "x$PYTHONPATH_CORRECT" != "xyes"; then
   AC_MSG_WARN([your \$PYTHONPATH does not contains the required directories to support your desired installation. Please check warnings above.])
fi


