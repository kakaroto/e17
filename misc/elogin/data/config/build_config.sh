#!/bin/sh -e
DB="./elogin_config.db"

edb_ed $DB add /elogin/welcome/mess str "Enter Your Username"
edb_ed $DB add /elogin/welcome/font/name str "notepad.ttf"
edb_ed $DB add /elogin/welcome/font/size int 20
edb_ed $DB add /elogin/welcome/font/r int 192
edb_ed $DB add /elogin/welcome/font/g int 192
edb_ed $DB add /elogin/welcome/font/b int 192
edb_ed $DB add /elogin/welcome/font/a int 220
edb_ed $DB add /elogin/passwd/mess str "Enter Your Password..."
edb_ed $DB add /elogin/passwd/font/name str "notepad.ttf"
edb_ed $DB add /elogin/passwd/font/size int 20
edb_ed $DB add /elogin/passwd/font/r int 192
edb_ed $DB add /elogin/passwd/font/g int 192
edb_ed $DB add /elogin/passwd/font/b int 192
edb_ed $DB add /elogin/passwd/font/a int 220
edb_ed $DB add /elogin/xinerama/screens/w int 1
edb_ed $DB add /elogin/xinerama/screens/h int 1
edb_ed $DB add /elogin/xinerama/on/w int 1
edb_ed $DB add /elogin/xinerama/on/h int 1
edb_ed $DB add /elogin/session/0 str "E17"
edb_ed $DB add /elogin/session/1 str "Enlightenment"
edb_ed $DB add /elogin/session/2 str "KDE"
edb_ed $DB add /elogin/session/3 str "GNOME"
edb_ed $DB add /elogin/session/4 str "blackbox"
edb_ed $DB add /elogin/session/5 str "Sawfish"
edb_ed $DB add /elogin/session/6 str "failsafe"
edb_ed $DB add /elogin/session/count int 7
