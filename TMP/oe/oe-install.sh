#!/bin/sh -e

if test -d bitbake; then
  cd bitbake
  svn update || true
  cd ..
else
  svn co svn://svn.berlios.de/bitbake/branches/bitbake-1.8/ bitbake
fi
if test ! -d build/conf; then
  mkdir -p build/conf
fi
if test -d org.openembedded.dev; then
  cd org.openembedded.dev
  git-fetch || true
  git-rebase origin || true
  cd ..
else
  git clone git://git.openembedded.net/openembedded org.openembedded.dev
  cd org.openembedded.dev
  git-checkout -b $USER
  cd ..
fi
if test ! -e build/conf/local.conf; then
  ln -sf ../../local.conf                                           build/conf/local.conf
fi
if test ! -e packages; then
  ln -sf org.openembedded.dev/packages                              packages
fi
if test ! -e sane-srcrevs.inc; then
  ln -sf org.openembedded.dev/conf/distro/include/sane-srcrevs.inc  sane-srcrevs.inc
fi
if test ! -e sane-srcdates.inc; then
  ln -sf org.openembedded.dev/conf/distro/include/sane-srcdates.inc sane-srcdates.inc
fi
if test ! -e org.openembedded.dev/packages/my_packages; then
  ln -sf ../../my_packages                                          org.openembedded.dev/packages/my_packages
fi
./oe-patches.sh || true

echo "========="
echo "Please edit local.conf to specify your build paramaters and target"
echo "Put any custom packages .bb files in my_packages"
echo "Before you do any building do:"
echo "  source ./setup-env"
echo
echo "Then for example:"
echo "  bitbake illume-image"
echo
echo "To update git:"
echo "  git-fetch; git-rebase origin"
# org.openembedded.dev
#echo "  git-fetch; git-rebase origin/master"
echo
echo "To commit any changes you have made to local git:"
echo "  git-commit -a"
echo
echo "To generate a patch set from your local commits:"
echo "  git-format-patch origin"
echo
echo "To modify SVN revision #'s used for packages or CVS dates edit"
echo "sane-srcrevs.inc or sane-srcdates.inc"
echo "Output image swill go into tmp.*/deploy/glibc/images/"
echo "Output packages files will go into tmp.*/deploy/glibc/ipk/"
echo "Input packages .bb files from OE will be in packages/"
echo "Any of your own packages put in my_packages"

exit 0
