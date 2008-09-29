#!/bin/sh

mkdir -p build/conf
svn co svn://svn.berlios.de/bitbake/branches/bitbake-1.8/ bitbake
git clone git://git.openembedded.net/org.openembedded.dev.git
cd org.openembedded.dev
git-checkout -b $USER
cd ..
ln -s ../../local.conf              build/conf/local.conf
ln -s org.openembedded.dev/packages packages
ln -s org.openembedded.dev/conf/distro/include/sane-srcrevs.inc sane-srcrevs.inc
ln -s org.openembedded.dev/conf/distro/include/sane-srcdates.inc sane-srcdates.inc

mkdir my_packages
ln -s ../../my_packages org.openembedded.dev/packages/my_packages

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
