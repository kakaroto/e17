#!/bin/bash
for x in darkness-{default,standard,illume}; do
	eet -e $x/base.cfg config $x/base.src 1
done

eet -i darkness-elm.cfg config profile.src 0
