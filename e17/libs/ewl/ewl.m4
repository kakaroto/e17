# Configure paths for Ewl
# Conrad Parker 2001-01-24
# blatantly ripped from gtk.m4, Owen Taylor     97-11-3

dnl AM_PATH_EWL([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND [, MODULES]]]])
dnl Test for Ewl, and define EWL_CFLAGS and ECORE_LIBS
dnl
AC_DEFUN([AM_PATH_EWL],
[dnl 
dnl Get the cflags and libraries from the ewl-config script
dnl
AC_ARG_WITH(ewl-prefix,[  --with-ewl-prefix=PFX   Prefix where Ewl is installed (optional)],
            ewl_config_prefix="$withval", ewl_config_prefix="")
AC_ARG_WITH(ewl-exec-prefix,[  --with-ewl-exec-prefix=PFX Exec prefix where Ewl is installed (optional)],
            ewl_config_exec_prefix="$withval", ewl_config_exec_prefix="")
AC_ARG_ENABLE(ewltest, [  --disable-ewltest       Do not try to compile and run a test Ewl program],
		    , enable_ewltest=yes)

  if test x$ewl_config_exec_prefix != x ; then
     ewl_config_args="$ewl_config_args --exec-prefix=$ewl_config_exec_prefix"
     if test x${EWL_CONFIG+set} != xset ; then
        EWL_CONFIG=$ewl_config_exec_prefix/bin/ewl-config
     fi
  fi
  if test x$ewl_config_prefix != x ; then
     ewl_config_args="$ewl_config_args --prefix=$ewl_config_prefix"
     if test x${EWL_CONFIG+set} != xset ; then
        EWL_CONFIG=$ewl_config_prefix/bin/ewl-config
     fi
  fi

  AC_PATH_PROG(EWL_CONFIG, ewl-config, no)
  min_ewl_version=ifelse([$1], ,0.0.0,$1)
  AC_MSG_CHECKING(for Ewl - version >= $min_ewl_version)
  no_ewl=""
  if test "$EWL_CONFIG" = "no" ; then
    no_ewl=yes
  else
    EWL_CFLAGS=`$ECORE_CONFIG $ewl_config_args --cflags`
    EWL_LIBS=`$ECORE_CONFIG $ewl_config_args --libs`
    ewl_config_major_version=`$EWL_CONFIG $ewl_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    ewl_config_minor_version=`$EWL_CONFIG $ewl_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    ewl_config_micro_version=`$EWL_CONFIG $ewl_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

dnl **********************************************************************
dnl Ewl cannot currently report its own version ; version info is not
dnl given in Ewl.h
dnl Thus, the "enable_ewltest" stuff is currently disabled, enable once
dnl you can determine the currently installed version by querying Ewl[.h]
dnl
dnl K. 2001-01-24
dnl **********************************************************************

dnl    if test "x$enable_ewltest" = "xyes" ; then
dnl      ac_save_CFLAGS="$CFLAGS"
dnl      ac_save_LIBS="$LIBS"
dnl      CFLAGS="$CFLAGS $EWL_CFLAGS"
dnl      LIBS="$EWL_LIBS $LIBS"
dnl dnl
dnl dnl Now check if the installed Ewl is sufficiently new. (Also sanity
dnl dnl checks the results of ewl-config to some extent
dnl dnl
dnl       rm -f conf.ewltest
dnl       AC_TRY_RUN([
dnl #include <Ewl.h>
dnl #include <stdio.h>
dnl #include <stdlib.h>
dnl 
dnl int 
dnl main ()
dnl {
dnl   int major, minor, micro;
dnl   char *tmp_version;
dnl 
dnl   system ("touch conf.ewltest");
dnl 
dnl   /* HP/UX 9 (%@#!) writes to sscanf strings */
dnl   tmp_version = g_strdup("$min_ewl_version");
dnl   if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
dnl      printf("%s, bad version string\n", "$min_ewl_version");
dnl      exit(1);
dnl    }
dnl 
dnl   if ((ewl_major_version != $ewl_config_major_version) ||
dnl       (ewl_minor_version != $ewl_config_minor_version) ||
dnl       (ewl_micro_version != $ewl_config_micro_version))
dnl     {
dnl       printf("\n*** 'ewl-config --version' returned %d.%d.%d, but Ewl (%d.%d.%d)\n", 
dnl              $ewl_config_major_version, $ewl_config_minor_version, $ewl_config_micro_version,
dnl              ewl_major_version, ewl_minor_version, ewl_micro_version);
dnl       printf ("*** was found! If ewl-config was correct, then it is best\n");
dnl       printf ("*** to remove the old version of Ewl. You may also be able to fix the error\n");
dnl       printf("*** by modifying your LD_LIBRARY_PATH enviroment variable, or by editing\n");
dnl       printf("*** /etc/ld.so.conf. Make sure you have run ldconfig if that is\n");
dnl       printf("*** required on your system.\n");
dnl       printf("*** If ewl-config was wrong, set the environment variable EWL_CONFIG\n");
dnl       printf("*** to point to the correct copy of ewl-config, and remove the file config.cache\n");
dnl       printf("*** before re-running configure\n");
dnl     } 
dnl #if defined (EWL_MAJOR_VERSION) && defined (ECORE_MINOR_VERSION) && defined (ECORE_MICRO_VERSION)
dnl   else if ((ewl_major_version != EWL_MAJOR_VERSION) ||
dnl 	   (ewl_minor_version != EWL_MINOR_VERSION) ||
dnl            (ewl_micro_version != EWL_MICRO_VERSION))
dnl     {
dnl       printf("*** Ewl header files (version %d.%d.%d) do not match\n",
dnl 	     EWL_MAJOR_VERSION, ECORE_MINOR_VERSION, ECORE_MICRO_VERSION);
dnl       printf("*** library (version %d.%d.%d)\n",
dnl 	     ewl_major_version, ewl_minor_version, ewl_micro_version);
dnl     }
dnl #endif /* defined (EWL_MAJOR_VERSION) ... */
dnl   else
dnl     {
dnl       if ((ewl_major_version > major) ||
dnl         ((ewl_major_version == major) && (ewl_minor_version > minor)) ||
dnl         ((ewl_major_version == major) && (ewl_minor_version == minor) && (ewl_micro_version >= micro)))
dnl       {
dnl         return 0;
dnl        }
dnl      else
dnl       {
dnl         printf("\n*** An old version of Ewl (%d.%d.%d) was found.\n",
dnl                ewl_major_version, ewl_minor_version, ewl_micro_version);
dnl         printf("*** You need a version of Ewl newer than %d.%d.%d. The latest version of\n",
dnl 	       major, minor, micro);
dnl         printf("*** Ewl is always available from ftp://ftp.enlightenment.org.\n");
dnl         printf("***\n");
dnl         printf("*** If you have already installed a sufficiently new version, this error\n");
dnl         printf("*** probably means that the wrong copy of the ewl-config shell script is\n");
dnl         printf("*** being found. The easiest way to fix this is to remove the old version\n");
dnl         printf("*** of Ewl, but you can also set the EWL_CONFIG environment to point to the\n");
dnl         printf("*** correct copy of ewl-config. (In this case, you will have to\n");
dnl         printf("*** modify your LD_LIBRARY_PATH enviroment variable, or edit /etc/ld.so.conf\n");
dnl         printf("*** so that the correct libraries are found at run-time))\n");
dnl       }
dnl     }
dnl   return 1;
dnl }
dnl ],, no_ewl=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
dnl        CFLAGS="$ac_save_CFLAGS"
dnl        LIBS="$ac_save_LIBS"
dnl      fi
dnl **********************************************************************

  fi
  if test "x$no_ewl" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$EWL_CONFIG" = "no" ; then
       echo "*** The ewl-config script installed by Ewl could not be found"
       echo "*** If Ewl was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the EWL_CONFIG environment variable to the"
       echo "*** full path to ewl-config."
     else
       if test -f conf.ewltest ; then
        :
       else
          echo "*** Could not run Ewl test program, checking why..."
          CFLAGS="$CFLAGS $EWL_CFLAGS"
          LIBS="$LIBS $EWL_LIBS"
          AC_TRY_LINK([
#include <Ewl.h>
#include <stdio.h>
],      [ return ((ewl_major_version) || (ewl_minor_version) || (ewl_micro_version)); ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding Ewl or finding the wrong"
          echo "*** version of Ewl. If it is not finding Ewl, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"
          echo "***"
          echo "*** If you have a RedHat 5.0 system, you should remove the Ewl package that"
          echo "*** came with the system with the command"
          echo "***"
          echo "***    rpm --erase --nodeps ewl ewl-devel" ],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means Ewl was incorrectly installed"
          echo "*** or that you have moved Ewl since it was installed. In the latter case, you"
          echo "*** may want to edit the ewl-config script: $EWL_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     EWL_CFLAGS=""
     EWL_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(EWL_CFLAGS)
  AC_SUBST(EWL_LIBS)
  rm -f conf.ewltest
])
