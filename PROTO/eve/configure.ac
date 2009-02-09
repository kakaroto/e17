AC_INIT([eve], [0.1.0], [enlightenment-devel@lists.sourceforge.net])
AC_PREREQ([2.52])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_HOST

AM_INIT_AUTOMAKE(1.6 dist-bzip2)
AM_CONFIG_HEADER(config.h)


### Checks for programs
AC_PROG_CC


### Checks for libraries

PKG_CHECK_MODULES([EVE], [ecore-evas edje ewebkit])


### Checks for compiler characteristics

AM_PROG_CC_C_O


### Checks for linker characteristics

# use --enable-auto-import on Windows

ld_enable_auto_import=""
case "$host_os" in
   mingw* | cegcc*)
      ld_enable_auto_import="-Wl,--enable-auto-import"
esac
AC_SUBST(ld_enable_auto_import)

# use --as-needed if supported

LDFLAGS_SAVE=${LDFLAGS}
LDFLAGS="${LDFLAGS} -Wl,--as-needed"

AC_LINK_IFELSE(
   [AC_LANG_PROGRAM([[]], [])],
   [ld_as_needed="-Wl,--as-needed"],
   [ld_as_needed=""]
)

LDFLAGS=${LDFLAGS_SAVE}
AC_SUBST(ld_as_needed)


AC_CONFIG_FILES([
Makefile
data/Makefile
data/theme/Makefile
data/theme/default/Makefile
data/theme/b_and_w/Makefile
src/Makefile
src/bin/Makefile
])

AC_OUTPUT

echo
echo
echo "------------------------------------------------------------------------"
echo "$PACKAGE_NAME $PACKAGE_VERSION"
echo "------------------------------------------------------------------------"
echo
echo "Configuration Options Summary:"
echo
echo "  Compilation..........: make"
echo
echo "  Installation.........: make install"
echo
echo "    prefix.............: $prefix"
echo
