#!/bin/bash

BLACK='\E[30;40m'
GREEN='\E[32;40m'
RED='\E[31;40m'
BLUE='\E[34;40m'

# $1 -> message to be printed
# $2 -> color escape sequence to be used
cecho() {
    echo -ne "$2"
    echo "$1"
    tput sgr0
}

print_begin() {
    echo -n "BUILDING $1 : "
}

print_success() {
    cecho "DONE" $GREEN
    LOG=
}

print_fail() {
    cecho "FAIL" $RED
}

die() {
    print_fail
    echo "  ! $*"
    if [ ! -z "$LOG" ]; then
        echo "  See $LOG for details"
    fi
    exit 1
}

get_rev() {
    echo -n "Checking revision available : "
    if [ -z $SVNREV ]; then
        SVNREV=`LC_ALL=C svn info $SVNREP | grep 'Revision' | cut -d' ' -f2`
        if [ -z $SVNREV ]; then
            SVNREV="HEAD"
        fi
    fi
    cecho $SVNREV $BLUE
}

create_srcdir() {
    if [ ! -z "$CHKTflag" ]; then
        echo -n "Removing old source directory: "
        rm -rf $SRCDIR
        print_success
    fi
    if [ ! -d $SRCDIR ]; then
        echo -n "Creating new source directory: "
        mkdir -p $SRCDIR &> /dev/null || die "Could not create source dir $SRCDIR"
        print_success
    fi
}

filter_oldenv() {
    echo $1 | sed -r 's#/opt/e17-[^:]+[:]?##g'
}

create_envfile() {
    echo -n "Creatind ENVVARS file : "
    ENVFILE=$PREFIX"/e17-env.sh"
    mkdir -p $PREFIX
    cat >> $ENVFILE << EOF
export PATH="/opt/e17-$SVNREV/bin:\$PATH"
export PKG_CONFIG_PATH="/opt/e17-$SVNREV/lib/pkgconfig:\$PKG_CONFIG_PATH"
export LD_LIBRARY_PATH="/opt/e17-$SVNREV/lib:\$LD_LIBRARY_PATH"
EOF
    export PATH=`filter_oldenv $PATH`
    export PKG_CONFIG_PATH=`filter_oldenv $PKG_CONFIG_PATH`
    export LD_LIBRARY_PATH=`filter_oldenv $LD_LIBRARY_PATH`
    print_success
}

find_distro() {
    echo -n "Finding out Linux distro : "
    if [ -e "/etc/debian_version" ]; then DISTRO="debian"; fi
    if [ -e "/etc/redhat-release" ]; then DISTRO="redhat"; fi
    if [ -e "/etc/arch-release" ]; then DISTRO="arch"; fi
    cecho $DISTRO $BLUE
}

get_deps() {
    find_distro
    echo -n "Installing dependencies : "
    LOG=$OUTPUT/dependencies.log
    case $DISTRO in
        debian)
            apt-get install -qy --no-install-recommends --force-yes subversion build-essential automake autopoint cvs libtool zlib1g-dev libjpeg62-dev gettext xorg-dev libpng12-dev libdbus-1-dev liblua5.1-0 liblua5.1-0-dev libexif-dev &> $LOG || die "Could not install dependencies"
            ;;
        redhat)
            yum -y install zlib-devel libjpeg-turbo-devel freetype-devel lua-devel libpng-devel dbus-devel libexif-devel libudev-devel subversion cvs openssl-devel gnutls-devel alsa-lib-devel &> $LOG || die "Could not install dependencies"
            yum -y groupinstall "X Software Development" "Development Tools" &> $LOG || die "Could not install dependencies"
            ;;
        arch)
            pacman -Sq --needed --noconfirm freetype2 libjpeg libpng libxml2 subversion xorg-server lua dbus-core openssl gnutls libxp curl libxss libxtst alsa-lib cvs libexif zlib gettext base-devel &> $LOG || die "Could not install dependencies"
            ;;
        *)
            LOG=
            die "Distro $DISTRO not supported"
    esac
    print_success
}

get_source() {
    PKG=`basename $1`
    if [ -d $1 ]; then
        echo -n "UPDATING $PKG : "
        LOG=$OUTPUT/$1/output-up.log
        svn up --accept theirs-full --revision $SVNREV "$SRCDIR/$1" &> $LOG || die "Failed updating source for $PKG"
    else
        echo -n "DOWNLOADING $PKG : "
        LOG=$OUTPUT/$1/output-checkout.log
        svn checkout --revision $SVNREV $SVNREP/$1 $SRCDIR/$1 &> $LOG || die "Failed fetching source for $PKG"
    fi
    print_success
}

fix_permissions() {
    echo -n "Fixing permissions : "
    for i in $EFL_SUID_BIN; do
        chown root:root $i
        chmod +s $i
    done
    print_success
}

build() {
    PKG=`basename $1`
    print_begin $PKG
    pushd $1 &> /dev/null || die "Could not enter $1"

    if [ -f Makefile ]; then
        make distclean &> /dev/null
    fi

    FLAGS=`eval echo \\$${PKG}_CFLAGS`
    if [ -z $FLAGS ]; then
        FLAGS=$CFLAGS
    fi

    OPTS=`eval echo \\$${PKG}_OPTIONS`
    LOG=$OUTPUT/$1/output-autogen.log
    CFLAGS=$FLAGS ./autogen.sh --prefix=$PREFIX $OPTS --disable-amalgamation &> $LOG || die "Failed configuring $PKG"
    LOG=$OUTPUT/$1/output-make.log
    make $MAKEOPTIONS &> $LOG || die "Failed building $PKG"
    LOG=$OUTPUT/$1/output-install.log
    make install &> $LOG || die "Failed installing $PKG"
    LOG=
    if [ $PKG = "e" ]; then
        install -Dm644 $SRCDIR/e/data/xsession/enlightenment.desktop /usr/share/xsessions/ &> /dev/null || die "Could not install enlightenment.destop file"
    fi
    make clean &> /dev/null
    make distclean &> /dev/null
    print_success
    popd &> /dev/null
}

build_efl() {
    for i in $PKGS; do
        mkdir -p $OUTPUT/$i &> /dev/null
        get_source $i
        build $i
    done
    fix_permissions
}

print_usage() {
    printf "Usage: %s: [OPTIONS]\n" $0
cat << EOF
Compile EFL source code from svn.

Available options:
  -f            compile from scratch (checking out new source code)
  -r <REV>      compile from the specified svn revision
  -h            print this help
EOF
}

PKGS="eina eet embryo evas ecore efreet edje e_dbus ethumb TMP/st/elementary e"

# Repository details
SVNREP="http://svn.enlightenment.org/svn/e/trunk"
SRCDIR="/var/cache/e17_src/trunk"
OUTPUT="/tmp/e17_compile"

# Detect whether script is being run with root
if [ `whoami` != root ]; then
    die "Please run this script as root or using sudo"
fi

SVNREV=
CHKTflag=
while getopts "hfr:" optname
do
    case $optname in
        f)
            CHKTflag=1
            ;;
        r)
            SVNREV=$OPTARG
            ;;
        h|?)
            print_usage
            ;;
        *)
            echo "Unknown error while processing options"
            ;;
    esac
done

mkdir $OUTPUT &> /dev/null
get_deps
get_rev

PREFIX="/opt/e17-$SVNREV"
DBUSPREFIX="/usr/share/dbus-1/services"
CFLAGS="-fvisibility=hidden -O2 $CFLAGS -I$PREFIX/include -I$PREFIX/include"
CPPFLAGS="$CFLAGS $CPPFLAGS"
LDFLAGS="-fvisibility=hidden $LDFLAGS -L$PREFIX/lib -L$PREFIX/lib"
MAKEOPTIONS="-j3"

EFL_SUID_BIN="$PREFIX/lib/enlightenment/utils/enlightenment_sys $PREFIX/lib/enlightenment/modules/cpufreq/linux-gnu-*-ver-*/freqset"

# Specific configure options can be passed using the <pkgname>_OPTIONS variable.
# The same notation is valid for CFLAGS if you need specific flags.
ethumb_OPTIONS="--with-dbus-services=$DBUSPREFIX"

ENVFILE=
LOG=

create_srcdir
pushd $SRCDIR &> /dev/null

create_envfile
source $PREFIX"/e17-env.sh"
build_efl

# Point to last env file
rm -f /opt/e17-env.sh
ln -s $PREFIX"/e17-env.sh" /opt/e17-env.sh

cat << EOF

EFL revision $SVNREV was compiled with success!

Note that to use that specific revision you should export the right environment
variables available at $ENVFILE or your previously compiled EFL
will be used instead. For the last successfully compiled one use the
/opt/e17-env.sh script.

The sources can be found at the $SRCDIR directory.

Enjoy!

EOF

popd &> /dev/null
