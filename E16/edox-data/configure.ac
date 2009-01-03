m4_define([pkg_version],  [0.16.8.0.2])
m4_define([pkg_revision], [0.00])
m4_define([pkg_version],  m4_ifdef([pkg_revision], [pkg_version-pkg_revision], [pkg_version]))
m4_define([rpm_revision], m4_ifdef([pkg_revision], [pkg_revision], [1]))

AC_INIT(e16-docs, [pkg_version], [enlightenment-devel@lists.sourceforge.net])
AM_INIT_AUTOMAKE([foreign])

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

edocdata=`cat $srcdir/E-docs/FILES`
edocdata=`eval echo $edocdata`
AC_SUBST(edocdata)

AC_OUTPUT([
  Makefile
  E-docs/Makefile
  e16-docs.spec
])
