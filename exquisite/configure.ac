dnl Process this file with autoconf to produce a configure script.

# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT([exquisite], [1.0.0], [enlightenment-devel@lists.sourceforge.net])
AC_PREREQ([2.52])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_HEADERS([config.h])
AH_TOP([
#ifndef EFL_CONFIG_H__
#define EFL_CONFIG_H__
])
AH_BOTTOM([
#endif /* EFL_CONFIG_H__ */
])

AM_INIT_AUTOMAKE([1.6 dist-bzip2])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AM_PROG_CC_STDC
AM_PROG_CC_C_O

requirements="\
   eina >= 1.0.0 \
   eet >= 1.4.0 \
   evas >= 1.0.0 \
   ecore >= 1.0.0 \
   ecore-evas >= 1.0.0 \
   ecore-con >= 1.0.0 \
   edje >= 1.0.0"
requirements_write="\
   ecore >= 1.0.0 \
   ecore-con >= 1.0.0"

have_ecore_fb="yes"
PKG_CHECK_EXISTS([ecore-fb >= 1.0.0],
   [
    AC_DEFINE(HAVE_ECORE_FB, 1, [Have Ecore-Fb support])
    requirements="${requirements} \
                  ecore-fb >= 1.0.0"
   ],
   [have_ecore_fb="no"])

PKG_CHECK_MODULES([EXQUISITE], [${requirements}])
PKG_CHECK_MODULES([EXQUISITE_WRITE], [${requirements_write}])

EFL_WITH_BIN([edje], [edje-cc], [edje_cc])

AC_CONFIG_FILES([
Makefile
src/Makefile
src/bin/Makefile
data/Makefile
data/fonts/Makefile
data/themes/Makefile
data/themes/images/Makefile
demo/Makefile
])

AC_OUTPUT

#####################################################################
## Info

echo
echo
echo
echo "------------------------------------------------------------------------"
echo "$PACKAGE $VERSION"
echo "------------------------------------------------------------------------"
echo
echo "Configuration Options Summary:"
echo
echo "  Frame Buffer.............: ${have_ecore_fb}"
echo
echo "Compilation................: make (or gmake)"
echo "  CPPFLAGS.................: $CPPFLAGS"
echo "  CFLAGS...................: $CFLAGS"
echo "  LDFLAGS..................: $LDFLAGS"
echo
echo "Installation...............: make install (as root if needed, with 'su' or 'sudo')"
echo "  prefix...................: $prefix"
echo
