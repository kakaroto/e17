#!/bin/sh

set -e

PACKAGE_LIBS="eina eet evas ecore embryo edje efreet e_dbus evas_generic_loaders"
PACKAGE_AUXLIBS="eio eeze emotion PROTO/epdf ethumb PROTO/libeweather elementary"
PACKAGE_SOFTS="e PROTO/elsa espionnage PROTO/ekbd"
PACKAGE_E_MODULES="E-MODULE-EXTRA/e-tilling"
PACKAGE_TESTS="expedite"
PACKAGE_TO_COMPIL="$PACKAGE_LIBS $PACKAGE_AUXLIBS $PACKAGE_SOFTS $PACKAGE_TESTS $PACKAGE_E_MODULES"
MAKE_OPTS="-j3"
MAKECMD="make"
SVNUP="svn up"
MY_USER="yoz"
CFLAGS="-W -Wall -Wmissing-declarations -Wshadow -Wmissing-declarations"
LOG_FILE="> /dev/null"
#LOG_FILE=""

#les couleurs utiliser dans le script
BLANC=`tput setaf 9`
ROUGE=`tput setaf 1`
BLEU=`tput setaf 4`
VERT=`tput setaf 2`

package_compil() {
   echo -n "compiling"
   for i in $@
   do
      echo " $i"
      if [ -d $i ]
      then
         cd $i
         su $MY_USER -c "$MAKECMD $MAKE_OPTS " > /dev/null
         cd - > /dev/null 2>&1
      fi
   done
   echo
}

package_maintainerclean() {
   echo -n "cleaning"
   for i in $@
   do
      echo -n " $i"
      $MAKECMD maintainer-clean -C $i > /dev/null || echo "Nothing to done !"
   done
   echo
}

package_clean() {
   echo -n "cleaning"
   for i in $@
   do
      echo -n " $i"
      $MAKECMD clean -C $i > /dev/null || echo "Already cleaned !"
   done
   echo
}

package_uninstall() {
   echo -n "uninstalling"
   for i in $@
   do
      echo -n " $i"
      $MAKECMD uninstall -C $i > /dev/null || echo "Error on uninstalling $i"
   done
   echo
}

package_install() {
   echo -n "installing"
   for i in $@
   do
      echo " $i"
      $MAKECMD install -C $i > /dev/null
   done
   echo
}

package_configure() {
   echo -n "configuring"
   for i in $@
   do
      echo " $i"
      cd $i
      su $MY_USER -c "./autogen.sh --prefix=/usr --sysconfdir=/etc --disable-doc --disable-install-examples --disable-cpu-sse3" > /dev/null
      cd - > /dev/null 2>&1
   done
   echo
}

package_update() {
   echo -n "updating"
   for i in $@
   do
      cd $i
      echo " $i"
      su $MY_USER -c "$SVNUP"
      cd - > /dev/null 2>&1
   done
   echo

}

package_compil_install() {
   for i in $@
   do
      package_configure $i
      package_compil $i
      package_install $i
   done
}

package_fast_compil_install() {
   for i in $@
   do
      package_compil $i
      package_install $i
   done
}

onexit() {
    local exit_status=${1:-$?}
    echo Exiting Cleanly $0 with $exit_status
    exit $exit_status
}


print_usage() {
   echo "Usage: ${0} <action> {PROJET}"
   echo "${BLEU}ACTION${BLANC}:"
   echo " ${VERT}update${BLANC}: met à jour les efl (uninstall clean update compil install)"
   echo " ${VERT}clean${BLANC}: clean les efl (maintainerclean)"
   echo " ${VERT}fast_update${BLANC}: met à jour les efl (update configure compil install)"
   echo " ${VERT}really fast_update${BLANC}: met à jour les efl (update compil install)"
   echo " ${VERT}uninstall${BLANC}: clean les efl (uninstall)"
   echo " ${VERT}install${BLANC}: install les efl"
   echo " ${VERT}tarball${BLANC}: fais une tarball des efl"
}

trap onexit 1 2 3 15 ERR

if [ -z $1 ]
then
   print_usage
   onexit
fi

ACTION=$1
shift;

if [ "a" != "a"$1 ]
then
   PACKAGE_TO_COMPIL=$1
fi

 case $ACTION in
         update)
                 package_uninstall $PACKAGE_TO_COMPIL
                 package_clean $PACKAGE_TO_COMPIL
                 package_update $PACKAGE_TO_COMPIL
                 package_compil_install $PACKAGE_TO_COMPIL
                 ;;
         uninstall)
                 package_uninstall $PACKAGE_TO_COMPIL
                 ;;
         install)
                 package_compil_install $PACKAGE_TO_COMPIL
                 ;;
         fast_update)
                 package_update $PACKAGE_TO_COMPIL
                 package_compil_install $PACKAGE_TO_COMPIL
                 ;;
         really_fast_update)
                 package_update $PACKAGE_TO_COMPIL
                 package_fast_compil_install $PACKAGE_TO_COMPIL
                 ;;
         clean)
                 package_maintainerclean $PACKAGE_TO_COMPIL
                 ;;
         tarball)
                 package_update $PACKAGE_TO_COMPIL
                 package_maintainerclean $PACKAGE_TO_COMPIL
                 tar -cjf efl.tar.bz2 $PACKAGE_TO_COMPIL
                 ;;
         *)
                 print_usage;
                 ;;
esac
onexit
