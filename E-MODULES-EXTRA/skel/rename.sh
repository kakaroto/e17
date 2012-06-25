#!/bin/bash

# short name
MODULE=my_module
# used in extra parts of the code
# suggestion: just caseup the first char of your $MODULE
MODULE_UP=My_module
# visible name
MODULE_NAME=MyModule
#your name
NAME1=Hannes 
NAME2=Janetzek
#email
EMAIL=hannes.janetzek@gmail.com


find . -type f ! -name rename.sh -exec sed -i 's/Christopher Michael/'$NAME1' '$NAME2'/g' {} \;
find . -type f ! -name rename.sh -exec sed -i 's/devilhorns@comcast.net/'$EMAIL'/g' {} \;
find . -type f ! -name rename.sh -exec sed -i 's/Skeleton/'$MODULE_NAME'/g' {} \;
find . -type f ! -name rename.sh -exec sed -i 's/Skel/'$MODULE_UP'/g' {} \;
find . -type f ! -name rename.sh -exec sed -i 's/skel/'$MODULE'/g' {} \;

mv e-module-skel.edc e-module-$MODULE.edc
mv e_modules-skel.spec.in e_modules-$MODULE.spec.in
rm -f po/skel.pot
