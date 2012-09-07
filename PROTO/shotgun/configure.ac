##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_define([v_maj], [0])
m4_define([v_min], [7])
m4_define([v_mic], [0])
m4_define([v_rev], m4_esyscmd([(svnversion "${SVN_REPO_PATH:-.}" | grep -v '\(export\|Unversioned directory\)' || echo 0) | awk -F : '{printf("%s\n", $1);}' | tr -d ' :MSP\n']))
m4_if(v_rev, [0], [m4_define([v_rev], m4_esyscmd([git log 2> /dev/null | (grep -m1 git-svn-id || echo 0) | sed -e 's/.*@\([0-9]*\).*/\1/' | tr -d '\n']))])
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_define([v_ver], [v_maj.v_min.v_mic])
m4_define([lt_rev], m4_eval(v_maj + v_min))
m4_define([lt_cur], v_mic)
m4_define([lt_age], v_min)
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##

AC_PREREQ(2.57)
AC_INIT([shotgun],[v_ver],[michael.blumenkrantz@gmail.com],[shotgun])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADER([config.h])
AC_GNU_SOURCE
AC_ISC_POSIX
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_ifdef([v_rev], , [m4_define([v_rev], [0])])
m4_ifdef([v_rel], , [m4_define([v_rel], [])])
AC_DEFINE_UNQUOTED(VMAJ, [v_maj], [Major version])
AC_DEFINE_UNQUOTED(VMIN, [v_min], [Minor version])
AC_DEFINE_UNQUOTED(VMIC, [v_mic], [Micro version])
AC_DEFINE(VREV, ["v_rev"], [Revison])
version_info="lt_rev:lt_cur:lt_age"
release_info="v_rel"
AC_SUBST(version_info)
AC_SUBST(release_info)
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
VMAJ=v_maj
AC_SUBST(VMAJ)

AM_INIT_AUTOMAKE([foreign])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_PROG_LIBTOOL
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_MAKE_SET
AC_HEADER_STDC
AC_TYPE_SIZE_T

AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_FUNC_MEMCMP

AC_FUNC_ALLOCA
AC_C___ATTRIBUTE__
CC_PIC="$lt_prog_compiler_pic"
CXX_PIC="$lt_prog_compiler_pic_CXX"
AC_SUBST([CC_PIC])
AC_SUBST([CXX_PIC])
AC_CHECK_HEADERS([limits.h])
AC_CHECK_FUNCS([memcpy])

PKG_PROG_PKG_CONFIG

# Check whether pkg-config supports Requires.private
if $PKG_CONFIG --atleast-pkgconfig-version 0.22; then
   pkgconfig_requires_private="Requires.private"
else
   pkgconfig_requires_private="Requires"
fi
AC_SUBST([pkgconfig_requires_private])


PKG_CHECK_MODULES([SHOTGUN], [eina >= 1.0.999 ecore >= 1.0.999 ecore-con >= 1.0.999])

have_gui="no"
want_gui="yes"
AC_ARG_ENABLE([gui],
   [AC_HELP_STRING([--disable-gui], [disable gui support. @<:@default=detect@:>@])],
   [want_gui=$enableval], [])

if test "x$want_gui" != "xno"; then
   PKG_CHECK_MODULES([GUI],
      [eet >= 1.4.0 evas >= 1.0.999 ecore-file >= 1.0.999 ecore-imf >= 1.0.999 edje >= 1.0.999 elementary efx],
      [have_gui="yes"],
      [have_gui="no"]
   )
else
    have_gui="no"
fi
AM_CONDITIONAL([GUI], [test "x${have_gui}" = "xyes"])

AC_ARG_WITH([edje-cc],
   [AC_HELP_STRING([--with-edje-cc=PATH],
                   [specify a specific path to edje_cc])],
   [edje_cc=$withval;
    AC_MSG_NOTICE([edje_cc explicitly set to $edje_cc])
   ],[edje_cc=$(pkg-config --variable=prefix edje)/bin/edje_cc])
AC_SUBST(edje_cc)

have_azy="no"
want_azy="no"
AC_ARG_ENABLE([azy],
   [AC_HELP_STRING([--disable-azy], [disable azy support. @<:@default=detect@:>@])],
   [want_azy=no], [])
##   [want_azy=$enableval], [])

if test "x$want_azy" != "xno"; then
   PKG_CHECK_MODULES([AZY],
      [azy >= 1.0.0],
      [
       AC_DEFINE(HAVE_AZY, 1, [Azy support])
       have_azy="yes"
      ],
      [have_azy="no"]
   )
else
    have_azy="no"
fi
if test "x$want_azy" = "xyes" -a "x$have_azy" = "xno"; then
    AC_MSG_ERROR([azy support requested, but not found by pkg-config.])
fi

have_edbus="no"
want_edbus="auto"
AC_ARG_ENABLE([edbus],
   [AC_HELP_STRING([--disable-edbus], [disable edbus support. @<:@default=detect@:>@])],
   [want_edbus=$enableval], [])

if test "x$want_edbus" != "xno"; then
   PKG_CHECK_MODULES([edbus],
      [edbus >= 1.0.0],
      [
       AC_DEFINE(HAVE_DBUS, 1, [DBUS support])
       have_edbus="yes"
      ],
      [have_edbus="no"]
   )
else
    have_edbus="no"
fi
if test "x$want_edbus" = "xyes" -a "x$have_edbus" = "xno"; then
    AC_MSG_ERROR([edbus support requested, but not found by pkg-config.])
fi

have_enotify="no"
want_enotify="auto"
AC_ARG_ENABLE([enotify],
   [AC_HELP_STRING([--disable-enotify], [disable enotify support. @<:@default=detect@:>@])],
   [want_enotify=$enableval], [])

if test "x$want_enotify" != "xno"; then
   PKG_CHECK_MODULES([enotify],
      [enotify >= 1.0.0],
      [
       AC_DEFINE(HAVE_NOTIFY, 1, [DBUS Notification support])
       have_enotify="yes"
      ],
      [have_enotify="no"]
   )
else
    have_enotify="no"
fi
if test "x$want_enotify" = "xyes" -a "x$have_enotify" = "xno"; then
    AC_MSG_ERROR([enotify support requested, but not found by pkg-config.])
fi

#EFL_CHECK_DOXYGEN([build_doc="yes"], [build_doc="no"])
# write output
AC_CONFIG_FILES([
  Makefile
  shotgun.pc
  data/Makefile
  data/theme/Makefile
  src/Makefile
  src/lib/Makefile
  src/bin/Makefile
])
AC_OUTPUT
