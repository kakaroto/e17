#!/bin/sh

# for now link ewl's ewldbtool to this dir or something ;)

cp -rf $HOME/.ewl/themes/defaultTheme $HOME/.ewl/themes/elogin
cp -rf ./images/*.png $HOME/.ewl/themes/elogin/images

###############################
#   ABSTRACT PARENT CLASSES   #
###############################

# create the default EwlWidget db
DB=$HOME/.ewl/themes/elogin/EwlWidget
./ewldbtool $DB  "padding/left"   "0"
./ewldbtool $DB  "padding/top"    "0"
./ewldbtool $DB  "padding/right"  "0"
./ewldbtool $DB  "padding/bottom" "0"

# create the default EwlContainer db
DB=$HOME/.ewl/themes/elogin/EwlContainer
./ewldbtool $DB  "padding/left"   "0"
./ewldbtool $DB  "padding/top"    "0"
./ewldbtool $DB  "padding/right"  "0"
./ewldbtool $DB  "padding/bottom" "0"

######################
#   EWL CONTAINERS   #
######################

###  EWL BOXES ###
# create the default EwlBox db
DB=$HOME/.ewl/themes/elogin/EwlBox
./ewldbtool $DB  "padding/left"   "0"
./ewldbtool $DB  "padding/top"    "0"
./ewldbtool $DB  "padding/right"  "0"
./ewldbtool $DB  "padding/bottom" "0"

# create the default EwlHBox db
DB=$HOME/.ewl/themes/elogin/EwlHBox
./ewldbtool $DB  "padding/left"   "0"
./ewldbtool $DB  "padding/top"    "0"
./ewldbtool $DB  "padding/right"  "0"
./ewldbtool $DB  "padding/bottom" "0"

# create the default EwlVBox db
DB=$HOME/.ewl/themes/elogin/EwlVBox
./ewldbtool $DB  "padding/left"   "0"
./ewldbtool $DB  "padding/top"    "0"
./ewldbtool $DB  "padding/right"  "0"
./ewldbtool $DB  "padding/bottom" "0"

# create the default EwlLBox db
DB=$HOME/.ewl/themes/elogin/EwlLBox
./ewldbtool $DB  "padding/left"   "0"
./ewldbtool $DB  "padding/top"    "0"
./ewldbtool $DB  "padding/right"  "0"
./ewldbtool $DB  "padding/bottom" "0"

### NON-BOX CONTAINERS ###
# create the default EwlWindow db
DB=$HOME/.ewl/themes/elogin/EwlWindow
./ewldbtool $DB "padding/left"   "0"
./ewldbtool $DB "padding/top"    "0"
./ewldbtool $DB "padding/right"  "0"
./ewldbtool $DB "padding/bottom" "0"
./ewldbtool $DB "background"     "images/box2.png"
./ewldbtool $DB "child_padding/left"   "0"
./ewldbtool $DB "child_padding/top"    "0"
./ewldbtool $DB "child_padding/right"  "0"
./ewldbtool $DB "child_padding/bottom" "0"
#./ewldbtool $DB "num_layers"     "1"
#./ewldbtool $DB "layer-00/name"        "eLogin Logo"
#./ewldbtool $DB "layer-00/width"       "181"
#./ewldbtool $DB "layer-00/height"      "132"
#./ewldbtool $DB "layer-00/alpha"       "true"
#./ewldbtool $DB "layer-00/visible"     "true"
#./ewldbtool $DB "layer-00/num_images"  "1"
#./ewldbtool $DB "layer-00/image-00"         "images/elogin.png"
#./ewldbtool $DB "layer-00/image-00/visible" "true"

# create the default EwlButton db
DB=$HOME/.ewl/themes/elogin/EwlButton
./ewldbtool $DB "padding/left"   "0"
./ewldbtool $DB "padding/top"    "0"
./ewldbtool $DB "padding/right"  "0"
./ewldbtool $DB "padding/bottom" "0"
./ewldbtool $DB "background"     "images/aqua_button.png"

#####################
#   OTHER WIDGETS   #
#####################

# create the default EwlLabel db
DB=$HOME/.ewl/themes/elogin/EwlLabel
./ewldbtool $DB "padding/left"   "0"
./ewldbtool $DB "padding/top"    "0"
./ewldbtool $DB "padding/right"  "0"
./ewldbtool $DB "padding/bottom" "0"
