#!/bin/sh

#######################################################################
# Create a user's ~/.ewl directory the first time -- this also        #
# generates the default theme, although eventually I'm probably going #
# to just make it copy pre-generated files.                           #
#                                                                     #
# - Paul                                                              #
#######################################################################

# create the direcories if they don't exist
mkdir $HOME/.ewl
mkdir $HOME/.ewl/personal
mkdir $HOME/.ewl/themes

# create the default preferences db
./ewldbtool $HOME/.ewl/preferences "path" $HOME"/.ewl/personal:"$HOME"/.ewl/themes:/usr/local/share/ewl/themes:/usr/local/share/ewl/default"
./ewldbtool $HOME/.ewl/preferences "theme" "defaultTheme"
./ewldbtool $HOME/.ewl/preferences "render/dithered" "false"
./ewldbtool $HOME/.ewl/preferences "render/antialiased" "false"
./ewldbtool $HOME/.ewl/preferences "render/method" "alpha_software"


# create a defaultTheme directory
cp -rf ./themes/defaultTheme $HOME/.ewl/themes

###$##################
#   PARENT CLASSES   #
##$###################

# create the default EwlWidget db
DB=$HOME/.ewl/themes/defaultTheme/EwlWidget
./ewldbtool $DB  "padding/left"   "0"
./ewldbtool $DB  "padding/top"    "0"
./ewldbtool $DB  "padding/right"  "0"
./ewldbtool $DB  "padding/bottom" "0"

# create the default EwlContainer db
DB=$HOME/.ewl/themes/defaultTheme/EwlContainer
./ewldbtool $DB  "padding/left"   "0"
./ewldbtool $DB  "padding/top"    "0"
./ewldbtool $DB  "padding/right"  "0"
./ewldbtool $DB  "padding/bottom" "0"

######################
#   EWL CONTAINERS   #
######################

###  EWL BOXES ###
# create the default EwlBox db
DB=$HOME/.ewl/themes/defaultTheme/EwlBox
./ewldbtool $DB  "padding/left"   "0"
./ewldbtool $DB  "padding/top"    "0"
./ewldbtool $DB  "padding/right"  "0"
./ewldbtool $DB  "padding/bottom" "0"

# create the default EwlHBox db
DB=$HOME/.ewl/themes/defaultTheme/EwlHBox
./ewldbtool $DB  "padding/left"   "0"
./ewldbtool $DB  "padding/top"    "0"
./ewldbtool $DB  "padding/right"  "0"
./ewldbtool $DB  "padding/bottom" "0"

# create the default EwlVBox db
DB=$HOME/.ewl/themes/defaultTheme/EwlVBox
./ewldbtool $DB  "padding/left"   "0"
./ewldbtool $DB  "padding/top"    "0"
./ewldbtool $DB  "padding/right"  "0"
./ewldbtool $DB  "padding/bottom" "0"

# create the default EwlLBox db
DB=$HOME/.ewl/themes/defaultTheme/EwlLBox
./ewldbtool $DB  "padding/left"   "0"
./ewldbtool $DB  "padding/top"    "0"
./ewldbtool $DB  "padding/right"  "0"
./ewldbtool $DB  "padding/bottom" "0"

### NON-BOX CONTAINERS ###
# create the default EwlWindow db
DB=$HOME/.ewl/themes/defaultTheme/EwlWindow
./ewldbtool $DB "padding/left"   "0"
./ewldbtool $DB "padding/top"    "0"
./ewldbtool $DB "padding/right"  "0"
./ewldbtool $DB "padding/bottom" "0"
#./ewldbtool $DB "background"     "images/bg.png"
./ewldbtool $DB "background"     "images/ribbed_bg.png"
./ewldbtool $DB "child_padding/left"   "5"
./ewldbtool $DB "child_padding/top"    "5"
./ewldbtool $DB "child_padding/right"  "5"
./ewldbtool $DB "child_padding/bottom" "5"
#./ewldbtool $DB "num_layers"     "1"
./ewldbtool $DB "num_layers"     "0"
./ewldbtool $DB "layer-00/name"        "Sample ImLayer"
./ewldbtool $DB "layer-00/width"       "320"
./ewldbtool $DB "layer-00/height"      "240"
./ewldbtool $DB "layer-00/alpha"       "false"
./ewldbtool $DB "layer-00/visible"     "true"
./ewldbtool $DB "layer-00/num_images"  "1"
./ewldbtool $DB "layer-00/image-00"         "images/aqua_button.png"
./ewldbtool $DB "layer-00/image-00/visible" "true"

# create the default EwlButton db
DB=$HOME/.ewl/themes/defaultTheme/EwlButton
./ewldbtool $DB "padding/left"   "0"
./ewldbtool $DB "padding/top"    "0"
./ewldbtool $DB "padding/right"  "0"
./ewldbtool $DB "padding/bottom" "0"
./ewldbtool $DB "background"     "images/chrome_button.png"
./ewldbtool $DB "images/normal"  "images/chrome_button.png"
./ewldbtool $DB "images/hilited" "images/chrome_button-hilited.png"
./ewldbtool $DB "images/clicked" "images/chrome_button-clicked.png"
./ewldbtool $DB "num_layers"     "1"
./ewldbtool $DB "layer-00/name"        "clicked"
./ewldbtool $DB "layer-00/width"       "320"
./ewldbtool $DB "layer-00/height"      "240"
./ewldbtool $DB "layer-00/alpha"       "false"
./ewldbtool $DB "layer-00/visible"     "true"
./ewldbtool $DB "layer-00/num_images"  "1"
./ewldbtool $DB "layer-00/image-00"         "images/aqua_button.png"
./ewldbtool $DB "layer-00/image-00/visible" "true"

#####################
#   OTHER WIDGETS   #
#####################

# create the default EwlLabel db
DB=$HOME/.ewl/themes/defaultTheme/EwlLabel
./ewldbtool $DB "padding/left"   "0"
./ewldbtool $DB "padding/top"    "0"
./ewldbtool $DB "padding/right"  "0"
./ewldbtool $DB "padding/bottom" "0"
