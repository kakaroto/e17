#!/bin/sh

# Use this only if your browser follows the -remote command
# line args # (http://www.mozilla.org/unix/remote.html),
# This script will open URLs in a new tab of the
# currently running browser, or starts a new browser if none
# is currently running.

# Set your BROWSER environment variable to the web browser
# this script should launch.

if test "x$BROWSER" == "x"; then
	echo "WARNING: BROWSER environment variable not set, falling back to mozilla";
	BROWSER="mozilla";
fi

RUNNING=`$BROWSER -remote 'ping()' 2>&1`;
if test "x$RUNNING" == "x"; then
	$BROWSER -remote "openURL($1, new-tab)";
else
	$BROWSER $1;
fi
