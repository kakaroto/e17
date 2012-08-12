AC_INIT([econnman], [1], [enlightenment-devel@lists.sourceforge.net])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AM_PATH_PYTHON([2.7])
EFL_WITH_BIN([edje], [edje-cc], [edje_cc])

# not strictly needed during compile, but let's force this check
PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([PY_EFL], [
   python-elementary >= 1.7.0
   python-edbus >= 1.7.0
])

AC_CONFIG_FILES([
Makefile
])

AC_OUTPUT

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
echo "  edje_cc..............: $edje_cc"
echo
echo "Installation...........: make install (as root if needed, with 'su' or 'sudo')"
echo "  prefix...............: $prefix"
echo "  pythondir............: $pythondir"
echo "  pyexecdir............: $pyexecdir"
echo
echo
