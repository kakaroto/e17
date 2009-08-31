AC_INIT(eupnp, 0.0.0.1, andre.dieb@gmail.com)
AC_PREREQ(2.52)
AC_CONFIG_SRCDIR(configure.ac)
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_ISC_POSIX

AM_INIT_AUTOMAKE(1.6 dist-bzip2)
AM_CONFIG_HEADER(config.h)
AC_SUBST(ACLOCAL_AMFLAGS, "-I m4")
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

AM_CONDITIONAL(HAVE_ECORE, false)
define([CHECK_MODULE_ECORE],
[
    AC_CHECK_PKG(ECORE, ecore, [], [ECORE=false])
    AC_CHECK_PKG(ECORECON, ecore-con, [], [ECORE=false])
])

# required modules
PKG_CHECK_MODULES(EINA, [eina-0])
PKG_CHECK_MODULES(LIBXML2, [libxml-2.0])
PKG_CHECK_MODULES(EUPNP, eina-0 libxml-2.0)

# Tests and coverage
EFL_CHECK_TESTS([enable_tests="yes"], [enable_tests="no"])
EFL_CHECK_COVERAGE([${enable_tests}], [enable_coverage="yes"], [enable_coverage="no"])

# Optional modules
AC_OPTIONAL_MODULE([eupnp_ecore], true, [CHECK_MODULE_ECORE])

PKG_CHECK_MODULES(ELM, elementary)

AC_OUTPUT([
eupnp-0.pc
eupnp-ecore.pc
Makefile
src/Makefile
src/bin/Makefile
src/lib/Makefile
src/lib/eupnp_ecore/Makefile
src/tests/Makefile
tools/Makefile
])


# report
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
 * project........: $PACKAGE $VERSION
 * prefix.........: $(txt_strip $prefix)
 * CFLAGS.........: $(txt_strip $CFLAGS)
SUMMARY_EOF

echo -e " * modules........: $MODS $UNUSED_MODS"
