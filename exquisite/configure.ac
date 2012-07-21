##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_define([v_maj], [1])
m4_define([v_min], [6])
m4_define([v_mic], [99])
m4_define([v_rev], m4_esyscmd([(svnversion "${SVN_REPO_PATH:-.}" | grep -v '\(export\|Unversioned directory\)' || echo 0) | awk -F : '{printf("%s\n", $1);}' | tr -d ' :MSP\n']))
m4_if(v_rev, [0], [m4_define([v_rev], m4_esyscmd([git log 2> /dev/null | (grep -m1 git-svn-id || echo 0) | sed -e 's/.*@\([0-9]*\).*/\1/' | tr -d '\n']))])
##--   When released, remove the dnl on the below line
dnl m4_undefine([v_rev])
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

AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_ifdef([v_rev], , [m4_define([v_rev], [0])])
m4_ifdef([v_rel], , [m4_define([v_rel], [])])
AC_DEFINE_UNQUOTED(VMAJ, [v_maj], [Major version])
AC_DEFINE_UNQUOTED(VMIN, [v_min], [Minor version])
AC_DEFINE_UNQUOTED(VMIC, [v_mic], [Micro version])
AC_DEFINE_UNQUOTED(VREV, [v_rev], [Revison])
version_info="lt_cur:lt_rev:lt_age"
release_info="v_rel"
AC_SUBST(version_info)
AC_SUBST(release_info)
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
VMAJ=v_maj
AC_SUBST(VMAJ)

### Checks for linker characteristics

# use --enable-auto-import on Windows

lt_enable_auto_import=""
case "$host_os" in
   mingw*)
      lt_enable_auto_import="-Wl,--enable-auto-import"
      ;;
esac
AC_SUBST([lt_enable_auto_import])

requirements_exquisite="\
   evas >= 1.0.0 \
   edje >= 1.0.0"

requirements_exquisite_bin="\
   eina >= 1.0.0 \
   eet >= 1.4.0 \
   evas >= 1.0.0 \
   ecore >= 1.0.0 \
   ecore-evas >= 1.0.0 \
   ecore-con >= 1.0.0 \
   edje >= 1.0.0"
requirements_exquisite_write="\
   ecore >= 1.0.0 \
   ecore-con >= 1.0.0"

have_ecore_fb="yes"
PKG_CHECK_EXISTS([ecore-fb >= 1.0.0],
   [
    AC_DEFINE(HAVE_ECORE_FB, 1, [Have Ecore-Fb support])
    requirements_exquisite_bin="${requirements_exquisite_bin} \
                  ecore-fb >= 1.0.0"
   ],
   [have_ecore_fb="no"])

EFL_ENABLE_BIN([exquisite-binaries])

if test "x${have_exquisite_binaries}" = "xyes"; then
   PKG_CHECK_MODULES([EXQUISITE_READ], [${requirements_exquisite_bin}])
   PKG_CHECK_MODULES([EXQUISITE_WRITE], [${requirements_exquisite_write}])
fi
PKG_CHECK_MODULES([EXQUISITE], [${requirements_exquisite}])

EFL_WITH_BIN([edje], [edje-cc], [edje_cc])

EFL_CHECK_PATH_MAX

AC_SUBST(requirements_exquisite)

AC_CONFIG_FILES([
Makefile
exquisite.pc
src/Makefile
src/bin/Makefile
src/lib/Makefile
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
echo "  Building exquisite binaries .........: ${have_exquisite_binaries}"
echo "  Frame Buffer.........................: ${have_ecore_fb}"
echo
echo "Compilation............................: make (or gmake)"
echo "  CPPFLAGS.............................: $CPPFLAGS"
echo "  CFLAGS...............................: $CFLAGS"
echo "  LDFLAGS..............................: $LDFLAGS"
echo
echo "Installation...........................: make install (as root if needed, with 'su' or 'sudo')"
echo "  prefix...............................: $prefix"
echo
