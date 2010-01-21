# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT(configure.ac)
AC_CANONICAL_TARGET
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE(eyelight, 0.5)
AM_CONFIG_HEADER(config.h)

AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_C_CONST
AM_PROG_CC_C_O

AM_PROG_LIBTOOL

requirements="ecore ecore-evas ecore-file evas edje"


AC_SUBST(version_info)
AC_SUBST(requirements)
AC_SUBST(packagesrcdir)

PKG_PROG_PKG_CONFIG

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

# doxygen program for documentation building

EFL_CHECK_DOXYGEN([build_doc="yes"], [build_doc="no"])

PKG_CHECK_MODULES(EYELIGHT, [
                        ecore >= 0.9.9.037,
                        ecore-evas >= 0.9.9.038,
                        ecore-file >= 0.9.9,
                        evas >= 0.9.9.40,
                        edje >= 0.5
                        emotion >= 0.1
                        libpng >= 0.1
                        ])
# required libs / cflags
eyelight_libs=$EYELIGHT_LIBS
eyelight_cflags=$EYELIGHT_CFLAGS
AC_SUBST(eyelight_libs)
AC_SUBST(eyelight_cflags)

AC_SUBST(EDJE_CC)
AC_MSG_CHECKING([Which edje_cc to use])
AC_MSG_RESULT(${EDJE_CC})


AC_OUTPUT([
Makefile
eyelight.pc
doc/Makefile
src/Makefile
src/lib/Makefile
src/bin/Makefile
data/Makefile
data/viewer/Makefile
data/themes/Makefile
data/themes/default/Makefile
data/themes/default/fonts/Makefile
data/themes/default/transition/Makefile
])


echo
echo
echo "--------------------------------------------------------"
echo "$PACKAGE $VERSION"
echo "--------------------------------------------------------"
echo
echo
echo "Configuration Options Summary:"
echo
echo "  Installation Path...................: $prefix"
echo "  Documentation.......................: ${build_doc}"
echo
echo "Now type 'make' ('gmake' on some systems) to compile $PACKAGE,"
echo "and then afterwards as root (or the user who will install this), type"
echo "'make install'. Change users with 'su' or 'sudo' appropriately."
echo
