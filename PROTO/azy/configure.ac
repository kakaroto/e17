##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_define([v_maj], [1])
m4_define([v_min], [0])
m4_define([v_mic], [0])
m4_define([v_rev], m4_esyscmd([(svnversion "${SVN_REPO_PATH:-.}" | grep -v export || echo 0) | awk -F : '{printf("%s\n", $1);}' | tr -d ' :MSP\n']))
m4_if(v_rev, [0], [m4_define([v_rev], m4_esyscmd([git log 2> /dev/null | (grep -m1 git-svn-id || echo 0) | sed -e 's/.*@\([0-9]*\).*/\1/' | tr -d '\n']))])
##--   When released, remove the dnl on the below line
m4_undefine([v_rev])
##--   When doing snapshots - change soname. remove dnl on below line
dnl m4_define([relname], [ver-pre-svn-07])
dnl m4_define([v_rel], [-release relname])
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_ifdef([v_rev], [m4_define([v_ver], [v_maj.v_min.v_mic.v_rev])], [m4_define([v_ver], [v_maj.v_min.v_mic])])
m4_define([lt_rev], m4_eval(v_maj + v_min))
m4_define([lt_cur], v_mic)
m4_define([lt_age], v_min)
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##

AC_PREREQ(2.57)
AC_INIT([azy],[1.0.v_rev],[mike@zentific.com],[azy])
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
AC_DEFINE_UNQUOTED(VREV, [v_rev], [Revison])
version_info="lt_rev:lt_cur:lt_age"
release_info="v_rel"
AC_SUBST(version_info)
AC_SUBST(release_info)
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
VMAJ=v_maj
AC_SUBST(VMAJ)

AM_INIT_AUTOMAKE([foreign])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES])

AC_PROG_LIBTOOL
AC_PROG_YACC
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

AC_CHECK_SIZEOF([char])
AC_CHECK_SIZEOF([short])
AC_CHECK_SIZEOF([int])
AC_CHECK_SIZEOF([long])

AC_FUNC_ALLOCA
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_C___ATTRIBUTE__
AC_CHECK_FUNCS([strdup strndup])
CC_PIC="$lt_prog_compiler_pic"
CXX_PIC="$lt_prog_compiler_pic_CXX"
AC_SUBST([CC_PIC])
AC_SUBST([CXX_PIC])


AZY_REQUIRES="ecore-con >= 1.0.0 ecore >= 1.0.0 eina >= 1.0.0"

want_mysql=
AC_ARG_ENABLE([mysql-tests],
   [AC_HELP_STRING([--disable-mysql-tests], [disable code for mysql in tests @<:@default=detect@:>@])],
   [want_mysql=$enableval]
)

if test "x$want_mysql" != "xno" ; then
   AC_PATH_PROG([mysql_config], [mysql_config])
   if test "x$mysql_config" != "x";then
      AC_CHECK_HEADERS([mysql/mysql.h])
      AC_CHECK_HEADERS([mysql.h])
      MYSQL_CFLAGS="$($mysql_config --cflags)"                                                                 
      MYSQL_LIBS="$($mysql_config --libs_r)"
      AC_DEFINE_UNQUOTED(HAVE_MYSQL, 1, "enable some mysql code in tests")
   fi
fi
AM_CONDITIONAL(HAVE_MYSQL, [test -n "$MYSQL_LIBS"])
AC_SUBST(MYSQL_CFLAGS)
AC_SUBST(MYSQL_LIBS)

PKG_CHECK_MODULES(AZY, [$AZY_REQUIRES])
cflags="$CFLAGS"
CFLAGS="$AZY_CFLAGS"
AC_CHECK_LIB([eina], [eina_error_find], [AC_DEFINE_UNQUOTED([HAVE_EINA_ERROR_FIND], [1], [eina_error_find is present])], [])
CFLAGS="$cflags"
want_xml=
AC_ARG_ENABLE([xml],
   [AC_HELP_STRING([--enable-xml], [enable xml parser support @<:@default=yes@:>@])],
   [want_xml=$enableval]
)

if test "x${want_xml}" != "xno"; then
   AC_DEFINE([HAVE_XML], [1], [xml support is enabled])
fi
AM_CONDITIONAL([HAVE_XML], [test "x${want_xml}" != "xno"])

AC_SUBST(AZY_REQUIRES)

want_extern_rec=
AC_ARG_ENABLE([external-re2c],
   [AC_HELP_STRING([--disable-external-re2c], [disable use of external re2c @<:@default=detect@:>@])],
   [want_extern_rec=$enableval]
)
if test "x$want_extern_rec" != "xno" ; then
   AC_PATH_PROG([RE2C], [re2c])
fi
AM_CONDITIONAL([NEED_RE2C], [test -z $RE2C])


AC_CHECK_HEADERS([Rpcdce.h])
EFL_CHECK_DOXYGEN([build_doc="yes"], [build_doc="no"])
# write output
AC_CONFIG_FILES([
  Makefile
  azy.spec
  doc/azy.dox
  doc/Makefile
  src/Makefile
  src/lib/Makefile
  src/include/Makefile
  src/lib/extras/Makefile
  src/bin/Makefile
  src/bin/re2c/Makefile
  src/tests/Makefile
  src/tests/unit/Makefile
  src/tests/identi.ca/Makefile
  src/tests/rss/Makefile
  azy.pc
])
AC_OUTPUT
