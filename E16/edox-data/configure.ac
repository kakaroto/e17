m4_define([pkg_version],  [0.16.8.0.1])
dnl m4_define([pkg_revision], [0.00])
m4_define([pkg_ver_rev],  m4_ifdef([pkg_revision], [pkg_version-pkg_revision], [pkg_version]))
m4_define([rpm_revision], m4_ifdef([pkg_revision], [pkg_revision], [1]))

AC_INIT(e16-docs, [pkg_ver_rev], [enlightenment-devel@lists.sourceforge.net])
AM_INIT_AUTOMAKE([foreign])

AC_DEFINE(E_PKG_VERSION, ["pkg_version"], [Version])
AC_SUBST(E_PKG_VERSION,  [pkg_version])
AC_SUBST(E_RPM_REVISION, [rpm_revision])

dnl reasonable guesses for where stuff is installed
if test "x$prefix" = "xNONE"; then
  prefix=$ac_default_prefix
fi
if test "x$exec_prefix" = "xNONE"; then
  exec_prefix=$prefix
fi

AM_MAINTAINER_MODE

AC_PROG_MAKE_SET
AC_PROG_INSTALL

edocdata=`cd $srcdir/E-docs; ls -1 MAIN *.png Vera.ttf VeraBd.ttf`
edocdata=`eval echo $edocdata`
AC_SUBST(edocdata)

AC_OUTPUT([
  Makefile
  E-docs/Makefile
  e16-docs.spec
])
