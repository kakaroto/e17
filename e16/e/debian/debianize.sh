#!/bin/sh

# This is the Debian Enlightened Squash System. It was inspired by a mutual
# dislike of the large number of debhelper files in the e/debian/ dir. I was
# able to kill several birds with one stone, too. The debianize.sh script does
# the menu and makefile mangling and also creates the debhelper files.

# The makefile target named, squash, in the debian/rules makefile creates
# debianize.sh from debianinze-stub.sh and the various debhelper files.

# ljlane@debian.org

set -e

mv src/themes/Makefile src/themes/Makefile.old
sed '/^install-data-local:$/,/^$/ d' src/themes/Makefile.old > src/themes/Makefile

mv src/Makefile src/Makefile.old
sed '/^install-data-local:$/,$ d' src/Makefile.old > src/Makefile

sed '/^__E_CFG_VERSION 0$/ { 
a\
\
BEGIN_NEW_FILE_MENU("DEBIAN_MENU", "ROOT", "/etc/X11/enlightenment/menus/Debian.menu")\
END_MENU\

}
/^ADD_MENU_SUBMENU_TEXT_ITEM("User Menus", *"APPS_SUBMENU")$/ {
a\
ADD_MENU_SUBMENU_TEXT_ITEM("Debian",  "DEBIAN_MENU")
}' 'src/themes/configs/menus.cfg' > 'debian/menus.cfg'

#### end of debianize.sh, file restoration follows, creates debianize-stub.sh too ####

cat > "debian/enlightenment-dox.links" <<- "END"
usr/share/doc/enlightenment-docs usr/share/doc/enlightenment-dox

END

cat > "debian/enlightenment-nosound.conffiles" <<- "END"
/etc/menu-methods/enlightenment-nosound
END

cat > "debian/enlightenment-nosound.links" <<- "END"
usr/share/doc/enlightenment-doc usr/share/doc/enlightenment-nosound
END

cat > "debian/enlightenment-nosound.menu" <<- "END"
?package(enlightenment-nosound):needs="wm" section="WindowManagers"\
   title="Enlightenment" command="/usr/X11R6/bin/enlightenment"
END

cat > "debian/enlightenment-nosound.menu-method" <<- "END"
#!/usr/sbin/install-menu

compat="menu-1"

genmenu=replacewith(parent($section), "/", "_") ".menu"
rootprefix="/etc/X11/enlightenment/menus"
treewalk="cm"
rootsection="Debian"

# Limit usage to root for now until we figure out how to convince the
# E menu to ignore the system menus if user menus exist
onlyrunasroot=true

function quote($target)= "\"" $target "\""
function tick($target)= "\'" $target "\'"

# choose your term here, uncomment one
function term()="Eterm -T"
#function term()="xterm -T"
#function term()="rxvt -title"

function menu()= quote(prefix() "/" replacewith($section , "/", "_") ".menu")

function icon_select()=ifelse($icon32x32, $icon32x32, \
     ifelse($icon16x16, $icon16x16, $icon))
function icon()= quote(ifelse(icon_select(),icon_select(),"NULL"))

function title()= quote(esc($title,"\""))

function x11()= quote($command)
function text()= quote(term() " " tick(esc($title,"\"")) " -e " tick($command))
function wm()= quote("eesh -e " tick("restart_wm " $command))

function menu_entry($name,$icon,$action,$parameters)=\
firstentry(quote(basename($section)) "\n")\
print($name "  " $icon "  " $action " " $parameters "\n")

supported
   x11=  menu_entry( title(), icon(), quote("exec"), x11())
   text= menu_entry( title(), icon(), quote("exec"), text())
   wm= menu_entry( title(), icon(), quote("exec"), wm())
endsupported

submenutitle= menu_entry( title(), icon(), quote("menu"), menu())

# this stuff is unused, but has proven to be a pain if not defined
preoutput=""
postoutput=""
startmenu=""
endmenu=""

END

cat > "debian/enlightenment-nosound.prerm" <<- "END"
#!/bin/sh 

set -e

find /etc/X11/enlightenment/menus -type f -name Debian_*.menu -print0 | xargs -0r rm -f

#DEBHELPER#

END

cat > "debian/enlightenment-theme-bluesteel.links" <<- "END"
usr/share/doc/enlightenment-docs usr/share/doc/enlightenment-theme-bluesteel
END

cat > "debian/enlightenment-theme-brushedmetal.links" <<- "END"
usr/share/doc/enlightenment-docs usr/share/doc/enlightenment-theme-brushedmetal
END

cat > "debian/enlightenment-theme-brushedmetal.postinst" <<- "END"
#!/bin/sh

set -e

update-alternatives --install /usr/share/enlightenment/themes/DEFAULT \
enlightenment-theme-default \
/usr/share/enlightenment/themes/BrushedMetal 50

#DEBHELPER#

END

cat > "debian/enlightenment-theme-shinymetal.postinst" <<- "END"
#!/bin/sh

set -e

update-alternatives --install /usr/share/enlightenment/themes/DEFAULT \
enlightenment-theme-default \
/usr/share/enlightenment/themes/ShinyMetal 40

#DEBHELPER#

END

cat > "debian/enlightenment-theme-shinymetal.prerm" <<- "END"
#!/bin/sh

set -e

update-alternatives --remove enlightenment-theme-default \
 /usr/share/enlightenment/themes/ShinyMetal

#DEBHELPER#

END

cat > "debian/enlightenment.links" <<- "END"
usr/share/doc/enlightenment-docs usr/share/doc/enlightenment
END

cat > "debian/enlightenment.menu" <<- "END"
?package(enlightenment):needs="wm" section="WindowManagers"\
   title="Enlightenment" command="/usr/bin/enlightenment"
END

cat > "debian/enlightenment.menu-method" <<- "END"
#!/usr/sbin/install-menu

compat="menu-1"

genmenu=replacewith(parent($section), "/", "_") ".menu"
rootprefix="/etc/X11/enlightenment/menus"
treewalk="cm"
rootsection="Debian"

# Limit usage to root for now until we figure out how to convince the
# E menu to ignore the system menus if user menus exist
onlyrunasroot=true

function quote($target)= "\"" $target "\""
function tick($target)= "\'" $target "\'"

# choose your term here, uncomment one
function term()="Eterm -T"
#function term()="xterm -T"
#function term()="rxvt -title"

function menu()= quote(prefix() "/" replacewith($section , "/", "_") ".menu")

function icon_select()=ifelse($icon32x32, $icon32x32, \
     ifelse($icon16x16, $icon16x16, $icon))
function icon()= quote(ifelse(icon_select(),icon_select(),"NULL"))

function title()= quote(esc($title,"\""))

function x11()= quote($command)
function text()= quote(term() " " tick(esc($title,"\"")) " -e " tick($command))
function wm()= quote("eesh -e " tick("restart_wm " $command))

function menu_entry($name,$icon,$action,$parameters)=\
firstentry(quote(basename($section)) "\n")\
print($name "  " $icon "  " $action " " $parameters "\n")

supported
   x11=  menu_entry( title(), icon(), quote("exec"), x11())
   text= menu_entry( title(), icon(), quote("exec"), text())
   wm= menu_entry( title(), icon(), quote("exec"), wm())
endsupported

submenutitle= menu_entry( title(), icon(), quote("menu"), menu())

# this stuff is unused, but has proven to be a pain if not defined
preoutput=""
postoutput=""
startmenu=""
endmenu=""

END

cat > "debian/enlightenment.prerm" <<- "END"
#!/bin/sh

set -e

find /etc/X11/enlightenment/menus -type f -name Debian_*.menu -print0 | xargs -0r rm -f

#DEBHELPER#

END

cat > "debian/enlightenment-docs.dirs" <<- "END"
usr/share/doc/enlightenment-docs
END

cat > "debian/enlightenment-dox.files" <<- "END"
usr/bin/dox
usr/share/enlightenment/E-docs
END

cat > "debian/debianize-stub.sh" <<- "END"
#!/bin/sh

# This is the Debian Enlightened Squash System. It was inspired by a mutual
# dislike of the large number of debhelper files in the e/debian/ dir. I was
# able to kill several birds with one stone, too. The debianize.sh script does
# the menu and makefile mangling and also creates the debhelper files.

# The makefile target named, squash, in the debian/rules makefile creates
# debianize.sh from debianinze-stub.sh and the various debhelper files.

# ljlane@debian.org

set -e

mv src/themes/Makefile src/themes/Makefile.old
sed '/^install-data-local:$/,/^$/ d' src/themes/Makefile.old > src/themes/Makefile

mv src/Makefile src/Makefile.old
sed '/^install-data-local:$/,$ d' src/Makefile.old > src/Makefile

grep -q 'DEBIAN_MENU' 'src/themes/configs/menus.cfg' ||  \
mv "src/themes/configs/menus.cfg" "src/themes/configs/menus.cfg.old" &&  \
sed '/^__E_CFG_VERSION 0$/ { 
a\
\
BEGIN_NEW_FILE_MENU("DEBIAN_MENU", "ROOT", "/etc/X11/enlightenment/menus/Debian.menu")\
END_MENU\

}
/^ADD_MENU_SUBMENU_TEXT_ITEM("User Menus", *"APPS_SUBMENU")$/ {
a\
ADD_MENU_SUBMENU_TEXT_ITEM("Debian",  "DEBIAN_MENU")
}' 'src/themes/configs/menus.cfg.old' > 'src/themes/configs/menus.cfg'

#### end of debianize.sh, file restoration follows, creates debianize-stub.sh too ####

END

cat > "debian/enlightenment-dox.dirs" <<- "END"
usr/share/enlightenment/E-docs
END

cat > "debian/enlightenment-theme-brushedmetal.dirs" <<- "END"
usr/share/enlightenment/themes/BrushedM
END

cat > "debian/enlightenment-theme-bluesteel.dirs" <<- "END"
usr/share/enlightenment/themes/BlueSteel
END

cat > "debian/enlightenment-nosound.dirs" <<- "END"
usr/share/enlightenment/themes
END

cat > "debian/enlightenment-theme-bluesteel.postinst" <<- "END"
#!/bin/sh

set -e

update-alternatives --install /usr/share/enlightenment/themes/DEFAULT \
enlightenment-theme-default \
/usr/share/enlightenment/themes/BlueSteel 30

#DEBHELPER#

END

cat > "debian/enlightenment-theme-shinymetal.dirs" <<- "END"
usr/share/enlightenment/themes/ShinyMetal
END

cat > "debian/enlightenment-theme-shinymetal.links" <<- "END"
usr/share/doc/enlightenment-docs usr/share/doc/enlightenment-theme-shinymetal
END

cat > "debian/enlightenment.conffiles" <<- "END"
/etc/menu-methods/enlightenment
END

cat > "debian/enlightenment.dirs" <<- "END"
usr/share/enlightenment/themes
END

cat > "debian/enlightenment-theme-bluesteel.prerm" <<- "END"
#!/bin/sh

set -e

update-alternatives --remove enlightenment-theme-default \
 /usr/share/enlightenment/themes/BlueSteel

#DEBHELPER#

END

cat > "debian/enlightenment-theme-brushedmetal.prerm" <<- "END"
#!/bin/sh

set -e

update-alternatives --remove enlightenment-theme-default \
 /usr/share/enlightenment/themes/BrushedMetal

#DEBHELPER#

END

