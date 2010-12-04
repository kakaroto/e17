# get rid of that stupid cache mechanism
rm -f config.cache

# Autoconf init.
AC_PREREQ(2.59)
AC_INIT([elixir], [0.1.31], [cedric.bail@free.fr])
AC_CONFIG_SRCDIR([configure.ac])

# Auxiliary files
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_FILES([
	Makefile
	build-aux/Makefile
])

AC_ISC_POSIX
AC_GNU_SOURCE
AC_CANONICAL_BUILD
AC_CANONICAL_HOST

# Automake init
AM_INIT_AUTOMAKE([1.9.2 foreign dist-bzip2])
AM_CONFIG_HEADER([config.h])

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# Libtool
AC_LIBTOOL_WIN32_DLL
define([AC_LIBTOOL_LANG_CXX_CONFIG], [:])dnl
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl
AC_PROG_LIBTOOL

VMAJ=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $1);}'`
VMIN=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $2);}'`
VMIC=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $3);}'`
SNAP=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $4);}'`
version_info=`expr $VMAJ + $VMIN`":$VMIC:$VMIN"
AC_SUBST(VMAJ)
AC_SUBST(version_info)

# Make sure $prefix is defined
test "x$prefix" = xNONE && prefix=$ac_default_prefix
test "x$exec_prefix" = xNONE && exec_prefix=$prefix
test "x$bindir" = xNONE && bindir=$prefix/bin
test "x$libdir" = xNONE && libdir=$prefix/lib

AC_ARG_ENABLE([debug],
	[AS_HELP_STRING([--enable-debug], [for debugging with gdb])],
	[echo "compiling in debug mode... -DDEBUG -ggdb3 -W -Wall";
	 CFLAGS="-DDEBUG -ggdb3 -W -Wall"
	 ])
ac_precompiled="no";

AC_ARG_ENABLE([precompiled],
	[AS_HELP_STRING([--enable-precompiled], [use precompiled javascript if available])],
	[ac_precompiled=$enableval])

if test "x${ac_precompiled}" = "xyes"; then
   AC_DEFINE(ELIXIR_USE_PRECOMPILED, 1, [Set to 1 if we should use precompiled javascript when available])
fi

elixir_static="no"

AC_ARG_ENABLE([static-build],
	[AS_HELP_STRING([--enable-static-build], [Build elixir as a static binary])],
	[elixir_static=$enableval])

AM_CONDITIONAL(ELIXIR_STATIC, test "x${elixir_static}" = "xyes")

if test "x${elixir_static}" = "xyes"; then
   AC_DEFINE(ELIXIR_STATIC, 1, [Set to 1 if Elixir is to be build statically])
fi

# Check for standard programs.
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_C_CONST
AC_TYPE_SIZE_T
AC_FUNC_ALLOCA
AC_C_CONST
AC_C_BIGENDIAN
AC_C_INLINE
AC_C___ATTRIBUTE__

PKG_PROG_PKG_CONFIG

# Define global variable
MODULE_ARCH="$host_os-$host_cpu"
AC_SUBST(MODULE_ARCH)
AC_DEFINE_UNQUOTED(MODULE_ARCH, "$MODULE_ARCH", "Module architecture")

PACKAGE_BIN_DIR=$bindir
AC_SUBST(PACKAGE_BIN_DIR)

PACKAGE_LIB_DIR=$libdir
AC_SUBST(PACKAGE_LIB_DIR)

AC_ARG_WITH(module,
	    AC_HELP_STRING([--with-module=PATH],
			   [specify the base directory where the module could be found [default=${libdir}]]),
	    [moduledir=$withval],
	    [moduledir=$libdir])
PACKAGE_MODULE_DIR=$moduledir
AC_SUBST(PACKAGE_MODULE_DIR)

PACKAGE_DATA_DIR=$prefix/share/$PACKAGE
AC_SUBST(PACKAGE_DATA_DIR)

# Doxygen
AC_CHECK_PROGS([DOXYGEN], [doxygen], [$am_aux_dir/missing doxygen])
AC_ARG_VAR([DOXYGEN], [A C++ documentation system program])

#################################
# Check for SpiderMonkey

smjs_name=""
smjs_dir=""

AC_ARG_WITH(smjs,
	    AC_HELP_STRING([--with-smjs=PATH],
			   [path to SpiderMonkey install directory [default=/usr]]),
	    [smjs_dir=$withval],
	    [smjs_dir="/usr"])

for name in smjs js mozjs; do
    if test -z "$smjs_name"; then
	smjs_cflags="-shared -fPIC -DPIC -DXP_UNIX -I$smjs_dir/include/$name"
	smjs_libs="-L$smjs_dir/lib -l$name"

	CPPFLAGS_X="$CPPFLAGS"
	CPPFLAGS="$smjs_cflags $CPPFLAGS"

	LIBS_X="$LIBS"
	LIBS="$smjs_libs -lm $LIBS"

	cf_result=no

	AC_CHECK_HEADER([jsapi.h],
			[AC_CHECK_LIB([$name], [JS_ExecuteScript],
				      [cf_result=yes])])

	if test "$cf_result" = no; then
	    CPPFLAGS="$CPPFLAGS_X"
	    LIBS="$LIBS_X"
	    unset ac_cv_header_jsapi_h
	else
	    CPPFLAGS="-DXP_UNIX -I$smjs_dir/include/$name $CPPFLAGS_X"
	    LIBS="$smjs_libs -lm $LIBS_X"
	    smjs_name="$name"
	fi
    fi
done

if test -z "$smjs_name"; then
    AC_MSG_ERROR([SpiderMonkey Javascript Library was not found])
fi

AC_MSG_CHECKING(Choosing SpiderMonkey)
AC_MSG_RESULT($smjs_name)

AC_SUBST(smjs_cflags)
AC_SUBST(smjs_libs)

#################################
# Check if we have ZLib
zlib_libs=""
have_zlib="yes"
AC_MSG_CHECKING(whether Zlib ID is to be built)
AC_ARG_ENABLE(zlib, AC_HELP_STRING([--enable-zlib],[enable Zlib ID support]), [
	      if test x"$enableval" = x"yes"; then
		   have_zlib="yes"
	      else
		   have_zlib="no"
	      fi
	], [
		   have_zlib="yes"
	]
)
if test "x$have_zlib" = "xyes"; then
   AC_CHECK_HEADER([zlib.h],
	[
		zlib_libs="-lz"
		have_zlib="yes"
	], [
		have_zlib="no"
	])
else
   AC_MSG_RESULT($have_zlib)
fi
if test x"$have_zlib" = x"yes"; then
   AC_DEFINE(HAVE_ZLIB, 1, [Zlib ID support])
fi
AC_SUBST(zlib_libs)

################################
# Check if we have Gcrypt
gcrypt_libs=""
have_gcrypt="yes"
AC_MSG_CHECKING(whether Gcrypt ID is to be built)
AC_ARG_ENABLE(gcrypt, AC_HELP_STRING([--enable-gcrypt],[enable Gcrypt ID support]), [
	      if test x"$enableval" = x"yes"; then
		   have_gcrypt="yes"
	      else
		   have_gcrypt="no"
	      fi
	], [
		   have_gcrypt="yes"
	]
)
if test "x$have_gcrypt" = "xyes"; then
   AC_CHECK_HEADER([gcrypt.h],
	[
		gcrypt_libs="-lgcrypt"
		have_gcrypt="yes"
	], [
		have_gcrypt="no"
	])
else
   AC_MSG_RESULT($have_gcrypt)
fi
if test x"$have_gcrypt" = x"yes"; then
   AC_DEFINE(HAVE_GCRYPT, 1, [Gcrypt ID support])
fi
AC_SUBST(gcrypt_libs)

################################
# Check if we have Tomcrypt
tomcrypt_libs=""
have_tomcrypt="yes"
AC_MSG_CHECKING(whether Tomcrypt ID is to be built)
AC_ARG_ENABLE(tomcrypt, AC_HELP_STRING([--enable-tomcrypt],[enable Tomcrypt ID support]), [
	      if test x"$enableval" = x"yes"; then
		   have_tomcrypt="yes"
	      else
		   have_tomcrypt="no"
	      fi
	], [
		   have_tomcrypt="yes"
	]
)
if test "x$have_tomcrypt" = "xyes"; then
   AC_CHECK_HEADER([tomcrypt.h],
	[
		tomcrypt_libs="-ltomcrypt"
		have_tomcrypt="yes"
	], [
		have_tomcrypt="no"
	])
else
   AC_MSG_RESULT($have_tomcrypt)
fi
if test x"$have_tomcrypt" = x"yes"; then
   AC_DEFINE(HAVE_TOMCRYPT, 1, [TomCrypt ID support])
fi
AC_SUBST(tomcrypt_libs)

#################################
# Enable and check for SDL
sdl_cflags=""
sdl_libs=""
have_sdl="no"

# Define sdl-config
AC_ARG_WITH(sdl-config, [  --with-sdl-config=SDL_CONFIG     use sdl-config specified],
[ SDL_CONFIG=$withval;
  echo "using "$SDL_CONFIG" for sdl-config"; ],
[ if test -z "$SDL_CONFIG"; then
    AC_PATH_PROG(SDL_CONFIG, "sdl-config", "", $PATH)
  fi
])
if test -z "$SDL_CONFIG" ; then SDL_CONFIG="sdl-config"; fi

# Automatic check...
AC_CHECK_HEADER(SDL/SDL.h,
	[ have_sdl="yes" ],
	[ have_sdl="no" ]
)

# Manual override
AC_MSG_CHECKING(whether SDL module is to be built)
AC_ARG_ENABLE(sdl, AC_HELP_STRING([--enable-sdl],[enable the SDL event module]), [
           if test x"$enableval" = x"yes" ; then
	     have_sdl="yes"
	   else
	     have_sdl="no"
	   fi
        ]
)
if test "x$have_sdl" = "xyes" -a "x$SDL_CONFIG" != "xno"; then
  sdl_cflags=`$SDL_CONFIG --cflags`
  sdl_libs=`$SDL_CONFIG --libs`
  AC_DEFINE(BUILD_MODULE_EVENT_SDL, 1, [SDL event module])
else
  have_sdl="no"
fi
AC_MSG_RESULT($have_sdl)

AC_SUBST(sdl_libs)
AC_SUBST(sdl_cflags)
AM_CONDITIONAL(BUILD_MODULE_EVENT_SDL, test x"$have_sdl" = x"yes")

##################################
# Enable and check libmagic
magic_libs=""
have_magic="no"
test_magic="no"

# Automatic check...
AC_CHECK_HEADER(magic.h,
	[ test_magic="yes" ],
	[ test_magic="no" ]
)

# Manual override
AC_MSG_CHECKING(whether to use mime type)
AC_ARG_ENABLE(magic, AC_HELP_STRING([--enable-magic],[check file type using libmagic]), [
		     if test x"$enableval" = x"yes" ; then
			have_magic="yes"
		     else
			have_magic="no"
		     fi
		  ]
)
if test "x$have_magic" = "xyes" -a "x$test_magic" = "xyes"; then
   magic_libs="-lmagic"
   have_magic="yes"
   AC_DEFINE(ENABLE_MAGIC, 1, [Use libmagic])
fi
AC_SUBST(magic_libs)
AM_CONDITIONAL(ENABLE_MAGIC, test x"$have_magic" = x"yes")
AC_MSG_RESULT($have_magic)

##################################
# Enable and check for SDL_Mixer
have_mix="yes"

AC_MSG_CHECKING(whether SDL_mixer module is to be built)
AC_ARG_ENABLE(mix, AC_HELP_STRING([--enable-mix],[enable the SDL_Mixer module]), [
	   if test x"$enableval" = x"yes"; then
	      AC_MSG_RESULT(yes)
	      have_mix="yes"
	   else
	      AC_MSG_RESULT(no)
	      have_mix="no"
	   fi
	], [
	   AC_MSG_RESULT($have_sdl)
	]
)

if test "x$have_sdl" = "xyes"; then
   if test "x$have_mix" = "xyes"; then
     AC_CHECK_HEADER(SDL/SDL_mixer.h,
			[ have_mix="yes" ],
			[ have_mix="no" ]
			)
   fi
   if test "x$have_mix" = "xyes"; then
      AC_DEFINE(BUILD_MODULE_MIX, 1, [SDL_Mixer module])
   else
      AC_MSG_RESULT(disabling sdl mixer module)
   fi
else
  AC_MSG_RESULT(disabling sdl mixer module)
  have_mix="no"
fi
AM_CONDITIONAL(BUILD_MODULE_MIX, test x"$have_mix" = x"yes")

################################
# Strong dependency
PKG_CHECK_MODULES([EINA], [eina])

################################
# Binding with nice PKG

ELIXIR_CHECK_PKG_MODULE(EET, eet, [Eet file loading support])
EINA_CHECK_STATIC(eet, eet)

ELIXIR_CHECK_PKG_MODULE(EVAS, evas, [Evas module binding support])
EINA_CHECK_STATIC(evas, evas)

ELIXIR_CHECK_PKG_MODULE(ECORE, ecore, [Ecore module binding support])
EINA_CHECK_STATIC(ecore, ecore)

ELIXIR_CHECK_PKG_MODULE(ECORE_CON, [ecore-con], [Ecore connection module binding support])
EINA_CHECK_STATIC(ecore_con, [ecore-con])

ELIXIR_CHECK_PKG_MODULE(ECORE_CONFIG, [ecore-config], [Ecore config module binding support])
EINA_CHECK_STATIC(ecore_config, [ecore-config])

ELIXIR_CHECK_PKG_MODULE(ECORE_EVAS, [ecore-evas], [Ecore Evas module binding support])
EINA_CHECK_STATIC(ecore_evas, [ecore-evas])

ELIXIR_CHECK_PKG_MODULE(ECORE_FILE, [ecore-file], [Ecore File module binding support])
EINA_CHECK_STATIC(ecore_file, [ecore-file])

ELIXIR_CHECK_PKG_MODULE(ECORE_IPC, [ecore-ipc], [Ecore Ipc module binding support])
EINA_CHECK_STATIC(ecore_ipc, [ecore-ipc])

ELIXIR_CHECK_PKG_MODULE(ECORE_DOWNLOAD, [ecore-download], [Ecore Download binding module support])
EINA_CHECK_STATIC(ecore_download, [ecore-download])

ELIXIR_CHECK_PKG_MODULE(EDJE, [edje], [Edje binding module support])
EINA_CHECK_STATIC(edje, [edje])

ELIXIR_CHECK_PKG_MODULE(EMOTION, [emotion], [Emotion binding module support])
EINA_CHECK_STATIC(emotion, [emotion])

ELIXIR_CHECK_PKG_MODULE([SQLITE], [sqlite3], [SQLite binding module support])
EINA_CHECK_STATIC([sqlite3], [sqlite])

ELIXIR_CHECK_PKG_MODULE(EIO, eio, [Eio file loading support])
EINA_CHECK_STATIC(eio, eio)

################################
# Build option and helper module

ELIXIR_CHECK_MODULE(FILE, file, [Support FILE* manipulation in Javascript])
EINA_CHECK_STATIC(file, [file manipulation])

ELIXIR_CHECK_MODULE(DIR, dir, [Support directory manipulation in Javascript])
EINA_CHECK_STATIC(dir, [dir manipulation])

ELIXIR_CHECK_MODULE(CMP, cmp, [Support raw certificate comparison security module])
EINA_CHECK_STATIC(cmp, [raw cmp security])

if test "x$have_gcrypt" = "xyes"; then
  ELIXIR_CHECK_MODULE(REMOTE, remote, [Support remote kill switch server security module])
  EINA_CHECK_STATIC(remote, [remote kill switch security])
else
  AM_CONDITIONAL(BUILD_MODULE_REMOTE, false)
  AM_CONDITIONAL(EINA_STATIC_BUILD_REMOTE, false)
fi

AC_ARG_WITH(cmp-path,   [  --with-cmp-path=CERT.DER         use CERT.DER as the source for comparison [[default = /etc/elixir.der]]],
  [
    DER_CERT_FILE=$withval;
    echo "using $DER_CERT_FILE as source for comparison"
    AC_DEFINE(DER_CERT_FILE, ${DER_CERT_FILE}, "Source for der certificate file")
  ])

ELIXIR_CHECK_MODULE(LET, let, [Let anything pass the security check])
EINA_CHECK_STATIC(let, [let security])

ELIXIR_CHECK_MODULE(TEXT, text, [Basic file loader])
EINA_CHECK_STATIC(text, [Text file loarder])

have_elixir="yes"
EINA_CHECK_STATIC(elixir, [elx object])
EINA_CHECK_STATIC(sdl, [sdl event evas feeder])
EINA_CHECK_STATIC(mix, [SDL_mixer bindings])

###############################
have_strndup="yes"
AC_CHECK_LIB(c, strndup, [ have_strndup="yes" ], [ have_strndup="no" ])

if test "x$have_strndup" = "xyes"; then
  AC_DEFINE(HAVE_STRNDUP, 1, "strndup workaround.")
fi

###############################
want_emotion_security="no"
have_emotion_security="no"
active_emotion_security="no"

EFL_CHECK_FNMATCH([have_emotion_security="yes"], [have_emotion_security="no"])

AC_ARG_ENABLE([emotion-security],
	[AC_HELP_STRING([--enable-emotion-security], [limit content playable by emotion])],
	[want_emotion_security=$enableval])

if test "x${want_emotion_security}" = "xyes" -a "x${have_emotion_security}" = "xyes"; then
   active_emotion_security="yes"
   AC_DEFINE(ELIXIR_EMOTION_SECURITY, 1, [Check file set to emotion against a set of regexp])
fi

AC_MSG_CHECKING([Enabling emotion security check])
AC_MSG_RESULT($active_emotion_security)

###############################

EFL_CHECK_PATH_MAX
EFL_LINKER_FLAG([-Wl,--as-needed])

elixir_cflags="${EMOTION_CFLAGS} ${EDJE_CFLAGS} ${EET_CFLAGS} ${EINA_CFLAGS} ${EVAS_CFLAGS}"
elixir_libs="${EMOTION_LIBS} ${EDJE_LIBS} ${EET_LIBS} ${zlib_libs} ${gcrypt_libs} ${tomcrypt_libs} ${magic_libs} ${EINA_LIBS}"

AC_SUBST(elixir_cflags)
AC_SUBST(elixir_libs)

requirements="eina"
AC_SUBST(requirements)

###############################

# Output
AC_OUTPUT([
elixir.pc
src/Makefile
src/lib/Makefile
src/modules/Makefile
src/modules/bindings/Makefile
src/modules/bindings/dir/Makefile
src/modules/bindings/file/Makefile
src/modules/bindings/elixir/Makefile
src/modules/bindings/evas/Makefile
src/modules/bindings/ecore/Makefile
src/modules/bindings/ecore_con/Makefile
src/modules/bindings/ecore_download/Makefile
src/modules/bindings/ecore_evas/Makefile
src/modules/bindings/edje/Makefile
src/modules/bindings/emotion/Makefile
src/modules/bindings/mix/Makefile
src/modules/bindings/sqlite/Makefile
src/modules/bindings/eet/Makefile
src/modules/bindings/eio/Makefile
src/modules/loaders/edje/Makefile
src/modules/loaders/eet/Makefile
src/modules/loaders/text/Makefile
src/modules/loaders/Makefile
src/modules/security/Makefile
src/modules/security/cmp/Makefile
src/modules/security/let/Makefile
src/modules/security/remote/Makefile
src/include/Makefile
src/bin/Makefile
doc/Makefile
doc/doc.doxy
src/tests/Makefile
elixir-config
README
elixir.c
],[
chmod +x elixir-config
])

# src/modules/ecore_config/Makefile
# src/modules/ecore_file/Makefile
# src/modules/ecore_ipc/Makefile

############################################################################
## Info

echo
echo
echo
echo "---------------------------------"
echo "$PACKAGE $VERSION"
echo "---------------------------------"
echo
echo "Configuration Options Summary:"
echo
echo "Bindings modules:"
echo "  File................: $have_file"
echo "  Dir.................: $have_dir"
echo "  Evas................: $have_evas"
echo "  SDL Mixer...........: $have_mix"
echo "  SQLite..............: $have_sqlite3"
echo "  Zlib ID.............: $have_zlib"
echo "  Gcrypt ID...........: $have_gcrypt"
echo "  Tomcrypt ID.........: $have_tomcrypt"
echo "  Ecore...............: $have_ecore"
echo "  Ecore-Con...........: ${have_ecore_con}"
echo "  Ecore-config........: ${have_ecore_config}"
echo "  Ecore-Download......: ${have_ecore_download}"
echo "  Ecore-Evas..........: ${have_ecore_evas}"
echo "  Ecore-file..........: ${have_ecore_file}"
echo "  Ecore-ipc...........: ${have_ecore_ipc}"
echo "  Edje................: $have_edje"
echo "  Emotion.............: $have_emotion"
echo "     regexp match.....: $active_emotion_security"
echo "  Eet.................: $have_eet"
echo
echo "Loaders modules:"
echo "  Edje................: $have_edje"
echo "  Eet.................: $have_eet"
echo "  Text................: yes"
echo
echo "Security modules:"
echo "  CMP.................: $have_cmp"
echo "  LET.................: $have_let"
echo
echo "Use precompiled javascript when available: $ac_precompiled"
echo

