#!/bin/sh

# debhelper2.sh - creates e's debhelper support files


cat > "./enlightenment-nosound.conffiles" <<- "END"
/etc/menu-methods/enlightenment-nosound
END


cat > "./enlightenment-nosound.menu" <<- "END"
?package(enlightenment-nosound):needs="wm" section="WindowManagers"\
   title="Enlightenment" command="/usr/X11R6/bin/enlightenment"
END


cat > "./enlightenment-nosound.menu-method" <<- "END"
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


cat > "./enlightenment-nosound.prerm" <<- "END"
#/bin/sh -e

find /etc/X11/enlightenment/menus -type f -name Debian_*.menu | xargs rm -rf

#DEBHELPER#
END


cat > "./enlightenment-theme-brushedmetal.postinst" <<- "END"
#!/bin/sh

set -e

update-alternatives --install /usr/share/enlightenment/themes/DEFAULT \
enlightenment-theme-default \
/usr/share/enlightenment/themes/BrushedMetal 50
END


cat > "./enlightenment-theme-brushedmetal.prerm" <<- "END"
#!/bin/sh

set -e

update-alternatives --remove enlightenment-theme-default \
 /usr/share/enlightenment/themes/BrushedMetal
END


cat > "./enlightenment-theme-ganymede.postinst" <<- "END"
#!/bin/sh

set -e

update-alternatives --install /usr/share/enlightenment/themes/DEFAULT \
enlightenment-theme-default \
/usr/share/enlightenment/themes/Ganymede 30
END


cat > "./enlightenment-theme-ganymede.prerm" <<- "END"
#!/bin/sh

set -e

update-alternatives --remove enlightenment-theme-default \
 /usr/share/enlightenment/themes/Ganymede
END


cat > "./enlightenment-theme-shinymetal.postinst" <<- "END"
#!/bin/sh

set -e

update-alternatives --install /usr/share/enlightenment/themes/DEFAULT \
enlightenment-theme-default \
/usr/share/enlightenment/themes/ShinyMetal 40
END


cat > "./enlightenment-theme-shinymetal.prerm" <<- "END"
#!/bin/sh

set -e

update-alternatives --remove enlightenment-theme-default \
 /usr/share/enlightenment/themes/ShinyMetal
END


cat > "./enlightenment.conffiles" <<- "END"
/etc/menu-methods/enlightenment
END


cat > "./enlightenment.menu" <<- "END"
?package(enlightenment):needs="wm" section="WindowManagers"\
   title="Enlightenment" command="/usr/X11R6/bin/enlightenment"
END


cat > "./enlightenment.menu-method" <<- "END"
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


cat > "./enlightenment.prerm" <<- "END"
#/bin/sh -e

find /etc/X11/enlightenment/menus -type f -name Debian_*.menu | xargs rm -rf

#DEBHELPER#
END


