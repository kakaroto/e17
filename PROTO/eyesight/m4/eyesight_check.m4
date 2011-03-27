
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
   requirement_eyesight="${requirement} ${requirement_eyesight}"
fi


AS_IF([test "x$have_dep" = "xyes"], [$2], [$3])

])

dnl use: EYESIGHT_CHECK_DEP_PDF(want_static[, ACTION-IF-FOUND[, ACTION-IF-NOT-FOUND]])

AC_DEFUN([EYESIGHT_CHECK_DEP_PDF],
[

requirement=""

if test "x${with_pdf_backend}" = "xpoppler" ; then
   PKG_CHECK_MODULES([POPPLER],
      [poppler >= 0.12],
      [
       have_dep="yes"
       requirement="poppler"
      ],
      [have_dep="no"])

   if test "x${have_dep}" = "xyes" ; then
      AC_LANG_PUSH(C++)
      CPPFLAGS_save=${CPPFLAGS}
      CPPFLAGS="${CPPFLAGS} ${POPPLER_CFLAGS}"
      AC_CHECK_HEADER([GlobalParams.h],
         [have_dep="yes"],
         [
          AC_MSG_WARN([Xpdf headers not found. Verify that poppler is configured with the option --enable-xpdf-headers])
          have_dep="no"
         ])
      CPPFLAGS=${CPPFLAGS_save}
      AC_LANG_POP(C++)
   fi

   if test "x${have_dep}" = "xyes" ; then
      PKG_CHECK_MODULES([POPPLER_0_14],
         [poppler >= 0.14],
         [AC_DEFINE([HAVE_POPPLER_0_14], [1], [Set to 1 if poppler 0.14 is installed])],
         [dummy=yes])
   fi
else
dnl PDF backend is enabled and it's not poppler, then it is mupdf

   MUPDF_CFLAGS=""
   MUPDF_LIBS=""

   mupdf_pkgs="freetype2"
   jp2k_libs=""

   PKG_CHECK_EXISTS([libopenjpeg1],
      [
       have_pkg_jp2k="yes"
       mupdf_pkgs="${mupdf_pkgs} libopenjpeg1"
      ],
      [have_pkg_jp2k="no"])

   if test "x${have_pkg_jp2k}" = "xno" ; then
      PKG_CHECK_EXISTS([libopenjpeg],
         [
          have_pkg_jp2k="yes"
          mupdf_pkgs="${mupdf_pkgs} libopenjpeg"
         ],
         [have_pkg_jp2k="no"])
   fi

dnl Freetype
   PKG_CHECK_MODULES([MUPDF_DEP],
      [${mupdf_pkgs}],
      [have_dep="yes"],
      [have_dep="no"])

dnl libopenjpeg (if the .pc does not exist)
   if test "x${have_pkg_jp2k}" = "xno" && test "x${have_dep}" = "xyes" ; then
      AC_MSG_NOTICE([no pkg-config file for openjpeg, checking files individually])
      AC_CHECK_HEADER([openjpeg.h], [have_dep="yes"], [have_dep="no"])
   fi

   if test "x${have_pkg_jp2k}" = "xno" && test "x${have_dep}" = "xyes" ; then
      AC_CHECK_LIB([openjpeg], [opj_image_create],
         [
          have_dep="yes"
          jp2k_libs="-lopenjpeg"
         ],
         [have_dep="no"])
   fi

dnl jbig2
   if test "x${have_dep}" = "xyes" ; then
      AC_CHECK_HEADER([jbig2.h], [have_dep="yes"], [have_dep="no"])
   fi

   if test "x${have_dep}" = "xyes" ; then
      AC_CHECK_LIB([jbig2dec], [jbig2_ctx_new], [have_dep="yes"], [have_dep="no"])
   fi

   if test "x${have_dep}" = "xyes" ; then
      requirement="${mupdf_pkgs}"
      MUPDF_CFLAGS="${MUPDF_DEP_CFLAGS}"
      MUPDF_LIBS="${MUPDF_DEP_LIBS} -ljbig2dec ${jp2k_libs}"
   fi

dnl CJK fonts
   if ! test "x${want_mupdf_cjk}" = "xyes" ; then
      BUILD_MUPDF_CJK_FONTS="-DNOCJK"
   fi

fi

AC_SUBST(MUPDF_CFLAGS)
AC_SUBST(MUPDF_LIBS)
AC_SUBST(BUILD_MUPDF_CJK_FONTS)

AM_CONDITIONAL([HAVE_PDF_BACKEND_MUPDF], [test "x${with_pdf_backend}" = "xmupdf"])
AM_CONDITIONAL([BUILD_MUPDF_CJK_FONTS], [test "x${want_mupdf_cjk}" = "xyes"])

if test "x$1" = "xstatic" ; then
   requirement_eyesight="${requirement} ${requirement_eyesight}"
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
   requirement_eyesight="${requirement} ${requirement_eyesight}"
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
       enable_module="yes"
    else
       if test "x${enableval}" = "xstatic" ; then
          enable_module="static"
       else
          enable_module="no"
       fi
    fi
   ],
   [enable_module="yes"])

if test "x${enable_module}" = "xyes" || test "x${enable_module}" = "xstatic" ; then
   want_module="yes"
fi

have_module="no"
if test "x${want_module}" = "xyes" && (test "x${enable_module}" = "xyes" || test "x${enable_module}" = "xstatic") ; then
   m4_default([EYESIGHT_CHECK_DEP_]m4_defn([UP]))(${enable_module}, [have_module="yes"], [have_module="no"])
fi

AC_MSG_CHECKING([whether to enable $1 module built])
AC_MSG_RESULT([${have_module}])

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
