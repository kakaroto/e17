dnl Process this file with autoconf to create configure.

AC_INIT(src/loader_xcf.c)
AM_INIT_AUTOMAKE(imlib2_loaders, 1.0.4)
AM_CONFIG_HEADER(config.h)

AC_C_BIGENDIAN	      

AC_LIBLTDL_CONVENIENCE
AC_CONFIG_SUBDIRS(libltdl)

AC_PROG_CC
AM_PROG_CC_STDC
AC_C_CONST

AC_PROG_INSTALL
AC_PROG_MAKE_SET
AM_PROG_LIBTOOL

AC_PATH_X
AC_PATH_XTRA

AC_ARG_ENABLE(edb,
[  --disable-edb           disable building the edb loader],
[
   if test x$enableval = xno; then
      edb=no;
      EDB_CONFIG_IN_PATH=disabled;
   fi
])

AC_ARG_ENABLE(xcf,
[  --disable-xcf           disable building the xcf loader],
[
   if test x$enableval = xno; then
      xcf=no;
   fi
])

#Set the default
IMLIB2_CONFIG="imlib2-config"
AC_ARG_WITH(imlib2,
[  --with-imlib2-config=FILE   imlib2-config script to use (eg /usr/bin/imlib2-config)],
[
  IMLIB2_CONFIG=$withval
  AC_CHECK_PROG(IMLIB2_CONFIG_IN_PATH, $IMLIB2_CONFIG, yes, no)
  if test $IMLIB2_CONFIG_IN_PATH = no; then
   echo "ERROR:"
   echo "The imlib2-config development script you specified "
   echo "($IMLIB2_CONFIG) was not found.  Please check the path "
   echo "and make sure the script exists and is executable."
   AC_MSG_ERROR([Fatal Error: no imlib2-config detected.])
   exit;
  fi
], 
[
  AC_CHECK_PROG(IMLIB2_CONFIG_IN_PATH, $IMLIB2_CONFIG, yes, no)
  if test $IMLIB2_CONFIG_IN_PATH = no; then
    echo "ERROR:"
    echo "The imlib2-config development script was not found in your execute"
    echo "path. This may mean one of several things"
    echo "1. You may not have installed the Imlib2-devel (or Imlib2-dev)"
    echo "   packages."
    echo "2. You may have Imlib2 installed somewhere not covered by your path."
    echo ""
    echo "If this is the case make sure you have the packages installed, AND"
    echo "that the imlib2-config script is in your execute path (see your"
    echo "shell's manual page on setting the \$PATH environment variable), OR"
    echo "alternatively, specify the script to use with --with-imlib2-config."
    AC_MSG_ERROR([Fatal Error: no imlib2-config detected.])
    exit;
  fi
])  

EDB_CONFIG="edb-config"
if test x$edb != xno; then
  AC_ARG_WITH(edb,
  [  --with-edb-config=PATH      edb-config script to use (eg /usr/bin/edb-config)],
  [
      EDB_CONFIG=$withval
      AC_CHECK_PROG(EDB_CONFIG_IN_PATH, $EDB_CONFIG, yes, no)
        if test $EDB_CONFIG_IN_PATH = no; then
         echo "ERROR:"
         echo "The edb-config development script you specified "
         echo "($EDB_CONFIG) was not found.  Please check the path "
         echo "and make sure the script exists and is executable."
         AC_MSG_ERROR([Fatal Error: no edb-config detected.])
         exit;
        fi
  ],
  [
     AC_CHECK_PROG(EDB_CONFIG_IN_PATH, $EDB_CONFIG, yes, no)
     if test $EDB_CONFIG_IN_PATH = no; then
       echo "WARNING:"
       echo "------------------------------------------------------------------"
       echo "The edb-config development script was not found in your execute"
       echo "path. This may mean one of several things"
       echo "1. You may not have installed the edb-devel (or edb-dev)"
       echo "   packages."
       echo "2. You may have edb installed somewhere not covered by your path."
       echo ""
       echo "If this is the case make sure you have the packages installed, AND"
       echo "that the edb-config script is in your execute path (see your"
       echo "shell's manual page on setting the \$PATH environment variable), OR"
       echo "alternatively, specify the script to use with --with-edb-config."
       echo "------------------------------------------------------------------"
       AC_MSG_WARN([no edb-config detected.])
     fi
  ])
else
  AC_MSG_CHECKING([for edb-config])
  AC_MSG_RESULT(disabled)
fi

AM_CONDITIONAL(EDB_LOADER, 
            test \( $EDB_CONFIG_IN_PATH = yes -o \
                    $EDB_CONFIG_IN_PATH = ignored \) -a x$edb != xno )

AM_CONDITIONAL(XCF_LOADER, test x$xcf != xno )

imlib2_includes=`$IMLIB2_CONFIG --cflags`
imlib2_libs=`$IMLIB2_CONFIG --libs`

if test x$edb != xno; then
   edb_includes=`$EDB_CONFIG --cflags`
   edb_libs=`$EDB_CONFIG --libs`
fi

AC_SUBST(imlib2_includes)
AC_SUBST(imlib2_libs)
AC_SUBST(edb_includes)
AC_SUBST(edb_libs)

AC_OUTPUT([
Makefile
src/Makefile
debian/Makefile
imlib2_loaders.spec
])

echo -e "\nBuild summary:"
echo    "------------------------"
echo -n " XCF: "
if test x$XCF_LOADER_TRUE = "x#"; then
   echo    "Disabled "
else
   echo    "Enabled "
fi   

echo -n " Edb: "
if test x$EDB_LOADER_TRUE = "x#"; then
   echo    "Disabled "
else
   echo    "Enabled "
fi   

echo -e "------------------------\n"
