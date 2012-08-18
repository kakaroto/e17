
dnl use: EYESIGHT_CHECK_DEP_IMG(want_static[, ACTION-IF-FOUND[, ACTION-IF-NOT-FOUND]])

AC_DEFUN([EYESIGHT_CHECK_DEP_IMG],
[

requirement=""

PKG_CHECK_MODULES([ECORE_FILE],
   [ecore-file],
   [
    have_dep="yes"
    requirement="ecore-file"
   ],
   [have_dep="no"])

if test "x$1" = "xstatic" ; then
   requirements_pc_eyesight="${requirement} ${requirements_pc_eyesight}"
fi


AS_IF([test "x$have_dep" = "xyes"], [$2], [$3])

])

dnl use: EYESIGHT_CHECK_DEP_PDF(want_static[, ACTION-IF-FOUND[, ACTION-IF-NOT-FOUND]])

AC_DEFUN([EYESIGHT_CHECK_DEP_PDF],
[

requirements_pc=""
requirements_libs=""

MUPDF_CFLAGS=""
MUPDF_LIBS=""

have_dep="yes"

dnl zlib
if test "x${have_dep}" = "xyes" ; then
   PKG_CHECK_EXISTS([zlib >= 1.2.5],
      [
       have_pkg_zlib="yes"
       requirements_pc="zlib ${requirements_pc}"
      ],
      [have_pkg_zlib="no"])

   if test "x${have_pkg_zlib}" = "xno" ; then
      AC_MSG_NOTICE([no pkg-config file for zlib, checking files individually])
      AC_CHECK_HEADER([zlib.h], [have_dep="yes"], [have_dep="no"])
      if test "x${have_dep}" = "xyes" ; then
         AC_CHECK_LIB([z], [zlibVersion],
            [
             have_dep="yes"
             requirements_libs="${requirements_libs} -lz"
            ],
            [have_dep="no"])
      fi
   fi
fi

dnl freetype
if test "x${have_dep}" = "xyes" ; then
   PKG_CHECK_EXISTS([freetype2],
      [requirements_pc="freetype2 ${requirements_pc}"],
      [have_dep="no"])
fi

dnl openjpeg
if test "x${have_dep}" = "xyes" ; then
   PKG_CHECK_EXISTS([libopenjpeg1 >= 1.5],
      [
       have_pkg_jp2k="yes"
       requirements_pc="libopenjpeg1 ${requirements_pc}"
      ],
      [have_pkg_jp2k="no"])

   if test "x${have_pkg_jp2k}" = "xno" ; then
      PKG_CHECK_EXISTS([libopenjpeg >= 1.5],
         [
          have_pkg_jp2k="yes"
          requirements_pc="libopenjpeg ${requirements_pc}"
         ],
         [have_pkg_jp2k="no"])
   fi

   if test "x${have_pkg_jp2k}" = "xno" ; then
      AC_MSG_NOTICE([no pkg-config file for openjpeg, checking files individually])
      AC_MSG_CHECKING([for openjpeg library])
      LIBS_save="${LIBS}"
      LIBS="${LIBS} -lopenjpeg"
      AC_LINK_IFELSE(
         [AC_LANG_PROGRAM(
             [[
#include <openjpeg.h>
             ]],
             [[
opj_dparameters_t params;

opj_set_default_decoder_parameters(&params);
params.flags |= OPJ_DPARAMETERS_IGNORE_PCLR_CMAP_CDEF_FLAG;
             ]])],
         [
          have_dep="yes"
          requirements_libs="-lopenjpeg ${requirements_libs}"
         ],
         [have_dep="no"])
      LIBS="${LIBS_save}"

      AC_MSG_RESULT([${have_dep}])
   fi
fi

dnl jbig2dec
if test "x${have_dep}" = "xyes" ; then
   AC_CHECK_HEADER([jbig2.h], [have_dep="yes"], [have_dep="no"])
   if test "x${have_dep}" = "xyes" ; then
      AC_CHECK_LIB([jbig2dec], [jbig2_ctx_new],
         [
          have_dep="yes"
          requirements_libs="-ljbig2dec ${requirements_libs}"
         ],
         [have_dep="no"])
   fi
fi

dnl check libraries
if test "x${have_dep}" = "xyes" ; then
   PKG_CHECK_MODULES([MUPDF],
         [${requirements_pc}],
         [
          have_dep="yes"
          MUPDF_LIBS="${requirements_libs} ${MUPDF_LIBS}"
         ],
         [have_dep="no"])

dnl CJK fonts
   if ! test "x${want_mupdf_cjk}" = "xyes" ; then
      BUILD_MUPDF_CJK_FONTS="-DNOCJK"
   fi
fi

AC_ARG_VAR([MUPDF_CFLAGS], [preprocessor flags for mupdf])
AC_SUBST([MUPDF_CFLAGS])
AC_ARG_VAR([MUPDF_LIBS], [linker flags for mupdf])
AC_SUBST([MUPDF_LIBS])
AC_SUBST([BUILD_MUPDF_CJK_FONTS])

AM_CONDITIONAL([BUILD_MUPDF_CJK_FONTS], [test "x${want_mupdf_cjk}" = "xyes"])

if test "x$1" = "xstatic" ; then
   requirements_pc_eyesight="${requirements_pc} ${requirements_pc_eyesight}"
   requirements_libs_eyesight="${requirements_libs} ${requirements_libs_eyesight}"
fi

AS_IF([test "x$have_dep" = "xyes"], [$2], [$3])

])

dnl use: EYESIGHT_CHECK_DEP_POSTSCRIPT(want_static[, ACTION-IF-FOUND[, ACTION-IF-NOT-FOUND]])

AC_DEFUN([EYESIGHT_CHECK_DEP_POSTSCRIPT],
[

requirement=""

PKG_CHECK_MODULES([POSTSCRIPT],
   [libspectre],
   [
    have_dep="yes"
    requirement="libspectre"
   ],
   [have_dep="no"])

if test "x$1" = "xstatic" ; then
   requirements_pc_eyesight="${requirement} ${requirements_pc_eyesight}"
fi

AS_IF([test "x$have_dep" = "xyes"], [$2], [$3])

])

dnl use: EYESIGHT_CHECK_DEP_TXT(want_static[, ACTION-IF-FOUND[, ACTION-IF-NOT-FOUND]])

AC_DEFUN([EYESIGHT_CHECK_DEP_TXT],
[
have_dep="yes"

AS_IF([test "x$have_dep" = "xyes"], [$2], [$3])

])

dnl use: EYESIGHT_CHECK_MODULE(description, want_module[, ACTION-IF-FOUND[, ACTION-IF-NOT-FOUND]])
AC_DEFUN([EYESIGHT_CHECK_MODULE],
[
m4_pushdef([UP], m4_translit([$1], [-a-z], [_A-Z]))dnl
m4_pushdef([DOWN], m4_translit([$1], [-A-Z], [_a-z]))dnl

want_module="$2"

AC_ARG_ENABLE([DOWN],
   [AC_HELP_STRING([--enable-]DOWN, [enable build of $1 module @<:@default=yes@:>@])],
   [
    if test "x${enableval}" = "xyes" ; then
       enable_module="strict"
    else
       if test "x${enableval}" = "xstatic" ; then
          enable_module="static"
       else
          enable_module="no"
       fi
    fi
   ],
   [enable_module="yes"])

if test "x${enable_module}" = "xyes" || test "x${enable_module}" = "xstrict" || test "x${enable_module}" = "xstatic" ; then
   want_module="yes"
fi

have_module="no"
if test "x${want_module}" = "xyes" && (test "x${enable_module}" = "xyes" || test "x${enable_module}" = "xstatic") ; then
   m4_default([EYESIGHT_CHECK_DEP_]m4_defn([UP]))(${enable_module}, [have_module="yes"], [have_module="no"])
fi

AC_MSG_CHECKING([whether to enable $1 module built])
AC_MSG_RESULT([${have_module}])

if test "x${have_module}" = "xno" && test "x${enable_module}" = "xstrict" ; then
   AC_MSG_ERROR([$1 module requested, but dependency checks failed. See config.log for more details. Exiting...])
fi

static_module="no"
if test "x${have_module}" = "xyes" && test "x${enable_module}" = "xstatic" ; then
   static_module="yes"
fi

AM_CONDITIONAL(EYESIGHT_BUILD_[]UP, [test "x${have_module}" = "xyes"])
AM_CONDITIONAL(EYESIGHT_STATIC_BUILD_[]UP, [test "x${static_module}" = "xyes"])

if test "x${static_module}" = "xyes" ; then
   AC_DEFINE(EYESIGHT_STATIC_BUILD_[]UP, 1, [Set to 1 if $1 is statically built])
   have_static_module="yes"
fi

enable_[]DOWN="no"
if test "x${have_module}" = "xyes" ; then
   enable_[]DOWN=${enable_module}
   AC_DEFINE(EYESIGHT_BUILD_[]UP, 1, [Set to 1 if $1 is built])
fi

AS_IF([test "x$have_module" = "xyes"], [$3], [$4])

m4_popdef([UP])
m4_popdef([DOWN])
])
