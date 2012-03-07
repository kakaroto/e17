#!/bin/bash
for x in darkness-{default,desktop,illume}; do
	eet -e $x/base.cfg config $x/base.src 1
	[[ $? != 0 ]] && exit 1
done

eet -i darkness.cfg config profile.src 0
[[ $? != 0 ]] && exit 1
[[ "$1" != "-i" ]] && exit 0
cp -r darkness-default darkness-illume darkness-desktop darkness.cfg ~/.elementary/config
rm ~/.elementary/config/darkness-{default,illume,desktop}/base.src
