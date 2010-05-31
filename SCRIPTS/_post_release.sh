#!/bin/sh

set -e

PROJ=$1
cd $PROJ
svn commit -m "Restore svnrev post-release"

exit 0
