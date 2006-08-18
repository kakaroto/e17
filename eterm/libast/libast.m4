dnl#####################################################################
dnl# Autoconf m4 macros for LibAST
dnl# $Id$
dnl#####################################################################

dnl#
dnl# Check for LibAST and support components
dnl#    - arg 1 is the env variable to set
dnl#    - arg 2 is the LIBS variable to use
dnl#
AC_DEFUN([AST_CHECK_LIBAST], [
    AC_CHECK_PROG(LIBAST_CONFIG, libast-config, libast-config, false)
    if test "$LIBAST_CONFIG" = "false"; then
        $1=0
    else
        $1=1
        CPPFLAGS="$CPPFLAGS `$LIBAST_CONFIG --cppflags`"
        LDFLAGS="$LDFLAGS `$LIBAST_CONFIG --ldflags`"
    fi
    AC_CHECK_HEADERS(libast.h, [
        AC_CHECK_LIB(ast, libast_malloc, $1=1, [ AC_CHECK_LIB(ast, spifmem_malloc, $1=0, ${$2}) ]
    ], $1=0)

    if test "${$1}" -ne "1"; then
        echo "ERROR:  You need the LibAST package to build Eterm.  If you already have it,";
        echo "        you may have it installed in a strange place, or you may need to run";
        echo "        /sbin/ldconfig.  If you don't have it, I can download it for you.";
        echo "        Shall I retrieve and build LibAST now (y/n)?";
        read ANS
        if test "x$ANS" = "xy" -o "x$ANS" = "xyes" -o "x$ANS" = "xY" -o "x$ANS" = "xYES"; then
            # Download from CVS server
            CVSROOT=":pserver:anonymous@cvs.enlightenment.sourceforge.net:/cvsroot/enlightenment"
            test -f $HOME/.cvspass || touch $HOME/.cvspass
            grep $CVSROOT $HOME/.cvspass >/dev/null 2>&1 || cvs -d $CVSROOT login
            cvs -z3 -d $CVSROOT co -d libast eterm/libast
            (cd libast && ./autogen.sh $ac_configure_args && make install && cd .. && rm -rf libast)
            if test $? -ne 0; then
                echo 'ERROR:  Unable to auto-get libast, sorry.' 1>&2
                exit 1
            fi
            $1=1
            AC_CHECK_PROG(LIBAST_CONFIG, libast-config, libast-config, false)
            test "$LIBAST_CONFIG" = "false" && $1=0
        fi
    fi
    if test "${$1}" -eq "1"; then
        if test ! -z "$LIBAST_CONFIG"; then
            $2="-last ${$2}"
            AC_DEFINE([HAVE_LIBAST], [1], [Define if the LibAST library is present.])
            test "$prefix" = "NONE" && prefix="`$LIBAST_CONFIG --prefix`"
            SUPPORT_FLAGS="`$LIBAST_CONFIG --support`"
            for i in $SUPPORT_FLAGS ; do
                case $i in
                    MMX)
                        AC_DEFINE([LIBAST_MMX_SUPPORT], [1], [Defined if LibAST has MMX support.])
                        ;;
                    X11)
                        AC_DEFINE([LIBAST_X11_SUPPORT], [1], [Defined if LibAST has X11 support.])
                        ;;
                    Imlib2)
                        AC_DEFINE([LIBAST_IMLIB2_SUPPORT], [1], [Defined if LibAST has Imlib2 support.])
                        ;;
                esac
            done
        fi
    fi
])

dnl#
dnl# LibAST macro for determining integer types by size
dnl#
AC_DEFUN([AST_SIZE_TYPE], [
    BIT_SIZE=[$1]
    BYTE_SIZE=`expr $BIT_SIZE '/' 8`
    case $BYTE_SIZE in
        $ac_cv_sizeof_char)       eval INT_${BIT_SIZE}_TYPE=char ;;
        $ac_cv_sizeof_short)      eval INT_${BIT_SIZE}_TYPE=short ;;
        $ac_cv_sizeof_int)        eval INT_${BIT_SIZE}_TYPE=int ;;
        $ac_cv_sizeof_long)       eval INT_${BIT_SIZE}_TYPE=long ;;
        $ac_cv_sizeof_long_long)  eval INT_${BIT_SIZE}_TYPE="'long long'" ;;
    esac
    test -z "`eval echo '$'INT_${BIT_SIZE}_TYPE`" && eval INT_${BIT_SIZE}_TYPE=long
])

dnl#
dnl# LibAST macro for determining regexp support
dnl#    - arg 1 is the name of the env var to use
dnl#
AC_DEFUN([AST_REGEXP_SUPPORT], [
    if test "${$1}" != "no"; then
        if test "${$1}" = "pcre" -o "${$1}" = "yes" ; then
            GOT_PCRE_HEADER=0
            GOT_PCRE_LIB=0
            AC_CHECK_HEADERS(pcre.h pcre/pcre.h, [
                                 GOT_PCRE_HEADER=1
                                 break
                             ])
            AC_SEARCH_LIBS(pcre_compile, pcre, [GOT_PCRE_LIB=1])
            if test $GOT_PCRE_HEADER -eq 1 -a $GOT_PCRE_LIB -eq 1 ; then
                AC_DEFINE([LIBAST_REGEXP_SUPPORT_PCRE], [1], [Build LibAST with PCRE support.])
                LIBAST_REGEXP_SUPPORT="regexp-pcre"
                $1="pcre"
            else
                $1="yes"
            fi
        fi
        if test "${$1}" = "posix" -o "${$1}" = "yes" ; then
            GOT_POSIXREGEXP_HEADER=0
            GOT_POSIXREGEXP_LIB=0
            AC_CHECK_HEADERS(regex.h, [
                                 GOT_POSIXREGEXP_HEADER=1
                                 break
                             ])
            AC_SEARCH_LIBS(regcomp, posix regexp regex re, [GOT_POSIXREGEXP_LIB=1])
            if test $GOT_POSIXREGEXP_HEADER -eq 1 -a $GOT_POSIXREGEXP_LIB -eq 1 ; then
                AC_DEFINE([LIBAST_REGEXP_SUPPORT_POSIX], [1], [Build LibAST with POSIX-style regexp support.])
                LIBAST_REGEXP_SUPPORT="regexp-posix"
                $1="posix"
            else
                $1="yes"
            fi
        fi
        if test "${$1}" = "bsd" -o "${$1}" = "yes" ; then
            GOT_BSD_HEADER=0
            GOT_BSD_LIB=0
            AC_CHECK_HEADERS(regex.h, [
                                 GOT_BSD_HEADER=1
                                 break
                             ])
            AC_SEARCH_LIBS(re_comp, bsd ucb regexp regex re, [GOT_BSD_LIB=1])
            if test $GOT_BSD_HEADER -eq 1 -a $GOT_BSD_LIB -eq 1 ; then
                AC_DEFINE([LIBAST_REGEXP_SUPPORT_BSD], [1], [Build LibAST with BSD-style regexp support.])
                LIBAST_REGEXP_SUPPORT="regexp-bsd"
                $1="bsd"
            else
                $1="yes"
            fi
        fi
        if test "${$1}" = "yes" ; then
            LIBAST_REGEXP_SUPPORT=""
            $1="no"
        fi
    else
        LIBAST_REGEXP_SUPPORT=""
        $1="no"
    fi
    AC_SUBST(LIBAST_REGEXP_SUPPORT)
])

dnl#
dnl# LibAST macro for X11 support
dnl#
AC_DEFUN([AST_X11_SUPPORT], [
    AC_PATH_XTRA
    if test ! -z "$X_CFLAGS"; then
        if test -z "$CPPFLAGS"; then
            CPPFLAGS="$X_CFLAGS"
        else
            CPPFLAGS="$CPPFLAGS $X_CFLAGS"
        fi
    fi
    if test ! -z "$X_LIBS"; then
        if test -z "$LDFLAGS"; then
            LDFLAGS="$X_LIBS"
        else
            LDFLAGS="$LDFLAGS $X_LIBS"
        fi
    fi
    LIBAST_X11_SUPPORT=""
    if test "x$no_x" != "xyes"; then
        AC_CHECK_LIB(X11, XOpenDisplay, [
                         LIBAST_X11_SUPPORT="X11"
                         GRLIBS="-lX11"
                         AC_DEFINE([LIBAST_X11_SUPPORT], [1], [Define for X11 support.])
                     ])
    fi
    AC_SUBST(LIBAST_X11_SUPPORT)
])

dnl#
dnl# LibAST macro for Imlib2 support
dnl#
AC_DEFUN([AST_IMLIB2_SUPPORT], [
    AC_ARG_WITH(imlib,
    [  --with-imlib[=DIR]        compile with Imlib2 support (default)],
    [
        if test "$withval" != "no"; then 
            if test "$withval" != "yes"; then
                CPPFLAGS="$CPPFLAGS -I${withval}/include"
                LDFLAGS="$LDFLAGS -L${withval}/lib"
            fi
            USE_IMLIB=1
        else
            USE_IMLIB=0
        fi
    ], [
        USE_IMLIB=1
    ])
    LIBAST_IMLIB2_SUPPORT=""
    if test $USE_IMLIB -eq 1 ; then
        AC_CHECK_PROG(IMLIB2_CONFIG, imlib2-config, imlib2-config)
            if test "x$IMLIB2_CONFIG" != "x"; then
                GRLIBS="`$IMLIB2_CONFIG --libs`"
                CFLAGS="$CFLAGS `$IMLIB2_CONFIG --cflags`"
                AC_DEFINE([LIBAST_IMLIB2_SUPPORT], [1], [Define for Imlib2 support.])
                LIBAST_IMLIB2_SUPPORT="Imlib2"
            else
                AC_CHECK_LIB(m, pow, LIBS="-lm $LIBS")
                AC_CHECK_LIB(dl, dlopen, LIBS="-ldl $LIBS")
                AC_CHECK_LIB(freetype, FT_Init_FreeType, GRLIBS="-lfreetype $GRLIBS", , $GRLIBS)
                AC_CHECK_LIB(Imlib2, imlib_create_image, [
                                GRLIBS="-lImlib2 $GRLIBS"
                                AC_DEFINE([LIBAST_IMLIB2_SUPPORT], [1], [Define for Imlib2 support.])
                                LIBAST_IMLIB2_SUPPORT="Imlib2"
                     ], [
                         AC_WARN(*** Imlib2 support has been disabled because Imlib2 ***)
                         AC_WARN(*** was not found or could not be linked.           ***)
                     ], $GRLIBS)
            fi
    fi
    AC_SUBST(LIBAST_IMLIB2_SUPPORT)
])

dnl#
dnl# LibAST macro for MMX support
dnl#
AC_DEFUN([AST_MMX_SUPPORT], [
    AC_MSG_CHECKING(for MMX support)
    HAVE_MMX=""
    AC_ARG_ENABLE(mmx, [  --enable-mmx            enable MMX assembly routines], [
                     test x$enableval = xyes && HAVE_MMX="yes"
                  ], [
                     if test x$build_os = xlinux-gnu; then
                         grep mmx /proc/cpuinfo >/dev/null 2>&1 && HAVE_MMX="yes"
                     fi
                  ])
    LIBAST_MMX_SUPPORT=""
    if test -n "$HAVE_MMX"; then
        AC_MSG_RESULT(yes)
        AC_DEFINE([LIBAST_MMX_SUPPORT], [1], [Define for MMX support.])
        LIBAST_MMX_SUPPORT="MMX"
    else
        AC_MSG_RESULT(no)
    fi
    AC_SUBST(LIBAST_MMX_SUPPORT)
])

dnl#
dnl# LibAST macros for standard checks
dnl#
AC_DEFUN([AST_STD_CHECKS], [
    AC_PROG_CPP

    dnl# These must be run after AC_PROG_CC but before any other macros that use
    dnl# the C compiler
    AC_AIX
    AC_ISC_POSIX
    AC_MINIX

    dnl# At least make the attempt to support CygWin32
    AC_CYGWIN
    AC_ARG_PROGRAM

    AM_PROG_LIBTOOL

    AC_GCC_TRADITIONAL

    AC_PROG_INSTALL

    ASFLAGS="$ASFLAGS -I../"
    AS=$CC
    AC_SUBST(ASFLAGS)
    AC_SUBST(AS)

    dnl# Check for host system type
    AC_CANONICAL_HOST

    dnl# Check the sanity of what we've done so far
    AM_SANITY_CHECK

    dnl# Most people don't want the developer-only clutter
    AM_MAINTAINER_MODE

    dnl# If it's there, what the hell?
    AM_WITH_DMALLOC
])
AC_DEFUN([AST_PROG_CHECKS], [
    AC_CHECK_PROG(SED, sed, sed, false)
    AC_CHECK_PROG(RM, rm, rm, true)
    AC_CHECK_PROG(CP, cp, cp, false)
    AC_CHECK_PROG(CHMOD, chmod, chmod, true)
    AC_CHECK_PROG(TAR, tar, tar, tar)
    AC_CHECK_PROG(MKDIR, mkdir, mkdir, false)
    AC_CHECK_PROG(CTAGS, ctags, ctags, true)
    AC_CHECK_PROG(AR, ar, ar, false)
    AC_CHECK_PROG(MV, mv, mv, true)
    AC_LN_S
])
AC_DEFUN([AST_VAR_CHECKS], [
    AC_CHECK_SIZEOF(char, 1)
    AC_CHECK_SIZEOF(short, 2)
    AC_CHECK_SIZEOF(int, 4)
    AC_CHECK_SIZEOF(long, 4)
    AC_CHECK_SIZEOF(long long, 8)
    AC_C_BIGENDIAN

    AST_SIZE_TYPE(8)
    AC_SUBST(INT_8_TYPE)
    AST_SIZE_TYPE(16)
    AC_SUBST(INT_16_TYPE)
    AST_SIZE_TYPE(32)
    AC_SUBST(INT_32_TYPE)
    AST_SIZE_TYPE(64)
    AC_SUBST(INT_64_TYPE)

    AC_C_CONST
    AC_C_INLINE
])
AC_DEFUN([AST_HEADER_CHECKS], [
    AC_HEADER_SYS_WAIT
    AC_CHECK_HEADERS(fcntl.h termios.h sys/ioctl.h sys/select.h sys/time.h \
                     sys/sockio.h sys/byteorder.h malloc.h utmpx.h unistd.h \
                     bsd/signal.h stdarg.h errno.h)
    AC_HEADER_TIME
])
AC_DEFUN([AST_FUNC_CHECKS], [
    AC_TYPE_SIGNAL
    AC_CHECK_FUNCS(memmove putenv strsep memmem usleep snprintf vsnprintf \
                   strcasestr strcasechr strcasepbrk strrev strnlen)
    AC_SEARCH_LIBS(hstrerror, resolv)
    dps_snprintf_oflow()
    dps_vsnprintf_oflow()
    dps_symlink_open_bug()
    dps_rlimit_nproc()
    dps_rlimit_memlock()
])
AC_DEFUN([AST_TYPE_CHECKS], [
    AC_TYPE_MODE_T
    AC_CHECK_TYPE(off_t, long)
    AC_TYPE_PID_T
    AC_TYPE_UID_T
])

dnl#
dnl# LibAST argument macros
dnl#    - arg 1 is the name of the env var to use
dnl#
AC_DEFUN([AST_ARG_DEBUG], [
    AC_MSG_CHECKING(for debugging level)
    AC_ARG_WITH(debugging, [  --with-debugging[=num]    compile in debugging support.  num >= 0], [
                    if test "$withval" = "yes"; then
                        withval=4
                    fi
                    if test "$withval" != "no"; then 
                        AC_MSG_RESULT($withval)
                        AC_DEFINE_UNQUOTED([$1], $withval, [Specify level of debugging to compile in.])
                        $1=$withval
                    else
                        AC_MSG_RESULT(no, disabling all debugging support)
                        AC_DEFINE_UNQUOTED([$1], [0], [Specify level of debugging to compile in.])
                        $1=0
                    fi
                ], [
                    AC_MSG_RESULT(4)
                    AC_DEFINE_UNQUOTED([$1], [4], [Specify level of debugging to compile in.])
                    $1=4
    ])
])
AC_DEFUN([AST_ARG_REGEXP], [
    AC_ARG_WITH(regexp,
    [  --with-regexp[=TYPE]      specify the type of regular expression support (bsd, posix, pcre)],
    [$1=$withval], [$1=yes])
    AST_REGEXP_SUPPORT($1)
    AC_MSG_CHECKING(for regular expression support)
    AC_MSG_RESULT(${$1})
])
AC_DEFUN([AST_ARG_BACKQUOTE_EXEC], [
    AC_MSG_CHECKING(if backquote execution support should be enabled)
    AC_ARG_WITH(backquote-exec,
        [  --without-backquote-exec  disables the execution of commands from inside config files],
        [
            if test "$withval" = "no"; then
                AC_MSG_RESULT(no)
                $1=no
            else
                AC_MSG_RESULT(yes)
                AC_DEFINE($1, [1], [Define for backquote execution.])
                $1=yes
            fi
        ], [
            AC_MSG_RESULT(yes)
            AC_DEFINE($1, [1], [Define for backquote execution.])
            $1=yes
        ])
])

dnl#
dnl# LibAST macro for flag post-processing
dnl#
AC_DEFUN([AST_FLAGS], [
    CPPFLAGS=`eval eval eval eval eval echo "-I$includedir -I$prefix/include $CPPFLAGS"`
    CPPFLAGS=`echo $CPPFLAGS | tr ' ' '\n' | uniq | grep -v NONE | tr '\n' ' '`
    CFLAGS=${CFLAGS--O}
    LDFLAGS=`eval eval eval eval eval echo "-L$libdir -L$prefix/lib ${LDFLAGS--O}"`
    LDFLAGS=`echo $LDFLAGS | tr ' ' '\n' | uniq | grep -v NONE | tr '\n' ' '`
    LIBS="$GRLIBS $X_PRE_LIBS $LIBS $X_EXTRA_LIBS"
])

dnl#
dnl# LibAST macro for final status report
dnl#
AC_DEFUN([AST_STATUS], [
    echo ""
    echo "$PACKAGE $VERSION"
    echo "Configuration:"
    echo "--------------"
    echo ""
    echo "  Source code location:    $srcdir"
    echo "  Host System Type:        $host"
    echo "  Preprocessor:            $CC $CPPFLAGS"
    echo "  Compiler:                $CC $CFLAGS"
    echo "  Linker:                  $CC $LDFLAGS $LIBS"
    echo "  Install path:            $prefix"
    echo ""
    echo "Now type 'make' to build $PACKAGE $VERSION."
    echo ""
])

dnl###########################################################################

dnl#
dnl# acl.m4 -- Written by Duncan Simpson <dps@io.stargate.co.uk>
dnl# Posted to BUGTRAQ on 17 June 1999
dnl# Used by encouragement. :-)
dnl#

dnl Check snprintf for overrun potential
AC_DEFUN([dps_snprintf_oflow], [
    AC_MSG_CHECKING(whether snprintf ignores n)
    AC_CACHE_VAL(dps_cv_snprintf_bug, [
        AC_TRY_RUN(
            changequote(<<, >>)dnl
<<#include <stdio.h>

#ifndef HAVE_SNPRINTF
#include "src/snprintf.c"
#endif /* HAVE_SNPRINTF */

int main(void)
{
char ovbuf[7];
int i;
for (i=0; i<7; i++) ovbuf[i]='x';
snprintf(ovbuf, 4,"foo%s", "bar");
if (ovbuf[5]!='x') exit(1);
snprintf(ovbuf, 4,"foo%d", 666);
if (ovbuf[5]!='x') exit(1);
exit(0);
} >>
            changequote([, ])
        , dps_cv_snprintf_bug=0, dps_cv_snprintf_bug=1, dps_cv_snprintf_bug=2)
    ])
    if test $dps_cv_snprintf_bug -eq 0; then
        AC_MSG_RESULT([no, snprintf is ok])
    elif test $dps_cv_snprint_bug -eq 1; then
        AC_MSG_RESULT([yes, snprintf is broken])
        AC_DEFINE([HAVE_SNPRINTF_BUG], [1], [Defined if libc snprintf is buggy.])
    else
        AC_MSG_RESULT([unknown, assuming yes])
        AC_DEFINE([HAVE_SNPRINTF_BUG], [1], [Defined if libc snprintf is buggy.])
    fi
])

dnl Check vsnprintf for overrun potential
AC_DEFUN([dps_vsnprintf_oflow], [
    AC_MSG_CHECKING(whether vsnprintf ignores n)
    AC_CACHE_VAL(dps_cv_vsnprintf_bug, [
        AC_TRY_RUN(
            changequote(<<, >>)dnl
<<#include <stdio.h>
#include <stdarg.h>

#ifndef HAVE_VSNPRINTF
#include "src/snprintf.c"
#endif /* HAVE_VSNPRINTF */

int prnt(char *s, const char *fmt, ...)
{
  va_list argp;
  va_start(argp, fmt);
  vsnprintf(s, 4, fmt, argp);
  va_end(argp);
}

int main(void)
{
  char ovbuf[8] = "xxxxxxx";
  int i;
  prnt(ovbuf, "foo%s", "bar");
  if (ovbuf[5]!='x') {fprintf(stderr, "buffer:  %s\n", ovbuf); exit(1);}
  prnt(ovbuf, "foo%d", 666);
  if (ovbuf[5]!='x') {fprintf(stderr, "buffer:  %s\n", ovbuf); exit(1);}
  exit(0);
} >>
            changequote([, ])
        , dps_cv_vsnprintf_bug=0, dps_cv_vsnprintf_bug=1, dps_cv_vsnprintf_bug=2)
    ])

    if test $dps_cv_vsnprintf_bug -eq 0; then
        AC_MSG_RESULT([no, vsnprintf is ok])
    elif test $dps_cv_vsnprintf_bug -eq 1; then
        AC_MSG_RESULT([yes, vsnprintf is broken])
        AC_DEFINE([HAVE_VSNPRINTF_BUG], [1], [Defined if libc vsnprintf is buggy.])
    else
        AC_MSG_RESULT([unknown, assuming yes])
        AC_DEFINE([HAVE_VSNPRINTF_BUG], [1], [Defined if libc vsnprintf is buggy.])
    fi
])

dnl open and symlink interaction bug test
AC_DEFUN([dps_symlink_open_bug], [
    AC_MSG_CHECKING(security of interaction between symlink and open)
    AC_CACHE_VAL(dps_cv_symlink_open_bug, [
        mkdir conftest.d
        AC_TRY_RUN(
            changequote(<<, >>)dnl
<<#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_ERRNO_H
#include <errno.h>
#else
extern int errno;
#endif

int main(void)
{
  int fd;
  if (chdir("conftest.d")!=0)
    exit(1);
  if (symlink("foo","bar")!=0)
    exit(1);
  if ((fd=open("bar", O_CREAT | O_EXCL | O_WRONLY, 0700))==0)
  {
        write(fd, "If the symlink was to .rhosts you would be unhappy", 50);
	close(fd);
	exit(1);
  }
  if (errno!=EEXIST)
    exit(1);
  exit(0);
} >>
            changequote([, ])
        ,
        dps_cv_symlink_open_bug=0,
        [
            if test -r conftest.d/foo; then
                dps_cv_symlink_open_bug=2
            else
                dps_cv_symlink_open_bug=1
            fi
        ],
        dps_cv_symlink_open_buf=3)
        rm -rf conftest.d
    ])
    case "$dps_cv_symlink_open_bug" in
        0) AC_MSG_RESULT(secure) ;;
        1) AC_MSG_RESULT(errno wrong but ok)
           AC_DEFINE([HAVE_SYMLINK_OPEN_ERRNO_BUG], [1], [Defined if symlink open() errno is wrong but safe.]) ;;
        2) AC_MSG_RESULT(insecure)
           AC_DEFINE([HAVE_SYMLINK_OPEN_SECURITY_HOLE], [1], [Defined if symlink open() is a security risk.])
           AC_DEFINE([HAVE_SYMLINK_OPEN_ERRNO_BUG], [1], [Defined if symlink open() is buggy.]) ;;
        3) AC_MSG_RESULT(assuming insecure)
           AC_DEFINE([HAVE_SYMLINK_OPEN_SECURITY_HOLE], [1], [Defined if symlink open() is a security risk.])
           AC_DEFINE([HAVE_SYMLINK_OPEN_ERRNO_BUG], [1], [Defined if symlink open() is buggy.]) ;;
        *) AC_MSG_RESULT($dps_cv_symlink_open_bug)
           AC_MSG_ERROR(Impossible value of dps_cv_symlink_open_bug) ;;
    esac
])

dnl Check to RLIMIT_NPROC resource limit
AC_DEFUN([dps_rlimit_nproc], [
    AC_MSG_CHECKING(for working RLIMIT_NPROC resource limit)
    AC_CACHE_VAL(dps_cv_rlimit_nproc, [
        AC_TRY_RUN(
            changequote(<<, >>)dnl
<<
#ifndef HAVE_STDLIB_H
#include <stdlib.h>
#endif /* HAVE_STDLIB_H */
#ifndef HAVE_SIGNAL_H
#include <signal.h>
#endif /* HAVE_SIGNAL_H */
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */
#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif /* HAVE_SYS_RESOURCE_H */

int main(void)
{
#ifdef RLIMIT_NPROC
    static const struct rlimit pid_lim={RLIMIT_NPROC, 1};
    pid_t f;

    signal(SIGCHLD, SIG_IGN);
    setrlimit(RLIMIT_NPROC, (struct rlimit *) &pid_lim);
    if ((f=fork())==0)
	exit(0);
    if (f==-1)
	exit(0); /* The fork() failed (the right thing) */
#endif
   exit(1);
} >>
            changequote([, ])
        , dps_cv_rlimit_nproc=0, dps_cv_rlimit_nproc=1, dps_cv_rlimit_nproc=2)
    ])
    if test $dps_cv_rlimit_nproc -eq 0; then
        AC_MSG_RESULT([yes])
        AC_DEFINE([HAVE_RLIMIT_NPROC], [1], [Defined if the RLIMIT_NPROC resource limit works.])
    elif test $dps_cv_rlimit_nproc -eq 1; then
        AC_MSG_RESULT([no])
    else
        AC_MSG_RESULT([unknown, assuming none])
    fi
])

dnl Check to RLIMIT_MEMLOCK resource limit
AC_DEFUN([dps_rlimit_memlock], [
    AC_MSG_CHECKING(for working RLIMIT_MEMLOCK resource limit)
    AC_CACHE_VAL(dps_cv_rlimit_memlock, [
        AC_TRY_RUN(
            changequote(<<, >>)dnl
<<
#ifndef HAVE_STDLIB_H
#include <stdlib.h>
#endif /* HAVE_STDLIB_H */
#ifndef HAVE_SIGNAL_H
#include <signal.h>
#endif /* HAVE_SIGNAL_H */
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */
#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif /* HAVE_SYS_RESOURCE_H */
#ifdef HAVE_SYS_MMAN
#include <sys/mman.h>
#endif /* HAVE_SYS_MMAN */
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif /* HAVE_ERRNO_H */

int main(void)
{
#ifdef RLIMIT_MEMLOCK
    static const struct rlimit mlock_lim={RLIMIT_MEMLOCK, 0};
    void *memory;

    if (setrlimit(RLIMIT_MEMLOCK, (struct rlimit *) &mlock_lim)!=-1)
	exit(0);
#endif
exit(1);
} >>
            changequote([, ])
        , dps_cv_rlimit_memlock=0, dps_cv_rlimit_memlock=1, dps_cv_rlimit_memlock=2)
    ])
    if test $dps_cv_rlimit_memlock -eq 0; then
        AC_MSG_RESULT([yes])
        AC_DEFINE([HAVE_RLIMIT_MEMLOCK], [1], [Defined if the RLIMIT_MEMLOCK resource limit works.])
    elif test $dps_cv_rlimit_memlock -eq 1; then
        AC_MSG_RESULT([no])
    else
        AC_MSG_RESULT([unknown, assuming none])
    fi
])

dnl# Taken from http://autoconf-archive.cryp.to/acx_pthread.html
AC_DEFUN([AST_PTHREADS], [
AC_REQUIRE([AC_CANONICAL_HOST])
AC_LANG_SAVE
AC_LANG_C
ast_pthreads_ok=no

# We used to check for pthread.h first, but this fails if pthread.h
# requires special compiler flags (e.g. on True64 or Sequent).
# It gets checked for in the link test anyway.

# First of all, check if the user has set any of the PTHREAD_LIBS,
# etcetera environment variables, and if threads linking works using
# them:
if test x"$PTHREAD_LIBS$PTHREAD_CFLAGS" != x; then
        save_CFLAGS="$CFLAGS"
        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
        save_LIBS="$LIBS"
        LIBS="$PTHREAD_LIBS $LIBS"
        AC_MSG_CHECKING([for pthread_join in LIBS=$PTHREAD_LIBS with CFLAGS=$PTHREAD_CFLAGS])
        AC_TRY_LINK_FUNC(pthread_join, ast_pthreads_ok=yes)
        AC_MSG_RESULT($ast_pthreads_ok)
        if test x"$ast_pthreads_ok" = xno; then
                PTHREAD_LIBS=""
                PTHREAD_CFLAGS=""
        fi
        LIBS="$save_LIBS"
        CFLAGS="$save_CFLAGS"
fi

# We must check for the threads library under a number of different
# names; the ordering is very important because some systems
# (e.g. DEC) have both -lpthread and -lpthreads, where one of the
# libraries is broken (non-POSIX).

# Create a list of thread flags to try.  Items starting with a "-" are
# C compiler flags, and other items are library names, except for "none"
# which indicates that we try without any flags at all, and "pthread-config"
# which is a program returning the flags for the Pth emulation library.

ast_pthreads_flags="pthreads none -Kthread -kthread lthread -pthread -pthreads -mthreads pthread --thread-safe -mt pthread-config"

# The ordering *is* (sometimes) important.  Some notes on the
# individual items follow:

# pthreads: AIX (must check this before -lpthread)
# none: in case threads are in libc; should be tried before -Kthread and
#       other compiler flags to prevent continual compiler warnings
# -Kthread: Sequent (threads in libc, but -Kthread needed for pthread.h)
# -kthread: FreeBSD kernel threads (preferred to -pthread since SMP-able)
# lthread: LinuxThreads port on FreeBSD (also preferred to -pthread)
# -pthread: Linux/gcc (kernel threads), BSD/gcc (userland threads)
# -pthreads: Solaris/gcc
# -mthreads: Mingw32/gcc, Lynx/gcc
# -mt: Sun Workshop C (may only link SunOS threads [-lthread], but it
#      doesn't hurt to check since this sometimes defines pthreads too;
#      also defines -D_REENTRANT)
#      ... -mt is also the pthreads flag for HP/aCC
# pthread: Linux, etcetera
# --thread-safe: KAI C++
# pthread-config: use pthread-config program (for GNU Pth library)

case "${host_cpu}-${host_os}" in
        *solaris*)

        # On Solaris (at least, for some versions), libc contains stubbed
        # (non-functional) versions of the pthreads routines, so link-based
        # tests will erroneously succeed.  (We need to link with -pthreads/-mt/
        # -lpthread.)  (The stubs are missing pthread_cleanup_push, or rather
        # a function called by this macro, so we could check for that, but
        # who knows whether they'll stub that too in a future libc.)  So,
        # we'll just look for -pthreads and -lpthread first:

        ast_pthreads_flags="-pthreads pthread -mt -pthread $ast_pthreads_flags"
        ;;
esac

if test x"$ast_pthreads_ok" = xno; then
for flag in $ast_pthreads_flags; do

        case $flag in
                none)
                AC_MSG_CHECKING([whether pthreads work without any flags])
                ;;

                -*)
                AC_MSG_CHECKING([whether pthreads work with $flag])
                PTHREAD_CFLAGS="$flag"
                ;;

                pthread-config)
                AC_CHECK_PROG(ast_pthreads_config, pthread-config, yes, no)
                if test x"$ast_pthreads_config" = xno; then continue; fi
                PTHREAD_CFLAGS="`pthread-config --cflags`"
                PTHREAD_LIBS="`pthread-config --ldflags` `pthread-config --libs`"
                ;;

                *)
                AC_MSG_CHECKING([for the pthreads library -l$flag])
                PTHREAD_LIBS="-l$flag"
                ;;
        esac

        save_LIBS="$LIBS"
        save_CFLAGS="$CFLAGS"
        LIBS="$PTHREAD_LIBS $LIBS"
        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"

        # Check for various functions.  We must include pthread.h,
        # since some functions may be macros.  (On the Sequent, we
        # need a special flag -Kthread to make this header compile.)
        # We check for pthread_join because it is in -lpthread on IRIX
        # while pthread_create is in libc.  We check for pthread_attr_init
        # due to DEC craziness with -lpthreads.  We check for
        # pthread_cleanup_push because it is one of the few pthread
        # functions on Solaris that doesn't have a non-functional libc stub.
        # We try pthread_create on general principles.
        AC_TRY_LINK([#include <pthread.h>],
                    [pthread_t th; pthread_join(th, 0);
                     pthread_attr_init(0); pthread_cleanup_push(0, 0);
                     pthread_create(0,0,0,0); pthread_cleanup_pop(0); ],
                    [ast_pthreads_ok=yes])

        LIBS="$save_LIBS"
        CFLAGS="$save_CFLAGS"

        AC_MSG_RESULT($ast_pthreads_ok)
        if test "x$ast_pthreads_ok" = xyes; then
                break;
        fi

        PTHREAD_LIBS=""
        PTHREAD_CFLAGS=""
done
fi

# Various other checks:
if test "x$ast_pthreads_ok" = xyes; then
        save_LIBS="$LIBS"
        LIBS="$PTHREAD_LIBS $LIBS"
        save_CFLAGS="$CFLAGS"
        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"

        # Detect AIX lossage: JOINABLE attribute is called UNDETACHED.
        AC_MSG_CHECKING([for joinable pthread attribute])
        attr_name=unknown
        for attr in PTHREAD_CREATE_JOINABLE PTHREAD_CREATE_UNDETACHED; do
            AC_TRY_LINK([#include <pthread.h>], [int attr=$attr; return attr;],
                        [attr_name=$attr; break])
        done
        AC_MSG_RESULT($attr_name)
        if test "$attr_name" != PTHREAD_CREATE_JOINABLE; then
            AC_DEFINE_UNQUOTED(PTHREAD_CREATE_JOINABLE, $attr_name,
                               [Define to necessary symbol if this constant
                                uses a non-standard name on your system.])
        fi

        AC_MSG_CHECKING([if more special flags are required for pthreads])
        flag=no
        case "${host_cpu}-${host_os}" in
            *-aix* | *-freebsd* | *-darwin*) flag="-D_THREAD_SAFE";;
            *solaris* | *-osf* | *-hpux*) flag="-D_REENTRANT";;
        esac
        AC_MSG_RESULT(${flag})
        if test "x$flag" != xno; then
            PTHREAD_CFLAGS="$flag $PTHREAD_CFLAGS"
        fi

        LIBS="$save_LIBS"
        CFLAGS="$save_CFLAGS"

        # More AIX lossage: must compile with cc_r
        AC_CHECK_PROG(PTHREAD_CC, cc_r, cc_r, ${CC})
else
        PTHREAD_CC="$CC"
fi

AC_SUBST(PTHREAD_LIBS)
AC_SUBST(PTHREAD_CFLAGS)
AC_SUBST(PTHREAD_CC)

# Finally, execute ACTION-IF-FOUND/ACTION-IF-NOT-FOUND:
if test x"$ast_pthreads_ok" = xyes; then
        ifelse([$1],,AC_DEFINE(HAVE_PTHREADS,1,[Define if you have POSIX threads libraries and header files.]),[$1])
        :
else
        ast_pthreads_ok=no
        $2
fi
AC_LANG_RESTORE
])dnl AST_PTHREADS

# Taken from http://cool.haxx.se/cvs.cgi/curl/docs/libcurl/libcurl.m4?rev=1.5
ifdef([LIBCURL_CHECK_CONFIG], [], [
AC_DEFUN([LIBCURL_CHECK_CONFIG],
[
  # LIBCURL_CHECK_CONFIG ([DEFAULT-ACTION], [MINIMUM-VERSION],
  #                       [ACTION-IF-YES], [ACTION-IF-NO])
  # ----------------------------------------------------------
  #      David Shaw <dshaw@jabberwocky.com>   May-09-2006
  #
  # Checks for libcurl.  DEFAULT-ACTION is the string yes or no to
  # specify whether to default to --with-libcurl or --without-libcurl.
  # If not supplied, DEFAULT-ACTION is yes.  MINIMUM-VERSION is the
  # minimum version of libcurl to accept.  Pass the version as a regular
  # version number like 7.10.1. If not supplied, any version is
  # accepted.  ACTION-IF-YES is a list of shell commands to run if
  # libcurl was successfully found and passed the various tests.
  # ACTION-IF-NO is a list of shell commands that are run otherwise.
  # Note that using --without-libcurl does run ACTION-IF-NO.
  #
  # This macro #defines HAVE_LIBCURL if a working libcurl setup is
  # found, and sets @LIBCURL@ and @LIBCURL_CPPFLAGS@ to the necessary
  # values.  Other useful defines are LIBCURL_FEATURE_xxx where xxx are
  # the various features supported by libcurl, and LIBCURL_PROTOCOL_yyy
  # where yyy are the various protocols supported by libcurl.  Both xxx
  # and yyy are capitalized.  See the list of AH_TEMPLATEs at the top of
  # the macro for the complete list of possible defines.  Shell
  # variables $libcurl_feature_xxx and $libcurl_protocol_yyy are also
  # defined to 'yes' for those features and protocols that were found.
  # Note that xxx and yyy keep the same capitalization as in the
  # curl-config list (e.g. it's "HTTP" and not "http").
  #
  # Users may override the detected values by doing something like:
  # LIBCURL="-lcurl" LIBCURL_CPPFLAGS="-I/usr/myinclude" ./configure
  #
  # For the sake of sanity, this macro assumes that any libcurl that is
  # found is after version 7.7.2, the first version that included the
  # curl-config script.  Note that it is very important for people
  # packaging binary versions of libcurl to include this script!
  # Without curl-config, we can only guess what protocols are available,
  # or use curl_version_info to figure it out at runtime.

  AH_TEMPLATE([LIBCURL_FEATURE_SSL],[Defined if libcurl supports SSL])
  AH_TEMPLATE([LIBCURL_FEATURE_KRB4],[Defined if libcurl supports KRB4])
  AH_TEMPLATE([LIBCURL_FEATURE_IPV6],[Defined if libcurl supports IPv6])
  AH_TEMPLATE([LIBCURL_FEATURE_LIBZ],[Defined if libcurl supports libz])
  AH_TEMPLATE([LIBCURL_FEATURE_ASYNCHDNS],[Defined if libcurl supports AsynchDNS])
  AH_TEMPLATE([LIBCURL_FEATURE_IDN],[Defined if libcurl supports IDN])
  AH_TEMPLATE([LIBCURL_FEATURE_SSPI],[Defined if libcurl supports SSPI])
  AH_TEMPLATE([LIBCURL_FEATURE_NTLM],[Defined if libcurl supports NTLM])

  AH_TEMPLATE([LIBCURL_PROTOCOL_HTTP],[Defined if libcurl supports HTTP])
  AH_TEMPLATE([LIBCURL_PROTOCOL_HTTPS],[Defined if libcurl supports HTTPS])
  AH_TEMPLATE([LIBCURL_PROTOCOL_FTP],[Defined if libcurl supports FTP])
  AH_TEMPLATE([LIBCURL_PROTOCOL_FTPS],[Defined if libcurl supports FTPS])
  AH_TEMPLATE([LIBCURL_PROTOCOL_FILE],[Defined if libcurl supports FILE])
  AH_TEMPLATE([LIBCURL_PROTOCOL_TELNET],[Defined if libcurl supports TELNET])
  AH_TEMPLATE([LIBCURL_PROTOCOL_LDAP],[Defined if libcurl supports LDAP])
  AH_TEMPLATE([LIBCURL_PROTOCOL_DICT],[Defined if libcurl supports DICT])
  AH_TEMPLATE([LIBCURL_PROTOCOL_TFTP],[Defined if libcurl supports TFTP])

  AC_ARG_WITH(libcurl,
     AC_HELP_STRING([--with-libcurl=DIR],[look for the curl library in DIR]),
     [_libcurl_with=$withval],[_libcurl_with=ifelse([$1],,[yes],[$1])])

  if test "$_libcurl_with" != "no" ; then

     AC_PROG_AWK

     _libcurl_version_parse="eval $AWK '{split(\$NF,A,\".\"); X=256*256*A[[1]]+256*A[[2]]+A[[3]]; print X;}'"

     _libcurl_try_link=yes

     if test -d "$_libcurl_with" ; then
        LIBCURL_CPPFLAGS="-I$withval/include"
        _libcurl_ldflags="-L$withval/lib"
        AC_PATH_PROG([_libcurl_config],["$withval/bin/curl-config"])
     else
	AC_PATH_PROG([_libcurl_config],[curl-config])
     fi

     if test x$_libcurl_config != "x" ; then
        AC_CACHE_CHECK([for the version of libcurl],
	   [libcurl_cv_lib_curl_version],
           [libcurl_cv_lib_curl_version=`$_libcurl_config --version | $AWK '{print $[]2}'`])

	_libcurl_version=`echo $libcurl_cv_lib_curl_version | $_libcurl_version_parse`
	_libcurl_wanted=`echo ifelse([$2],,[0],[$2]) | $_libcurl_version_parse`

        if test $_libcurl_wanted -gt 0 ; then
	   AC_CACHE_CHECK([for libcurl >= version $2],
	      [libcurl_cv_lib_version_ok],
              [
   	      if test $_libcurl_version -ge $_libcurl_wanted ; then
	         libcurl_cv_lib_version_ok=yes
      	      else
	         libcurl_cv_lib_version_ok=no
  	      fi
              ])
        fi

	if test $_libcurl_wanted -eq 0 || test x$libcurl_cv_lib_version_ok = xyes ; then
           if test x"$LIBCURL_CPPFLAGS" = "x" ; then
              LIBCURL_CPPFLAGS=`$_libcurl_config --cflags`
           fi
           if test x"$LIBCURL" = "x" ; then
              LIBCURL=`$_libcurl_config --libs`

              # This is so silly, but Apple actually has a bug in their
	      # curl-config script.  Fixed in Tiger, but there are still
	      # lots of Panther installs around.
              case "${host}" in
                 powerpc-apple-darwin7*)
                    LIBCURL=`echo $LIBCURL | sed -e 's|-arch i386||g'`
                 ;;
              esac
           fi

	   # All curl-config scripts support --feature
	   _libcurl_features=`$_libcurl_config --feature`

           # Is it modern enough to have --protocols? (7.12.4)
	   if test $_libcurl_version -ge 461828 ; then
              _libcurl_protocols=`$_libcurl_config --protocols`
           fi
	else
           _libcurl_try_link=no
	fi

	unset _libcurl_wanted
     fi

     if test $_libcurl_try_link = yes ; then

        # we didn't find curl-config, so let's see if the user-supplied
        # link line (or failing that, "-lcurl") is enough.
        LIBCURL=${LIBCURL-"$_libcurl_ldflags -lcurl"}

        AC_CACHE_CHECK([whether libcurl is usable],
           [libcurl_cv_lib_curl_usable],
           [
           _libcurl_save_cppflags=$CPPFLAGS
           CPPFLAGS="$LIBCURL_CPPFLAGS $CPPFLAGS"
           _libcurl_save_libs=$LIBS
           LIBS="$LIBCURL $LIBS"

           AC_LINK_IFELSE(AC_LANG_PROGRAM([#include <curl/curl.h>],[
/* Try and use a few common options to force a failure if we are
   missing symbols or can't link. */
int x;
curl_easy_setopt(NULL,CURLOPT_URL,NULL);
x=CURL_ERROR_SIZE;
x=CURLOPT_WRITEFUNCTION;
x=CURLOPT_FILE;
x=CURLOPT_ERRORBUFFER;
x=CURLOPT_STDERR;
x=CURLOPT_VERBOSE;
]),libcurl_cv_lib_curl_usable=yes,libcurl_cv_lib_curl_usable=no)

           CPPFLAGS=$_libcurl_save_cppflags
           LIBS=$_libcurl_save_libs
           unset _libcurl_save_cppflags
           unset _libcurl_save_libs
           ])

        if test $libcurl_cv_lib_curl_usable = yes ; then

	   # Does curl_free() exist in this version of libcurl?
	   # If not, fake it with free()

           _libcurl_save_cppflags=$CPPFLAGS
           CPPFLAGS="$CPPFLAGS $LIBCURL_CPPFLAGS"
           _libcurl_save_libs=$LIBS
           LIBS="$LIBS $LIBCURL"

           AC_CHECK_FUNC(curl_free,,
  	      AC_DEFINE(curl_free,free,
		[Define curl_free() as free() if our version of curl lacks curl_free.]))

           CPPFLAGS=$_libcurl_save_cppflags
           LIBS=$_libcurl_save_libs
           unset _libcurl_save_cppflags
           unset _libcurl_save_libs

           AC_DEFINE(HAVE_LIBCURL,1,
             [Define to 1 if you have a functional curl library.])
           AC_SUBST(LIBCURL_CPPFLAGS)
           AC_SUBST(LIBCURL)

           for _libcurl_feature in $_libcurl_features ; do
	      AC_DEFINE_UNQUOTED(AS_TR_CPP(libcurl_feature_$_libcurl_feature),[1])
	      eval AS_TR_SH(libcurl_feature_$_libcurl_feature)=yes
           done

	   if test "x$_libcurl_protocols" = "x" ; then

	      # We don't have --protocols, so just assume that all
	      # protocols are available
	      _libcurl_protocols="HTTP FTP FILE TELNET LDAP DICT"

	      if test x$libcurl_feature_SSL = xyes ; then
	         _libcurl_protocols="$_libcurl_protocols HTTPS"

		 # FTPS wasn't standards-compliant until version
		 # 7.11.0
		 if test $_libcurl_version -ge 461568; then
		    _libcurl_protocols="$_libcurl_protocols FTPS"
		 fi
	      fi
	   fi

	   for _libcurl_protocol in $_libcurl_protocols ; do
	      AC_DEFINE_UNQUOTED(AS_TR_CPP(libcurl_protocol_$_libcurl_protocol),[1])
	      eval AS_TR_SH(libcurl_protocol_$_libcurl_protocol)=yes
           done
	else
	   unset LIBCURL
	   unset LIBCURL_CPPFLAGS
        fi
     fi

     unset _libcurl_try_link
     unset _libcurl_version_parse
     unset _libcurl_config
     unset _libcurl_feature
     unset _libcurl_features
     unset _libcurl_protocol
     unset _libcurl_protocols
     unset _libcurl_version
     unset _libcurl_ldflags
  fi

  if test x$_libcurl_with = xno || test x$libcurl_cv_lib_curl_usable != xyes ; then
     # This is the IF-NO path
     ifelse([$4],,:,[$4])
  else
     # This is the IF-YES path
     ifelse([$3],,:,[$3])
  fi

  unset _libcurl_with
])])dnl
