#!/bin/bash
# 
# Copyright (C) 2006-2012 Jerome Pinot <ngc891@gmail.com>
#
# Begin slacke17-svn-build.sh

CWD=$(pwd)
TMP=${TMP:-/tmp}
E17=${TMP}/slacke17-svn/
LOG=${E17}/logs

RELEASE=${RELEASE:-0}

# check if we are in the right folder
if [ ! -f ${CWD}/slacke17-svn-build.sh ]; then
  echo "**Error: I can't find the build script. Are you in the right folder?"
  exit 1
fi

# we have to install the packages one by one: root is required.
if [ ! `id -u` -eq 0 ] ; then 
  echo "**Error: We're going to INSTALL, so you have to be ROOT"
  exit 1
fi

# create the main directory
mkdir -p ${E17}/logs

# go
for package in `cat ${CWD}/PACKAGES`; do

  cd ${CWD}/${package}
  echo "##############################################################################"
  echo "# Building ${package}..."
  echo "##############################################################################"
  ./${package}.SlackBuild 2>&1 | tee ${LOG}/${package}.log

  # install/upgrade the package
  upgradepkg --install-new ${TMP}/${package}-*-*-*.txz
  ldconfig
  cp slack-desc ${TMP}/`basename /${TMP}/${package}-*-*-*.txz txz`txt
  mv ${TMP}/${package}-*-*-*.tx? ${E17}/

done

echo "##############################################################################"
echo "# Building done!"
echo "##############################################################################"

# End slacke17-svn-build.sh
