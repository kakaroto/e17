#!/bin/sh

# for now link ewl's ewldbtool to this dir or something ;)
EWLPREFIX=/usr/local

rm -rf $HOME/.ewl/themes/elogin
mkdir -p $HOME/.ewl/themes/elogin/images
cp -rf ./images/*.png $HOME/.ewl/themes/elogin/images

###############################
#   ABSTRACT PARENT CLASSES   #
###############################

# create the default EwlWidget db
DB=$HOME/.ewl/themes/elogin/EwlWidget
$EWLPREFIX/bin/ewldbtool $DB  "padding/left"   "0"
$EWLPREFIX/bin/ewldbtool $DB  "padding/top"    "0"
$EWLPREFIX/bin/ewldbtool $DB  "padding/right"  "0"
$EWLPREFIX/bin/ewldbtool $DB  "padding/bottom" "0"

# create the default EwlContainer db
DB=$HOME/.ewl/themes/elogin/EwlContainer
$EWLPREFIX/bin/ewldbtool $DB  "padding/left"   "0"
$EWLPREFIX/bin/ewldbtool $DB  "padding/top"    "0"
$EWLPREFIX/bin/ewldbtool $DB  "padding/right"  "0"
$EWLPREFIX/bin/ewldbtool $DB  "padding/bottom" "0"

######################
#   EWL CONTAINERS   #
######################

###  EWL BOXES ###
# create the default EwlBox db
DB=$HOME/.ewl/themes/elogin/EwlBox
$EWLPREFIX/bin/ewldbtool $DB  "padding/left"   "0"
$EWLPREFIX/bin/ewldbtool $DB  "padding/top"    "0"
$EWLPREFIX/bin/ewldbtool $DB  "padding/right"  "0"
$EWLPREFIX/bin/ewldbtool $DB  "padding/bottom" "0"

# create the default EwlHBox db
DB=$HOME/.ewl/themes/elogin/EwlHBox
$EWLPREFIX/bin/ewldbtool $DB  "padding/left"   "0"
$EWLPREFIX/bin/ewldbtool $DB  "padding/top"    "0"
$EWLPREFIX/bin/ewldbtool $DB  "padding/right"  "0"
$EWLPREFIX/bin/ewldbtool $DB  "padding/bottom" "0"

# create the default EwlVBox db
DB=$HOME/.ewl/themes/elogin/EwlVBox
$EWLPREFIX/bin/ewldbtool $DB  "padding/left"   "0"
$EWLPREFIX/bin/ewldbtool $DB  "padding/top"    "0"
$EWLPREFIX/bin/ewldbtool $DB  "padding/right"  "0"
$EWLPREFIX/bin/ewldbtool $DB  "padding/bottom" "0"

# create the default EwlLBox db
DB=$HOME/.ewl/themes/elogin/EwlLBox
$EWLPREFIX/bin/ewldbtool $DB  "padding/left"   "0"
$EWLPREFIX/bin/ewldbtool $DB  "padding/top"    "0"
$EWLPREFIX/bin/ewldbtool $DB  "padding/right"  "0"
$EWLPREFIX/bin/ewldbtool $DB  "padding/bottom" "0"

### NON-BOX CONTAINERS ###
# create the default EwlWindow db
DB=$HOME/.ewl/themes/elogin/EwlWindow
$EWLPREFIX/bin/ewldbtool $DB "padding/left"   "0"
$EWLPREFIX/bin/ewldbtool $DB "padding/top"    "0"
$EWLPREFIX/bin/ewldbtool $DB "padding/right"  "0"
$EWLPREFIX/bin/ewldbtool $DB "padding/bottom" "0"
$EWLPREFIX/bin/ewldbtool $DB "background"     "images/box2.png"
$EWLPREFIX/bin/ewldbtool $DB "child_padding/left"   "0"
$EWLPREFIX/bin/ewldbtool $DB "child_padding/top"    "0"
$EWLPREFIX/bin/ewldbtool $DB "child_padding/right"  "0"
$EWLPREFIX/bin/ewldbtool $DB "child_padding/bottom" "0"
#$EWLPREFIX/bin/ewldbtool $DB "num_layers"     "1"
#$EWLPREFIX/bin/ewldbtool $DB "layer-00/name"        "eLogin Logo"
#$EWLPREFIX/bin/ewldbtool $DB "layer-00/width"       "181"
#$EWLPREFIX/bin/ewldbtool $DB "layer-00/height"      "132"
#$EWLPREFIX/bin/ewldbtool $DB "layer-00/alpha"       "true"
#$EWLPREFIX/bin/ewldbtool $DB "layer-00/visible"     "true"
#$EWLPREFIX/bin/ewldbtool $DB "layer-00/num_images"  "1"
#$EWLPREFIX/bin/ewldbtool $DB "layer-00/image-00"         "images/elogin.png"
#$EWLPREFIX/bin/ewldbtool $DB "layer-00/image-00/visible" "true"

# create the default EwlButton db
DB=$HOME/.ewl/themes/elogin/EwlButton
$EWLPREFIX/bin/ewldbtool $DB "padding/left"   "0"
$EWLPREFIX/bin/ewldbtool $DB "padding/top"    "0"
$EWLPREFIX/bin/ewldbtool $DB "padding/right"  "0"
$EWLPREFIX/bin/ewldbtool $DB "padding/bottom" "0"
$EWLPREFIX/bin/ewldbtool $DB "background"     "images/aqua_button.png"

#####################
#   OTHER WIDGETS   #
#####################

# create the default EwlLabel db
DB=$HOME/.ewl/themes/elogin/EwlLabel
$EWLPREFIX/bin/ewldbtool $DB "padding/left"   "0"
$EWLPREFIX/bin/ewldbtool $DB "padding/top"    "0"
$EWLPREFIX/bin/ewldbtool $DB "padding/right"  "0"
$EWLPREFIX/bin/ewldbtool $DB "padding/bottom" "0"
