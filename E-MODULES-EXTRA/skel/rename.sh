#!/bin/bash

# short name
MODULE=my_module
# visible name
MODULE_NAME=MyModule
#your name
NAME1=Hannes 
NAME2=Janetzek
#email
EMAIL=hannes.janetzek@gmail.com

find . -type f -exec sed -i 's/Christopher Michael/'$NAME1' '$NAME2'/g' {} \;
find . -type f -exec sed -i 's/devilhorns@comcast.net/'$EMAIL'/g' {} \;
find . -type f -exec sed -i 's/skel/'$MODULE'/g' {} \;
find . -type f -exec sed -i 's/Skeleton/'$MODULE_NAME'/g' {} \;

mv e-module-skel.edc e-module-$MODULE.edc
mv e_modules-skel.spec.in e_modules-$MODULE.spec.in
rm po/skel.pot