# Configure paths for Ewd
# Conrad Parker 2001-01-24
# blatantly ripped from gtk.m4, Owen Taylor     97-11-3

dnl AM_PATH_EWD([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND [, MODULES]]]])
dnl Test for Ewd, and define EWD_CFLAGS and ECORE_LIBS
dnl
AC_DEFUN(AM_PATH_EWD,
[dnl 
dnl Get the cflags and libraries from the ewd-config script
dnl
AC_ARG_WITH(ewd-prefix,[  --with-ewd-prefix=PFX   Prefix where Ewd is installed (optional)],
            ewd_config_prefix="$withval", ewd_config_prefix="")
AC_ARG_WITH(ewd-exec-prefix,[  --with-ewd-exec-prefix=PFX Exec prefix where Ewd is installed (optional)],
            ewd_config_exec_prefix="$withval", ewd_config_exec_prefix="")
AC_ARG_ENABLE(ewdtest, [  --disable-ewdtest       Do not try to compile and run a test Ewd program],
		    , enable_ewdtest=yes)

  if test x$ewd_config_exec_prefix != x ; then
     ewd_config_args="$ewd_config_args --exec-prefix=$ewd_config_exec_prefix"
     if test x${EWD_CONFIG+set} != xset ; then
        EWD_CONFIG=$ewd_config_exec_prefix/bin/ewd-config
     fi
  fi
  if test x$ewd_config_prefix != x ; then
     ewd_config_args="$ewd_config_args --prefix=$ewd_config_prefix"
     if test x${EWD_CONFIG+set} != xset ; then
        EWD_CONFIG=$ewd_config_prefix/bin/ewd-config
     fi
  fi

  AC_PATH_PROG(EWD_CONFIG, ewd-config, no)
  min_ewd_version=ifelse([$1], ,0.0.0,$1)
  AC_MSG_CHECKING(for Ewd - version >= $min_ewd_version)
  no_ewd=""
  if test "$EWD_CONFIG" = "no" ; then
    no_ewd=yes
  else
    EWD_CFLAGS=`$ECORE_CONFIG $ewd_config_args --cflags`
    EWD_LIBS=`$ECORE_CONFIG $ewd_config_args --libs`
    ewd_config_major_version=`$EWD_CONFIG $ewd_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    ewd_config_minor_version=`$EWD_CONFIG $ewd_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    ewd_config_micro_version=`$EWD_CONFIG $ewd_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

dnl **********************************************************************
dnl Ewd cannot currently report its own version ; version info is not
dnl given in Ewd.h
dnl Thus, the "enable_ewdtest" stuff is currently disabled, enable once
dnl you can determine the currently installed version by querying Ewd[.h]
dnl
dnl K. 2001-01-24
dnl **********************************************************************

dnl    if test "x$enable_ewdtest" = "xyes" ; then
dnl      ac_save_CFLAGS="$CFLAGS"
dnl      ac_save_LIBS="$LIBS"
dnl      CFLAGS="$CFLAGS $EWD_CFLAGS"
dnl      LIBS="$EWD_LIBS $LIBS"
dnl dnl
dnl dnl Now check if the installed Ewd is sufficiently new. (Also sanity
dnl dnl checks the results of ewd-config to some extent
dnl dnl
dnl       rm -f conf.ewdtest
dnl       AC_TRY_RUN([
dnl #include <Ewd.h>
dnl #include <stdio.h>
dnl #include <stdlib.h>
dnl 
dnl int 
dnl main ()
dnl {
dnl   int major, minor, micro;
dnl   char *tmp_version;
dnl 
dnl   system ("touch conf.ewdtest");
dnl 
dnl   /* HP/UX 9 (%@#!) writes to sscanf strings */
dnl   tmp_version = g_strdup("$min_ewd_version");
dnl   if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
dnl      printf("%s, bad version string\n", "$min_ewd_version");
dnl      exit(1);
dnl    }
dnl 
dnl   if ((ewd_major_version != $ewd_config_major_version) ||
dnl       (ewd_minor_version != $ewd_config_minor_version) ||
dnl       (ewd_micro_version != $ewd_config_micro_version))
dnl     {
dnl       printf("\n*** 'ewd-config --version' returned %d.%d.%d, but Ewd (%d.%d.%d)\n", 
dnl              $ewd_config_major_version, $ewd_config_minor_version, $ewd_config_micro_version,
dnl              ewd_major_version, ewd_minor_version, ewd_micro_version);
dnl       printf ("*** was found! If ewd-config was correct, then it is best\n");
dnl       printf ("*** to remove the old version of Ewd. You may also be able to fix the error\n");
dnl       printf("*** by modifying your LD_LIBRARY_PATH enviroment variable, or by editing\n");
dnl       printf("*** /etc/ld.so.conf. Make sure you have run ldconfig if that is\n");
dnl       printf("*** required on your system.\n");
dnl       printf("*** If ewd-config was wrong, set the environment variable EWD_CONFIG\n");
dnl       printf("*** to point to the correct copy of ewd-config, and remove the file config.cache\n");
dnl       printf("*** before re-running configure\n");
dnl     } 
dnl #if defined (EWD_MAJOR_VERSION) && defined (ECORE_MINOR_VERSION) && defined (ECORE_MICRO_VERSION)
dnl   else if ((ewd_major_version != EWD_MAJOR_VERSION) ||
dnl 	   (ewd_minor_version != EWD_MINOR_VERSION) ||
dnl            (ewd_micro_version != EWD_MICRO_VERSION))
dnl     {
dnl       printf("*** Ewd header files (version %d.%d.%d) do not match\n",
dnl 	     EWD_MAJOR_VERSION, ECORE_MINOR_VERSION, ECORE_MICRO_VERSION);
dnl       printf("*** library (version %d.%d.%d)\n",
dnl 	     ewd_major_version, ewd_minor_version, ewd_micro_version);
dnl     }
dnl #endif /* defined (EWD_MAJOR_VERSION) ... */
dnl   else
dnl     {
dnl       if ((ewd_major_version > major) ||
dnl         ((ewd_major_version == major) && (ewd_minor_version > minor)) ||
dnl         ((ewd_major_version == major) && (ewd_minor_version == minor) && (ewd_micro_version >= micro)))
dnl       {
dnl         return 0;
dnl        }
dnl      else
dnl       {
dnl         printf("\n*** An old version of Ewd (%d.%d.%d) was found.\n",
dnl                ewd_major_version, ewd_minor_version, ewd_micro_version);
dnl         printf("*** You need a version of Ewd newer than %d.%d.%d. The latest version of\n",
dnl 	       major, minor, micro);
dnl         printf("*** Ewd is always available from ftp://ftp.enlightenment.org.\n");
dnl         printf("***\n");
dnl         printf("*** If you have already installed a sufficiently new version, this error\n");
dnl         printf("*** probably means that the wrong copy of the ewd-config shell script is\n");
dnl         printf("*** being found. The easiest way to fix this is to remove the old version\n");
dnl         printf("*** of Ewd, but you can also set the EWD_CONFIG environment to point to the\n");
dnl         printf("*** correct copy of ewd-config. (In this case, you will have to\n");
dnl         printf("*** modify your LD_LIBRARY_PATH enviroment variable, or edit /etc/ld.so.conf\n");
dnl         printf("*** so that the correct libraries are found at run-time))\n");
dnl       }
dnl     }
dnl   return 1;
dnl }
dnl ],, no_ewd=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
dnl        CFLAGS="$ac_save_CFLAGS"
dnl        LIBS="$ac_save_LIBS"
dnl      fi
dnl **********************************************************************

  fi
  if test "x$no_ewd" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$EWD_CONFIG" = "no" ; then
       echo "*** The ewd-config script installed by Ewd could not be found"
       echo "*** If Ewd was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the EWD_CONFIG environment variable to the"
       echo "*** full path to ewd-config."
     else
       if test -f conf.ewdtest ; then
        :
       else
          echo "*** Could not run Ewd test program, checking why..."
          CFLAGS="$CFLAGS $EWD_CFLAGS"
          LIBS="$LIBS $EWD_LIBS"
          AC_TRY_LINK([
#include <Ewd.h>
#include <stdio.h>
],      [ return ((ewd_major_version) || (ewd_minor_version) || (ewd_micro_version)); ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding Ewd or finding the wrong"
          echo "*** version of Ewd. If it is not finding Ewd, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"
          echo "***"
          echo "*** If you have a RedHat 5.0 system, you should remove the Ewd package that"
          echo "*** came with the system with the command"
          echo "***"
          echo "***    rpm --erase --nodeps ewd ewd-devel" ],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means Ewd was incorrectly installed"
          echo "*** or that you have moved Ewd since it was installed. In the latter case, you"
          echo "*** may want to edit the ewd-config script: $EWD_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     EWD_CFLAGS=""
     EWD_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(EWD_CFLAGS)
  AC_SUBST(EWD_LIBS)
  rm -f conf.ewdtest
])
