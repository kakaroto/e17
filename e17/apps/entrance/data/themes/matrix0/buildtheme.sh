#!/bin/sh -e
DB="./matrix0.db"
rm -f $DB

edb_ed $DB add /entrance/welcome/font/name str "ocran.ttf"
edb_ed $DB add /entrance/welcome/font/size int 14
edb_ed $DB add /entrance/welcome/font/style str "raised"
edb_ed $DB add /entrance/welcome/color str "#90FF90FF"
edb_ed $DB add /entrance/welcome/pos str "0.0:1.0"
edb_ed $DB add /entrance/welcome/offset str "65:-55"

edb_ed $DB add /entrance/passwd/font/name str "ocran.ttf"
edb_ed $DB add /entrance/passwd/font/size int 14
edb_ed $DB add /entrance/passwd/font/style str "raised"
edb_ed $DB add /entrance/passwd/color str "#90FF90FF"

edb_ed $DB add /entrance/entry/font/name str "ocran.ttf"
edb_ed $DB add /entrance/entry/font/size int 18
edb_ed $DB add /entrance/entry/font/style "normal"
edb_ed $DB add /entrance/entry/pos str "0.0:1.0"
edb_ed $DB add /entrance/entry/offset str "65:-35"
edb_ed $DB add /entrance/entry/color str "#D8FFD8FF"
edb_ed $DB add /entrance/entry/box/pos str "0.0:1.0"
edb_ed $DB add /entrance/entry/box/offset str "55:-35"
edb_ed $DB add /entrance/entry/box/size str "300:42"
edb_ed $DB add /entrance/entry/box/color str "#00000000"

edb_ed $DB add /entrance/sessions/selected/font/name str "extrafrosty.ttf"
edb_ed $DB add /entrance/sessions/selected/font/size int 24
edb_ed $DB add /entrance/sessions/selected/font/style str "shadow"
edb_ed $DB add /entrance/sessions/selected/icon/size str "48:48"
edb_ed $DB add /entrance/sessions/selected/icon/pos str "0.04:0.04"
edb_ed $DB add /entrance/sessions/selected/icon/offset str "0:55"
edb_ed $DB add /entrance/sessions/selected/text/pos str "0.04:0.04"
edb_ed $DB add /entrance/sessions/selected/text/offset str "58:60"
edb_ed $DB add /entrance/sessions/selected/text/color str "#FFFFFFFF"
edb_ed $DB add /entrance/sessions/selected/text/hicolor str "#C0FFC0FF"

edb_ed $DB add /entrance/sessions/list/text/font/name str "extrafrosty.ttf"
edb_ed $DB add /entrance/sessions/list/text/font/size int 16
edb_ed $DB add /entrance/sessions/list/text/font/style str "outline"
edb_ed $DB add /entrance/sessions/list/text/color str "#28FF8090"

edb_ed $DB add /entrance/sessions/list/seltext/font/name str "extrafrosty.ttf"
edb_ed $DB add /entrance/sessions/list/seltext/font/size str 16
edb_ed $DB add /entrance/sessions/list/seltext/font/style str "shadow"
edb_ed $DB add /entrance/sessions/list/seltext/color str "#28FF80FF"

edb_ed $DB add /entrance/sessions/list/box/color str "#00000060"
edb_ed $DB add /entrance/sessions/list/box/size str "250:200"
edb_ed $DB add /entrance/sessions/list/box/pos str "0.04:0.04"
edb_ed $DB add /entrance/sessions/list/box/offset str "325:55"

edb_ed $DB add /entrance/hostname/font/name str "miltown.ttf"
edb_ed $DB add /entrance/hostname/font/size int 24
edb_ed $DB add /entrance/hostname/font/style str "shadow"
edb_ed $DB add /entrance/hostname/pos str "0.04:0.04"
edb_ed $DB add /entrance/hostname/offset str "10:10"
edb_ed $DB add /entrance/hostname/color str "#FFFFFFFF"

edb_ed $DB add /entrance/date/font/name str "ocran.ttf"
edb_ed $DB add /entrance/date/font/size int 14
edb_ed $DB add /entrance/date/font/style str "normal"
edb_ed $DB add /entrance/date/pos str "0.8:1.0"
edb_ed $DB add /entrance/date/offset str "0:-55"
edb_ed $DB add /entrance/date/color str "#FFFFFFFF"

edb_ed $DB add /entrance/time/font/name str "ocran.ttf"
edb_ed $DB add /entrance/time/font/size int 14
edb_ed $DB add /entrance/time/font/style str "normal"
edb_ed $DB add /entrance/time/pos str "0.8:1.0"
edb_ed $DB add /entrance/time/offset str "0:-30"
edb_ed $DB add /entrance/time/color str "#FFFFFFFF"

edb_ed $DB add /entrance/face/pos str "0.04:0.04"
edb_ed $DB add /entrance/face/offset str "0:125"
edb_ed $DB add /entrance/face/size str "192:223"
edb_ed $DB add /entrance/face/border int 2
edb_ed $DB add /entrance/face/color str "#20C020FF"
edb_ed $DB add /entrance/background str "matrix0.bg.db"
edb_ed $DB add /entrance/pointer str "pointer.png"

