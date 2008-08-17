# Configure paths for Etox
# blatantly ripped from gtk.m4, Owen Taylor     97-11-3

dnl AM_PATH_ETOX([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND [, MODULES]]]])
dnl Test for Etox, and define ETOX_CFLAGS
dnl
AC_DEFUN([AM_PATH_ETOX],
[dnl 
dnl Get the cflags and libraries from the etox-config script
dnl
AC_ARG_WITH(etox-prefix,[  --with-etox-prefix=PFX   Prefix where Etox is installed (optional)],
            etox_config_prefix="$withval", etox_config_prefix="")
AC_ARG_WITH(etox-exec-prefix,[  --with-etox-exec-prefix=PFX Exec prefix where Etox is installed (optional)],
            etox_config_exec_prefix="$withval", etox_config_exec_prefix="")
AC_ARG_ENABLE(etoxtest, [  --disable-etoxtest       Do not try to compile and run a test Etox program],
		    , enable_etoxtest=yes)

  if test x$etox_config_exec_prefix != x ; then
     etox_config_args="$etox_config_args --exec-prefix=$etox_config_exec_prefix"
     if test x${ETOX_CONFIG+set} != xset ; then
        ETOX_CONFIG=$etox_config_exec_prefix/bin/etox-config
     fi
  fi
  if test x$etox_config_prefix != x ; then
     etox_config_args="$etox_config_args --prefix=$etox_config_prefix"
     if test x${ETOX_CONFIG+set} != xset ; then
        ETOX_CONFIG=$etox_config_prefix/bin/etox-config
     fi
  fi

  AC_PATH_PROG(ETOX_CONFIG, etox-config, no)
  min_etox_version=ifelse([$1], ,0.0.0,$1)
  AC_MSG_CHECKING(for Etox - version >= $min_etox_version)
  no_etox=""
  if test "$ETOX_CONFIG" = "no" ; then
    no_etox=yes
  else
    ETOX_CFLAGS=`$etox_config_args --cflags`
    ETOX_LIBS=`$etox_config_args --libs`
    etox_config_major_version=`$ETOX_CONFIG $etox_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    etox_config_minor_version=`$ETOX_CONFIG $etox_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    etox_config_micro_version=`$ETOX_CONFIG $etox_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

dnl **********************************************************************
dnl Etox cannot currently report its own version ; version info is not
dnl given in Etox.h
dnl Thus, the "enable_etoxtest" stuff is currently disabled, enable once
dnl you can determine the currently installed version by querying Etox[.h]
dnl
dnl K. 2001-01-24
dnl **********************************************************************

dnl    if test "x$enable_etoxtest" = "xyes" ; then
dnl      ac_save_CFLAGS="$CFLAGS"
dnl      ac_save_LIBS="$LIBS"
dnl      CFLAGS="$CFLAGS $ETOX_CFLAGS"
dnl      LIBS="$ETOX_LIBS $LIBS"
dnl dnl
dnl dnl Now check if the installed Etox is sufficiently new. (Also sanity
dnl dnl checks the results of etox-config to some extent
dnl dnl
dnl       rm -f conf.etoxtest
dnl       AC_TRY_RUN([
dnl #include <Etox.h>
dnl #include <stdio.h>
dnl #include <stdlib.h>
dnl 
dnl int 
dnl main ()
dnl {
dnl   int major, minor, micro;
dnl   char *tmp_version;
dnl 
dnl   system ("touch conf.etoxtest");
dnl 
dnl   /* HP/UX 9 (%@#!) writes to sscanf strings */
dnl   tmp_version = g_strdup("$min_etox_version");
dnl   if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
dnl      printf("%s, bad version string\n", "$min_etox_version");
dnl      exit(1);
dnl    }
dnl 
dnl   if ((etox_major_version != $etox_config_major_version) ||
dnl       (etox_minor_version != $etox_config_minor_version) ||
dnl       (etox_micro_version != $etox_config_micro_version))
dnl     {
dnl       printf("\n*** 'etox-config --version' returned %d.%d.%d, but Etox (%d.%d.%d)\n", 
dnl              $etox_config_major_version, $etox_config_minor_version, $etox_config_micro_version,
dnl              etox_major_version, etox_minor_version, etox_micro_version);
dnl       printf ("*** was found! If etox-config was correct, then it is best\n");
dnl       printf ("*** to remove the old version of Etox. You may also be able to fix the error\n");
dnl       printf("*** by modifying your LD_LIBRARY_PATH enviroment variable, or by editing\n");
dnl       printf("*** /etc/ld.so.conf. Make sure you have run ldconfig if that is\n");
dnl       printf("*** required on your system.\n");
dnl       printf("*** If etox-config was wrong, set the environment variable ETOX_CONFIG\n");
dnl       printf("*** to point to the correct copy of etox-config, and remove the file config.cache\n");
dnl       printf("*** before re-running configure\n");
dnl     } 
dnl #if defined (ETOX_MAJOR_VERSION)
dnl   else if ((etox_major_version != ETOX_MAJOR_VERSION) ||
dnl 	   (etox_minor_version != ETOX_MINOR_VERSION) ||
dnl            (etox_micro_version != ETOX_MICRO_VERSION))
dnl     {
dnl       printf("*** Etox header files (version %d) do not match\n",
dnl 	     ETOX_MAJOR_VERSION);
dnl       printf("*** library (version %d.%d.%d)\n",
dnl 	     etox_major_version, etox_minor_version, etox_micro_version);
dnl     }
dnl #endif /* defined (ETOX_MAJOR_VERSION) ... */
dnl   else
dnl     {
dnl       if ((etox_major_version > major) ||
dnl         ((etox_major_version == major) && (etox_minor_version > minor)) ||
dnl         ((etox_major_version == major) && (etox_minor_version == minor) && (etox_micro_version >= micro)))
dnl       {
dnl         return 0;
dnl        }
dnl      else
dnl       {
dnl         printf("\n*** An old version of Etox (%d.%d.%d) was found.\n",
dnl                etox_major_version, etox_minor_version, etox_micro_version);
dnl         printf("*** You need a version of Etox newer than %d.%d.%d. The latest version of\n",
dnl 	       major, minor, micro);
dnl         printf("*** Etox is always available from ftp://ftp.enlightenment.org.\n");
dnl         printf("***\n");
dnl         printf("*** If you have already installed a sufficiently new version, this error\n");
dnl         printf("*** probably means that the wrong copy of the etox-config shell script is\n");
dnl         printf("*** being found. The easiest way to fix this is to remove the old version\n");
dnl         printf("*** of Etox, but you can also set the ETOX_CONFIG environment to point to the\n");
dnl         printf("*** correct copy of etox-config. (In this case, you will have to\n");
dnl         printf("*** modify your LD_LIBRARY_PATH enviroment variable, or edit /etc/ld.so.conf\n");
dnl         printf("*** so that the correct libraries are found at run-time))\n");
dnl       }
dnl     }
dnl   return 1;
dnl }
dnl ],, no_etox=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
dnl        CFLAGS="$ac_save_CFLAGS"
dnl        LIBS="$ac_save_LIBS"
dnl      fi
dnl **********************************************************************

  fi
  if test "x$no_etox" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$ETOX_CONFIG" = "no" ; then
       echo "*** The etox-config script installed by Etox could not be found"
       echo "*** If Etox was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the ETOX_CONFIG environment variable to the"
       echo "*** full path to etox-config."
     else
       if test -f conf.etoxtest ; then
        :
       else
          echo "*** Could not run Etox test program, checking why..."
          CFLAGS="$CFLAGS $ETOX_CFLAGS"
          LIBS="$LIBS $ETOX_LIBS"
          AC_TRY_LINK([
#include <Etox.h>
#include <stdio.h>
],      [ return ((etox_major_version) || (etox_minor_version) || (etox_micro_version)); ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding Etox or finding the wrong"
          echo "*** version of Etox. If it is not finding Etox, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"
          echo "***"
          echo "*** If you have a RedHat 5.0 system, you should remove the Etox package that"
          echo "*** came with the system with the command"
          echo "***"
          echo "***    rpm --erase --nodeps etox etox-devel" ],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means Etox was incorrectly installed"
          echo "*** or that you have moved Etox since it was installed. In the latter case, you"
          echo "*** may want to edit the etox-config script: $ETOX_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     ETOX_CFLAGS=""
     ETOX_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(ETOX_CFLAGS)
  AC_SUBST(ETOX_LIBS)
  rm -f conf.etoxtest
])
