# Configure paths for Estyle
# blatantly ripped from gtk.m4, Owen Taylor     97-11-3

dnl AM_PATH_ESTYLE([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND [, MODULES]]]])
dnl Test for Estyle, and define ESTYLE_CFLAGS
dnl
AC_DEFUN(AM_PATH_ESTYLE,
[dnl 
dnl Get the cflags and libraries from the estyle-config script
dnl
AC_ARG_WITH(estyle-prefix,[  --with-estyle-prefix=PFX   Prefix where Estyle is installed (optional)],
            estyle_config_prefix="$withval", estyle_config_prefix="")
AC_ARG_WITH(estyle-exec-prefix,[  --with-estyle-exec-prefix=PFX Exec prefix where Estyle is installed (optional)],
            estyle_config_exec_prefix="$withval", estyle_config_exec_prefix="")
AC_ARG_ENABLE(estyletest, [  --disable-estyletest       Do not try to compile and run a test Estyle program],
		    , enable_estyletest=yes)

  if test x$estyle_config_exec_prefix != x ; then
     estyle_config_args="$estyle_config_args --exec-prefix=$estyle_config_exec_prefix"
     if test x${ESTYLE_CONFIG+set} != xset ; then
        ESTYLE_CONFIG=$estyle_config_exec_prefix/bin/estyle-config
     fi
  fi
  if test x$estyle_config_prefix != x ; then
     estyle_config_args="$estyle_config_args --prefix=$estyle_config_prefix"
     if test x${ESTYLE_CONFIG+set} != xset ; then
        ESTYLE_CONFIG=$estyle_config_prefix/bin/estyle-config
     fi
  fi

  AC_PATH_PROG(ESTYLE_CONFIG, estyle-config, no)
  min_estyle_version=ifelse([$1], ,0.0.0,$1)
  AC_MSG_CHECKING(for Estyle - version >= $min_estyle_version)
  no_estyle=""
  if test "$ESTYLE_CONFIG" = "no" ; then
    no_estyle=yes
  else
    ESTYLE_CFLAGS=`$estyle_config_args --cflags`
    ESTYLE_LIBS=`$estyle_config_args --libs`
    estyle_config_major_version=`$ESTYLE_CONFIG $estyle_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    estyle_config_minor_version=`$ESTYLE_CONFIG $estyle_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    estyle_config_micro_version=`$ESTYLE_CONFIG $estyle_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

dnl **********************************************************************
dnl Estyle cannot currently report its own version ; version info is not
dnl given in Estyle.h
dnl Thus, the "enable_estyletest" stuff is currently disabled, enable once
dnl you can determine the currently installed version by querying Estyle[.h]
dnl
dnl K. 2001-01-24
dnl **********************************************************************

dnl    if test "x$enable_estyletest" = "xyes" ; then
dnl      ac_save_CFLAGS="$CFLAGS"
dnl      ac_save_LIBS="$LIBS"
dnl      CFLAGS="$CFLAGS $ESTYLE_CFLAGS"
dnl      LIBS="$ESTYLE_LIBS $LIBS"
dnl dnl
dnl dnl Now check if the installed Estyle is sufficiently new. (Also sanity
dnl dnl checks the results of estyle-config to some extent
dnl dnl
dnl       rm -f conf.estyletest
dnl       AC_TRY_RUN([
dnl #include <Estyle.h>
dnl #include <stdio.h>
dnl #include <stdlib.h>
dnl 
dnl int 
dnl main ()
dnl {
dnl   int major, minor, micro;
dnl   char *tmp_version;
dnl 
dnl   system ("touch conf.estyletest");
dnl 
dnl   /* HP/UX 9 (%@#!) writes to sscanf strings */
dnl   tmp_version = g_strdup("$min_estyle_version");
dnl   if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
dnl      printf("%s, bad version string\n", "$min_estyle_version");
dnl      exit(1);
dnl    }
dnl 
dnl   if ((estyle_major_version != $estyle_config_major_version) ||
dnl       (estyle_minor_version != $estyle_config_minor_version) ||
dnl       (estyle_micro_version != $estyle_config_micro_version))
dnl     {
dnl       printf("\n*** 'estyle-config --version' returned %d.%d.%d, but Estyle (%d.%d.%d)\n", 
dnl              $estyle_config_major_version, $estyle_config_minor_version, $estyle_config_micro_version,
dnl              estyle_major_version, estyle_minor_version, estyle_micro_version);
dnl       printf ("*** was found! If estyle-config was correct, then it is best\n");
dnl       printf ("*** to remove the old version of Estyle. You may also be able to fix the error\n");
dnl       printf("*** by modifying your LD_LIBRARY_PATH enviroment variable, or by editing\n");
dnl       printf("*** /etc/ld.so.conf. Make sure you have run ldconfig if that is\n");
dnl       printf("*** required on your system.\n");
dnl       printf("*** If estyle-config was wrong, set the environment variable ESTYLE_CONFIG\n");
dnl       printf("*** to point to the correct copy of estyle-config, and remove the file config.cache\n");
dnl       printf("*** before re-running configure\n");
dnl     } 
dnl #if defined (ESTYLE_MAJOR_VERSION)
dnl   else if ((estyle_major_version != ESTYLE_MAJOR_VERSION) ||
dnl 	   (estyle_minor_version != ESTYLE_MINOR_VERSION) ||
dnl            (estyle_micro_version != ESTYLE_MICRO_VERSION))
dnl     {
dnl       printf("*** Estyle header files (version %d) do not match\n",
dnl 	     ESTYLE_MAJOR_VERSION);
dnl       printf("*** library (version %d.%d.%d)\n",
dnl 	     estyle_major_version, estyle_minor_version, estyle_micro_version);
dnl     }
dnl #endif /* defined (ESTYLE_MAJOR_VERSION) ... */
dnl   else
dnl     {
dnl       if ((estyle_major_version > major) ||
dnl         ((estyle_major_version == major) && (estyle_minor_version > minor)) ||
dnl         ((estyle_major_version == major) && (estyle_minor_version == minor) && (estyle_micro_version >= micro)))
dnl       {
dnl         return 0;
dnl        }
dnl      else
dnl       {
dnl         printf("\n*** An old version of Estyle (%d.%d.%d) was found.\n",
dnl                estyle_major_version, estyle_minor_version, estyle_micro_version);
dnl         printf("*** You need a version of Estyle newer than %d.%d.%d. The latest version of\n",
dnl 	       major, minor, micro);
dnl         printf("*** Estyle is always available from ftp://ftp.enlightenment.org.\n");
dnl         printf("***\n");
dnl         printf("*** If you have already installed a sufficiently new version, this error\n");
dnl         printf("*** probably means that the wrong copy of the estyle-config shell script is\n");
dnl         printf("*** being found. The easiest way to fix this is to remove the old version\n");
dnl         printf("*** of Estyle, but you can also set the ESTYLE_CONFIG environment to point to the\n");
dnl         printf("*** correct copy of estyle-config. (In this case, you will have to\n");
dnl         printf("*** modify your LD_LIBRARY_PATH enviroment variable, or edit /etc/ld.so.conf\n");
dnl         printf("*** so that the correct libraries are found at run-time))\n");
dnl       }
dnl     }
dnl   return 1;
dnl }
dnl ],, no_estyle=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
dnl        CFLAGS="$ac_save_CFLAGS"
dnl        LIBS="$ac_save_LIBS"
dnl      fi
dnl **********************************************************************

  fi
  if test "x$no_estyle" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$ESTYLE_CONFIG" = "no" ; then
       echo "*** The estyle-config script installed by Estyle could not be found"
       echo "*** If Estyle was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the ESTYLE_CONFIG environment variable to the"
       echo "*** full path to estyle-config."
     else
       if test -f conf.estyletest ; then
        :
       else
          echo "*** Could not run Estyle test program, checking why..."
          CFLAGS="$CFLAGS $ESTYLE_CFLAGS"
          LIBS="$LIBS $ESTYLE_LIBS"
          AC_TRY_LINK([
#include <Estyle.h>
#include <stdio.h>
],      [ return ((estyle_major_version) || (estyle_minor_version) || (estyle_micro_version)); ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding Estyle or finding the wrong"
          echo "*** version of Estyle. If it is not finding Estyle, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"
          echo "***"
          echo "*** If you have a RedHat 5.0 system, you should remove the Estyle package that"
          echo "*** came with the system with the command"
          echo "***"
          echo "***    rpm --erase --nodeps estyle estyle-devel" ],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means Estyle was incorrectly installed"
          echo "*** or that you have moved Estyle since it was installed. In the latter case, you"
          echo "*** may want to edit the estyle-config script: $ESTYLE_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     ESTYLE_CFLAGS=""
     ESTYLE_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(ESTYLE_CFLAGS)
  AC_SUBST(ESTYLE_LIBS)
  rm -f conf.estyletest
])
