AC_INIT(eupnp, 0.0.0.1, andre.dieb@gmail.com)
AC_PREREQ(2.52)
AC_CONFIG_SRCDIR(configure.ac)
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_ISC_POSIX

AM_INIT_AUTOMAKE(1.6 dist-bzip2)
AM_CONFIG_HEADER([config.h])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
_XTERM_COLORS

AC_PROG_CC
AM_PROG_CC_STDC
AM_PROG_CC_C_O
AC_HEADER_STDC
AC_C_CONST

define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl
AC_PROG_LIBTOOL

VMAJ=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $1);}'`
VMIN=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $2);}'`
VMIC=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $3);}'`
SNAP=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $4);}'`
version_info=`expr $VMAJ + $VMIN`":$VMIC:$VMIN"
AC_SUBST(VMAJ)
AC_SUBST(version_info)

AC_CHECK_FUNCS(realpath)

case "$host_os" in
   mingw32ce* | cegcc*)
      ;;
   *)
      E_UPNP_BUILD="-DE_UPNP_BUILD=1"
      ;;
esac
AC_SUBST(E_UPNP_BUILD)


# Required modules
PKG_CHECK_MODULES(EINA, [eina])
PKG_CHECK_MODULES(LIBXML2, [libxml-2.0])
PKG_CHECK_MODULES(EUPNP, eina libxml-2.0)


# Tests and coverage
EFL_CHECK_TESTS([enable_tests="yes"], [enable_tests="no"])
EFL_CHECK_COVERAGE([${enable_tests}], [enable_coverage="yes"], [enable_coverage="no"])


# Optional modules

## Ecore

AM_CONDITIONAL(HAVE_ECORE, false)
define([CHECK_MODULE_ECORE],
[
AC_CHECK_PKG(ECORE, ecore, [], [ECORE=false])
AC_CHECK_PKG(ECORECON, ecore-con, [], [ECORE=false])
])

AC_CHECK_PKG(ECORE, ecore, [], [ECORE=false])
AC_CHECK_PKG(ECORECON, ecore-con, [], [ECORE=false])

AC_OPTIONAL_MODULE([eupnp_ecore], true, [CHECK_MODULE_ECORE])


## AV

AM_CONDITIONAL(HAVE_AV, false)
define([CHECK_MODULE_AV],
[
AC_CHECK_PKG(EINA, eina, [], [EINA=false])
])
AC_OPTIONAL_MODULE([eupnp_av], true, [CHECK_MODULE_AV])


## Documentation

EFL_CHECK_DOXYGEN([build_doc="yes"], [build_doc="no"])


# Build Eupnp examples
want_examples="yes"
AC_ARG_ENABLE([examples],
   [AC_HELP_STRING([--disable-examples], [do not build examples. @<:@default=yes@:>@])],
   [want_examples=$enableval], [want_examples="no"])

AC_CHECK_PKG(ELM, elementary, [have_elm="yes"], [have_elm="no"])
AC_CHECK_PKG(EMOTION, emotion, [have_emotion="yes"], [have_emotion="no"])

AM_CONDITIONAL(EUPNP_BUILD_EXAMPLES, test "x${want_examples}" = "xyes")
AM_CONDITIONAL(EUPNP_BUILD_EXAMPLE_BROWSER, test "x${have_emotion}" = "xyes" -a "x${have_elm}" = "xyes")


# Build Eupnp tools
want_tools="no"
have_tools="no"

AC_ARG_ENABLE([tools],
   [AC_HELP_STRING([--disable-tools], [do not build tools. @<:@default=yes@:>@])],
   [want_tools=$enableval], [want_tools="no"])

if test "x$want_tools" = "xyes"; then
   AC_CHECK_PKG(ELM, elementary, [have_tools="yes"], [have_tools="no"])
else
   have_tools="no"
fi
if test "x$want_tools" = "xyes" -a "x$have_tools" = "xno"; then
   AC_MSG_ERROR([Tools build requested but elementary library was not found by pkg-config.])
fi

AM_CONDITIONAL(EUPNP_BUILD_TOOLS, test "x${have_tools}" = "xyes")


# Output and summary

AC_OUTPUT([
eupnp-0.pc
eupnp-ecore.pc
eupnp-av.pc
Makefile
doc/Makefile
src/Makefile
examples/Makefile
src/lib/Makefile
src/lib/eupnp_ecore/Makefile
src/lib/eupnp_av/Makefile
src/tests/Makefile
tools/Makefile
tools/igd/Makefile
tools/lighting/Makefile
])

txt_strip() {
        echo "[$]@" | sed -e 's/^[[ \t]]*\([[^ \t]]*\)[[ \t]]*$/\1/g'
}

MODS=""
for mod in $OPTIONAL_MODULES; do
        MODS="$MODS ${COLOR_HGREEN}+$mod${COLOR_END}"
done
MODS=$(txt_strip $MODS)

UNUSED_MODS=""
for mod in $UNUSED_OPTIONAL_MODULES; do
        UNUSED_MODS="$UNUSED_MODS ${COLOR_HRED}-$mod${COLOR_END}"
done
UNUSED_MODS=$(txt_strip $UNUSED_MODS)

cat << SUMMARY_EOF

Summary:
 * project.........: $PACKAGE $VERSION
 * prefix..........: $(txt_strip $prefix)
SUMMARY_EOF

echo -e " * modules.........: $MODS $UNUSED_MODS"
echo -e " * documentation...: $build_doc"
echo -e " * tools...........: $want_tools"
echo -e " * examples........: $want_examples"

echo

echo -e "Build documentation: make doc"
echo -e "Build tools........: make tools"

echo

echo -e "Compilation........: make"
echo -e "  CFLAGS...........: $CFLAGS"
echo -e "  CPPFLAGS.........: $CPPFLAGS"
echo -e "  CXXFLAGS.........: $CXXFLAGS"
echo -e "  LDFLAGS..........: $LDFLAGS"

echo

echo -e "Installation.......: make install (as root if needed, with 'su' or 'sudo')"
echo -e "  prefix...........: $prefix"
echo
