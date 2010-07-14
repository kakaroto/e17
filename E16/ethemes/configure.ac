m4_define([pkg_version],  [1.0.1])
dnl m4_define([pkg_revision], [000])
m4_define([pkg_version],  m4_ifdef([pkg_revision], [pkg_version.pkg_revision], [pkg_version]))
m4_define([rpm_revision], m4_ifdef([pkg_revision], ["0.%(date '+%y%m%d')"], [1]))

AC_INIT([e16-themes], [pkg_version], [enlightenment-devel@lists.sourceforge.net])
AM_INIT_AUTOMAKE([foreign])

AC_SUBST(E_RPM_REVISION, [rpm_revision])

dnl reasonable guesses for where stuff is installed
if test "x$prefix" = "xNONE"; then
  prefix=$ac_default_prefix
fi
if test "x$exec_prefix" = "xNONE"; then
  exec_prefix=$prefix
fi

AC_PROG_MAKE_SET
AC_PROG_INSTALL

THEMES="BlueSteel BrushedMetal-Tigert Ganymede ShinyMetal"
AC_SUBST(THEMES)

AC_CONFIG_FILES([
  Makefile
  BlueSteel/Makefile
  BrushedMetal-Tigert/Makefile
  Ganymede/Makefile
  ShinyMetal/Makefile
  e16-themes.spec
])
AC_OUTPUT
