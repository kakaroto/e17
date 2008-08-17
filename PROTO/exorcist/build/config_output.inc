#############
#
# pc script file
#
#############

lib_pc_create() {
    rm -f $PACKAGE.pc
    cat > $PACKAGE.pc << EOF
prefix=$prefix
exec_prefix=$exec_prefix
libdir=$libdir
includedir=$includedir

Name: $PACKAGE
Description: $1
Version: VERSION
Libs: -L$libdir $2
Cflags: -I$includedir
EOF
}

#############
#
# config script file
#
#############

lib_config_create() {
    rm -f $PACKAGE-config
    cat > $PACKAGE-config << EOF
#!/bin/sh

prefix=$prefix
exec_prefix=$exec_prefix
exec_prefix_set=no

usage="\\
Usage: $PACKAGE-config [--prefix[=DIR]] [--exec-prefix[=DIR]] [--version] [--libs] [--cflags]"

if test \$# -eq 0; then
      echo "\${usage}" 1>&2
      exit 1
fi

while test \$# -gt 0; do
  case "\$1" in
  -*=*) optarg=\`echo "\$1" | sed 's/[-_a-zA-Z0-9]*=//'\` ;;
  *) optarg= ;;
  esac

  case \$1 in
    --prefix=*)
      prefix=\$optarg
      if test \$exec_prefix_set = no ; then
        exec_prefix=\$optarg
      fi
      ;;
    --prefix)
      echo \$prefix
      ;;
    --exec-prefix=*)
      exec_prefix=\$optarg
      exec_prefix_set=yes
      ;;
    --exec-prefix)
      echo \$exec_prefix
      ;;
    --version)
      echo $VERSION
      ;;
    --cflags)
      if test $includedir != /usr/include ; then
        includes=-I$includedir
      fi
      echo \$includes
      ;;
    --libs)
      libdirs=-L$libdir
      echo \$libdirs $1
      ;;
    *)
      echo "\${usage}" 1>&2
      exit 1
      ;;
  esac
  shift
done

exit 0
EOF
}
