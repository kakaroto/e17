##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_define([v_maj], [1])
m4_define([v_min], [0])
m4_define([v_mic], [0])
m4_define([v_rev], m4_esyscmd([(svnversion "${SVN_REPO_PATH:-.}" | grep -v '\(export\|Unversioned directory\)' || echo 0) | awk -F : '{printf("%s\n", $1);}' | tr -d ' :MSP\n']))
m4_if(v_rev, [0], [m4_define([v_rev], m4_esyscmd([git log 2> /dev/null | (grep -m1 git-svn-id || echo 0) | sed -e 's/.*@\([0-9]*\).*/\1/' | tr -d '\n']))])
##--   When released, remove the dnl on the below line
dnl m4_undefine([v_rev])
##--   When doing snapshots - change soname. remove dnl on below line
m4_define([relname], [ver-pre-svn-09])
m4_define([v_rel], [-release relname])
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_ifdef([v_rev], [m4_define([v_ver], [v_maj.v_min.v_mic.v_rev])],
[m4_define([v_ver], [v_maj.v_min.v_mic])])
m4_define([lt_rev], m4_eval(v_maj + v_min))
m4_define([lt_cur], v_mic)
m4_define([lt_age], v_min)
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##

AC_INIT([esskyuehl], [v_ver], [michael.blumenkrantz@gmail.com], [esskyuehl])
AC_PREREQ([2.57])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AM_CONFIG_HEADER([config.h])

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

AM_INIT_AUTOMAKE([1.6 dist-bzip2])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_PROG_LIBTOOL
AC_PROG_INSTALL
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_MAKE_SET
AC_C___ATTRIBUTE__
AC_FUNC_ALLOCA

CC_PIC="$lt_prog_compiler_pic"
AC_SUBST([CC_PIC])

case "$host_os" in
   mingw32ce*)
      MODULE_ARCH="$host_os-$host_cpu"
      ;;
   *)
      MODULE_ARCH="$host_os-$host_cpu-v_maj.v_min.v_mic"
      ;;
esac
AC_SUBST(MODULE_ARCH)
AC_DEFINE_UNQUOTED(MODULE_ARCH, "${MODULE_ARCH}", "Module architecture")

case "$host_os" in
   mingw*)
      MODULE_EXT=".dll"
      ;;
   *)
      MODULE_EXT=".so"
      ;;
esac

AC_DEFINE_UNQUOTED(SHARED_LIB_SUFFIX, "${MODULE_EXT}", [Suffix for shared objects])

requirement_esskyuehl="eina >= 1.1.0 ecore >= 1.1.99"
AC_SUBST(requirement_esskyuehl)

PKG_CHECK_MODULES([EFL], [$requirement_esskyuehl])

AC_ARG_ENABLE([mysql],
   [AC_HELP_STRING([--disable-mysql], [disable mysql module @<:@default=detect@:>@])],
   [
    if test "x${enableval}" = "xyes" ; then
       want_mysql="yes"
    else
       want_mysql="no"
    fi
   ],
   [want_mysql="yes"])

if test "x${want_mysql}" != "xno" ; then
   AC_MSG_CHECKING([for mysql])
   AC_PATH_PROG([mysql_config], [mysql_config])
   if test "x$mysql_config" != "x";then
         MYSQL_CFLAGS="$($mysql_config --cflags)"
         MYSQL_LIBS="$($mysql_config --libs)"
         cflags="$CFLAGS"
         cppflags="$CPPFLAGS"
         CFLAGS="$MYSQL_CFLAGS"
         CPPFLAGS="$CFLAGS"
         AC_CHECK_HEADERS([mysql/mysql.h mysql.h])
         CFLAGS="$cflags"
         CPPFLAGS="$cppflags"
         if test "x$ac_cv_header_mysql_h" != "xno" -o "x$ac_cv_header_mysql_mysql_h" != "xno" ; then
            AC_MSG_RESULT([yes])
            mysql=mysql
         else
            AC_MSG_RESULT([no])
         fi
   else
         AC_MSG_RESULT([no])
   fi
   AM_CONDITIONAL([MYSQL], [test -n "$MYSQL_LIBS"])
   AC_SUBST(MYSQL_CFLAGS)
   AC_SUBST(MYSQL_LIBS)
fi

AC_ARG_ENABLE([postgresql],
   [AC_HELP_STRING([--disable-postgresql], [disable postgresql module @<:@default=detect@:>@])],
   [
    if test "x${enableval}" = "xyes" ; then
       want_postgresql="yes"
    else
       want_postgresql="no"
    fi
   ],
   [want_postgresql="yes"])

if test "x${want_postgresql}" != "xno" ; then
   AC_MSG_CHECKING([for postgresql])
   AC_PATH_PROG([pg_config], [pg_config])
   if test "x$pg_config" != "x";then
         POSTGRESQL_CFLAGS="-I$($pg_config --includedir) -I$($pg_config --includedir-server)"
         POSTGRESQL_LIBS="-L$($pg_config --libdir) -lpq"
         cflags="$CFLAGS"
         CFLAGS="$POSTGRESQL_CFLAGS"
         CPPFLAGS="$CFLAGS"
         AC_CHECK_HEADERS([libpq-fe.h])
         CFLAGS="$cflags"
         CPPFLAGS="$cppflags"
         AC_MSG_RESULT([yes])
         postgresql=postgresql
   else
         AC_MSG_RESULT([no])
   fi
   AM_CONDITIONAL([POSTGRESQL], [test -n "$POSTGRESQL_LIBS"])
   AC_SUBST(POSTGRESQL_CFLAGS)
   AC_SUBST(POSTGRESQL_LIBS)
fi

AC_ARG_ENABLE([sqlite],
   [AC_HELP_STRING([--disable-sqlite], [disable sqlite module @<:@default=detect@:>@])],
   [
    if test "x${enableval}" = "xyes" ; then
       want_sqlite="yes"
    else
       want_sqlite="no"
    fi
   ],
   [want_sqlite="yes"])

if test "x${want_sqlite}" != "xno" ; then
   PKG_CHECK_MODULES([SQLITE3], [sqlite3])
   test -n "$SQLITE3_LIBS" && sqlite=sqlite
fi
AM_CONDITIONAL([SQLITE], [test -n "$SQLITE3_LIBS"])


EFL_CHECK_DOXYGEN([build_doc="yes"], [build_doc="no"])

AC_OUTPUT([
Makefile
doc/esskyuehl.dox
doc/Makefile
src/Makefile
src/lib/Makefile
src/modules/Makefile
src/modules/postgresql/Makefile
src/modules/sqlite/Makefile
src/modules/mysql/Makefile
src/modules/mysql/mysac/Makefile
src/tests/Makefile
esskyuehl.pc
])


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
echo "Documentation..........: ${build_doc}"
echo
echo "Backend support........: $mysql $postgresql $sqlite"
echo
echo "Compilation............: make (or gmake)"
echo "  CPPFLAGS.............: $CPPFLAGS"
echo "  CFLAGS...............: $CFLAGS"
echo "  LDFLAGS..............: $LDFLAGS"
echo
echo "Installation...........: make install (as root if needed, with 'su' or 'sudo')"
echo "  prefix...............: $prefix"
echo
