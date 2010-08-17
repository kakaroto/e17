# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT(image-viewer, 0.1.0.0, enlightenment-devel@lists.sourceforge.net)
AC_PREREQ(2.52)
AC_CONFIG_SRCDIR(configure.ac)
AC_CANONICAL_BUILD
AC_CANONICAL_HOST

AM_INIT_AUTOMAKE(1.6 dist-bzip2)
AM_CONFIG_HEADER(config.h)

AC_C_BIGENDIAN
AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_C_CONST

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# Find edje_cc
PKG_CHECK_MODULES(EDJE, [edje >= 0.5.0])
AC_ARG_WITH(edje-cc,
  AC_HELP_STRING([--with-edje-cc=PATH], [specify a specific path to edje_cc]),
  [
    v=$withval;
    EDJE_CC=$v
  ],[
    EDJE_CC=$(pkg-config --variable=prefix edje)/bin/edje_cc
  ]
)
AC_SUBST(EDJE_CC)
AC_MSG_CHECKING([Which edje_cc to use])
AC_MSG_RESULT(${EDJE_CC})

PKG_CHECK_MODULES([ELEMENTARY], [
                   eina
                   eet
                   ecore
                   ecore-file
                   ecore-x
                   efreet
                   efreet-trash
                   elementary
])

AC_ARG_ENABLE([experimental],
   [AC_HELP_STRING([--enable-experimental], [enable experimental code @<:@default=no@:>@])],
   [
    if test "x${enableval}" = "xyes" ; then
       _iv_enable_experimental="yes"
    else
       _iv_enable_experimental="no"
    fi
   ],
   [_iv_enable_experimental="no"]
)

AC_MSG_CHECKING([whether experimental code is built])
AC_MSG_RESULT([${_iv_enable_experimental}])

have_experimental="no"

if test "x${_iv_enable_experimental}" = "xyes" ; then
    have_experimental="yes"
    AC_DEFINE(HAVE_EXPERIMENTAL, 1, [have experimental code support])
fi

AC_ARG_ENABLE([ethumb],
   [AC_HELP_STRING([--enable-ethumb], [enable ethumb @<:@default=yes@:>@])],
   [
    if test "x${enableval}" = "xyes" ; then
       _iv_enable_ethumb="yes"
    else
       _iv_enable_ethumb="no"
    fi
   ],
   [_iv_enable_ethumb="yes"]
)

AC_MSG_CHECKING([whether ethumb is built])
AC_MSG_RESULT([${_iv_enable_ethumb}])

have_ethumb="no"

if test "x${_iv_enable_ethumb}" = "xyes" ; then
    PKG_CHECK_MODULES(ETHUMB, ethumb_client, [
        have_ethumb="yes"
        AC_DEFINE(HAVE_ETHUMB, 1, [have ethumb support])
    ],[
       have_ethumb="no"
    ])
fi

AM_CONDITIONAL(BUILD_ETHUMB, test $have_ethumb = yes)

AC_ARG_ENABLE([libexif],
   [AC_HELP_STRING([--enable-libexif], [enable libexif support @<:@default=yes@:>@])],
   [
    if test "x${enableval}" = "xyes" ; then
       _iv_enable_libexif="yes"
    else
       _iv_enable_libexif="no"
    fi
   ],
   [_iv_enable_libexif="yes"]
)

AC_MSG_CHECKING([whether ethumb is built])
AC_MSG_RESULT([${_iv_enable_libexif}])

have_libexif="no"

if test "x${_iv_enable_libexif}" = "xyes" ; then
    PKG_CHECK_MODULES(EXIF, libexif, [
        have_libexif="yes"
        AC_DEFINE(HAVE_LIBEXIF, 1, [have libexif support])
    ],[
       have_libexif="no"
    ])
fi

AM_CONDITIONAL(BUILD_LIBEXIF, test $have_libexif = yes)

AC_OUTPUT([
Makefile
src/Makefile
src/bin/Makefile
data/Makefile
data/desktop/Makefile
data/theme/Makefile
])

echo
echo
echo
echo "------------------------------------------------------------------------"
echo "$PACKAGE $VERSION"
echo "------------------------------------------------------------------------"
echo
echo "Configuration Options Summary:"
echo
echo "  Build with experimental code support....: $have_experimental"
echo "  Build with EThumb support...............: $have_ethumb"
echo "  Build with EXIF support.................: $have_libexif"
echo
echo "------------------------------------------------------------------------"
echo
echo "Now type 'make' ('gmake' on some systems) to compile $PACKAGE,"
echo "and then afterwards as root (or the user who will install this), type"
echo "'make install'. Change users with 'su' or 'sudo' appropriately."
echo
