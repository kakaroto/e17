#!/bin/sh
message=`cat`
uname=`id -un`
lineno=`echo "$message" | grep -n "Log Message:" | awk -F: ' { print $1 } '`
message=`echo "$message" | sed "1,${lineno}d"`

projectname="e"
tmpfile="/tmp/$RANDOM-$projectname"
message=`echo "$message" | head -n 5`

cat <<EOF >$tmpfile
From: commits@enlightenment.org
To: cia@users.sf.net
Content-Type: text/plain;
Subject: Announce $projectname

commit by $uname:
Files: $@
$message
EOF

if [ -e /tmp/lastlog-$projectname ]; then
        if ! diff /tmp/lastlog-$projectname $tmpfile &>/dev/null; then
                # there are differences, so this is a different commit
                cat $tmpfile | /usr/sbin/sendmail -t
                mv $tmpfile /tmp/lastlog-$projectname
        else
                # it's just cvs spamming us from another directory
                rm $tmpfile
        fi
else
        cat $tmpfile | /usr/sbin/sendmail -t
        mv $tmpfile /tmp/lastlog-$projectname
fi
