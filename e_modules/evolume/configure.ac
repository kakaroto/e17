dnl    VERSION : MAJOR_RELEASE.FEATURE_ADD.BUG_FIX
AC_INIT(eVolume,0.0.19,boldin.pavel@gmail.com,evolume)
AC_CONFIG_SRCDIR(src/module/e_mod_main.c)
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(config.h)

AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_C_CONST
AM_ENABLE_SHARED
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_C___ATTRIBUTE__

ALL_LINGUAS="eo it fi sv"
AC_SUBST(ALL_LINGUAS)

AM_GNU_GETTEXT([external]) 
AM_GNU_GETTEXT_VERSION(0.12.1)
#if test "x$LIBINTL" = "x"; then
#    LIBINTL="$INTLLIBS"
#fi

# Checks for libraries.
AC_PATH_GENERIC(eet, 0.9.10, [
			      AC_SUBST(eet_libs)
			      AC_SUBST(eet_cflags) ],
			      AC_MSG_ERROR(Cannot find eet: Is eet-config in path?))
eet_libs=`eet-config --libs`
eet_cflags=`eet-config --cflags`
AC_SUBST(eet_flags)
AC_SUBST(eet_cflags)

AC_PATH_GENERIC(evas, 0.9.9, [
			      AC_SUBST(evas_libs)
			      AC_SUBST(evas_cflags) ],
			      AC_MSG_ERROR(Cannot find evas: Is evas-config in path?))
evas_libs=`evas-config --libs`
evas_cflags=`evas-config --cflags`
AC_SUBST(evas_libs)
AC_SUBST(evas_cflags)

AC_PATH_GENERIC(ecore, 0.9.9, [
			       AC_SUBST(ecore_libs)
			       AC_SUBST(ecore_cflags) ],
			       AC_MSG_ERROR(Cannot find ecore: Is ecore-config in path?))
ecore_libs=`ecore-config --libs`
ecore_cflags=`ecore-config --cflags`
AC_SUBST(ecore_libs)
AC_SUBST(ecore_cflags)

AC_PATH_GENERIC(embryo, 0.5.0, [
				AC_SUBST(embryo_libs)
				AC_SUBST(embryo_cflags) ],
				AC_MSG_ERROR(Cannot find embryo: Is embryo-config in path?))
embryo_libs=`embryo-config --libs`
embryo_cflags=`embryo-config --cflags`
AC_SUBST(embryo_libs)
AC_SUBST(embryo_cflags)

AC_PATH_GENERIC(edje, 0.5.0, [
			      AC_SUBST(edje_libs)
			      AC_SUBST(edje_cflags) ],
			      AC_MSG_ERROR(Cannot find edje: Is edje-config in path?))
edje_libs=`edje-config --libs`
edje_cflags=`edje-config --cflags`
AC_SUBST(edje_libs)
AC_SUBST(edje_cflags)

edje_cc="`$EDJE_CONFIG --prefix`/bin/edje_cc"
AC_ARG_WITH(edje-cc,
	    [  --with-edje-cc=PATH              specify a specific path to edje_cc],
	    [
	     v=$withval;
	     edje_cc=$v
	     echo "  Enlightenment edje_cc explicitly set to "$edje_cc;
	     ],[
		edje_cc="`$EDJE_CONFIG --prefix`/bin/edje_cc"
		])
AC_SUBST(edje_cc)

AC_PATH_GENERIC(enlightenment, 0.16.999, [
					  AC_SUBST(e_libs)
					  AC_SUBST(e_cflags) ],
					  AC_MSG_ERROR(Cannot find enlightenment: Is enlightenment-config in path?))
e_libs=`enlightenment-config --libs`
e_cflags=`enlightenment-config --cflags`
AC_SUBST(e_libs)
AC_SUBST(e_cflags)

MODULE_ARCH="$host_os-$host_cpu"
AC_SUBST(MODULE_ARCH)
AC_DEFINE_UNQUOTED(MODULE_ARCH, "$MODULE_ARCH", "Module architecture")

AC_PREFIX_DEFAULT(${HOME}/.e/e)
if test "x$prefix" = "xNONE"; then
    prefix=$ac_default_prefix
    datarootdir=${prefix}
    moduledir=${prefix}/modules
else
    datarootdir=${prefix}/share
    moduledir=${prefix}/lib/enlightenment/modules
fi

AC_SUBST(E_MODULES, "${moduledir}")
AC_MSG_NOTICE([Module will be installed under ${moduledir}])

localedir=${datarootdir}/locale
AC_SUBST(LOCALEDIR, "${localedir}")
AC_DEFINE_UNQUOTED(LOCALEDIR, "${localedir}", [Module Locale Directory])

dnl FIXME: flags and libs there

AM_PATH_ALSA(1.0.8)
#alsa_libs=-lasound
#alsa_cflags=-I/usr/include/alsa

AC_SUBST(alsa_libs)
AC_SUBST(alsa_cflags)

AC_OUTPUT([Makefile
	   data/Makefile
	   data/themes/Makefile
	   data/themes/default/Makefile
	   patches/Makefile
	   src/Makefile
	   po/Makefile
	   src/lib/Makefile
	   src/module/Makefile
	   e_modules-evolume.spec]) 

echo ------------------------------------
echo "If you want to contribute to or maintain project, please contact me"
echo -e "\t\tdavinchi@jabber.org"
echo ------------------------------------
echo "PLEASE REMOVE YOU OLD eVolume config: rm ~/.e/e/config/*/module.evolume.cfg"
echo ------------------------------------
