#!/bin/sh -e
DB="./entrance_config.db"
rm -f $DB

edb_ed $DB add /entrance/welcome str "Enter Username:"
edb_ed $DB add /entrance/passwd str "Enter Password:"
edb_ed $DB add /entrance/passwd_echo int 1
edb_ed $DB add /entrance/use_pam_auth int 1
edb_ed $DB add /entrance/xinerama/screens/w int 1
edb_ed $DB add /entrance/xinerama/screens/h int 1
edb_ed $DB add /entrance/xinerama/on/w int 1
edb_ed $DB add /entrance/xinerama/on/h int 1
edb_ed $DB add /entrance/theme str "BlueCrystal"
edb_ed $DB add /entrance/date_format str "%A %B %e, %Y"
edb_ed $DB add /entrance/time_format str "%l:%M:%S %p"
edb_ed $DB add /entrance/session/0/name str "enlightenment"
edb_ed $DB add /entrance/session/0/path str ""
edb_ed $DB add /entrance/session/0/icon str "enlightenment.png"
edb_ed $DB add /entrance/session/1/name str "kde2"
edb_ed $DB add /entrance/session/1/path str ""
edb_ed $DB add /entrance/session/1/icon str "kde.png"
edb_ed $DB add /entrance/session/2/name str "blackbox"
edb_ed $DB add /entrance/session/2/path str ""
edb_ed $DB add /entrance/session/2/icon str "blackbox.png"
edb_ed $DB add /entrance/session/3/name str "fluxbox"
edb_ed $DB add /entrance/session/3/path str ""
edb_ed $DB add /entrance/session/3/icon str "default.png"
edb_ed $DB add /entrance/session/4/name str "Failsafe"
edb_ed $DB add /entrance/session/4/path str "failsafe"
edb_ed $DB add /entrance/session/4/icon str "default.png"
edb_ed $DB add /entrance/session/count int 5
edb_ed $DB add /entrance/session/default int 0

