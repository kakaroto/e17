#!/bin/bash
# define $EDITOR and EDITOR_FLAGS in your environment to customize this
CL="ChangeLog"
DSTR=`date +%Y%m%d-%H%M-%Z`
TF="ewl-changelog-entry-"$USER

if test -z $EDITOR; then
	EDITOR="vim"
	EDITOR_FLAGS="+"
fi

echo -e "-----\n$DSTR, $USER\n\n" > $TF
$EDITOR $EDITOR_FLAGS $TF
echo "" >> $TF
cat $TF >> $CL
rm $TF
