#!/bin/sh -e
DB="./entrance_config.db"
rm -f $DB

# set auth to 1 for pam, 2 for shadow
edb_ed $DB add /entrance/auth int 1
#edb_ed $DB add /entrance/xinerama/screens/w int 1
#edb_ed $DB add /entrance/xinerama/screens/h int 1
#edb_ed $DB add /entrance/xinerama/on/w int 1
#edb_ed $DB add /entrance/xinerama/on/h int 1
edb_ed $DB add /entrance/theme str "default.eet"
edb_ed $DB add /entrance/date_format str "%A %B %e, %Y"
edb_ed $DB add /entrance/time_format str "%l:%M:%S %p"
#edb_ed $DB add /entrance/fonts/count int 2
#edb_ed $DB add /entrance/fonts/0/str str "/usr/share/fonts/truetype/"
#edb_ed $DB add /entrance/fonts/1/str str "/usr/X11R6/lib/X11/fonts/Truetype/"
edb_ed $DB add /entrance/session/count int 9
edb_ed $DB add /entrance/session/0/value str ""
edb_ed $DB add /entrance/session/0/key str "Default"
edb_ed $DB add /entrance/session/1/value str "enlightenment"
edb_ed $DB add /entrance/session/1/key str "Enlightenment"
edb_ed $DB add /entrance/session/2/value str "kde"
edb_ed $DB add /entrance/session/2/key str "KDE"
edb_ed $DB add /entrance/session/3/value str "gnome"
edb_ed $DB add /entrance/session/3/key str "Gnome"
edb_ed $DB add /entrance/session/4/value str "gnome"
edb_ed $DB add /entrance/session/4/key str "Gnome"
edb_ed $DB add /entrance/session/5/value str "blackbox"
edb_ed $DB add /entrance/session/5/key str "Blackbox"
edb_ed $DB add /entrance/session/6/value str "sawfish"
edb_ed $DB add /entrance/session/6/key str "Sawfish"
edb_ed $DB add /entrance/session/7/value str "xfce"
edb_ed $DB add /entrance/session/7/key str "XFce"
edb_ed $DB add /entrance/session/8/value str "failsafe"
edb_ed $DB add /entrance/session/8/key str "Failsafe"
edb_ed $DB add /entrance/system/reboot int 1
edb_ed $DB add /entrance/system/halt int 1
