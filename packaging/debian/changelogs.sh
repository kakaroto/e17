#!/bin/bash
# simple script for updating changelogs
# license: gpl v3
# author: quaker <quaker66@gmail.com>

# variables
# find debian dirs in..
subdirs="main extras"
# snapshot version
snapversion="062"
# snapshot revision
snaprevision="ENTER"

# function for parsing and initiating new changelog
initiatechangelog()
{
	echo "Making new changelog template for $1"
	oldversion=$(dpkg-parsechangelog -l$1 | grep "Version:" | sed 's/Version: //')
	source=$(dpkg-parsechangelog -l$1 | grep "Source:" | sed 's/Source: //')
	distribution=$(dpkg-parsechangelog -l$1 | grep "Distribution:" | sed 's/Distribution: //')
	urgency=$(dpkg-parsechangelog -l$1 | grep "Urgency:" | sed 's/Urgency: //')
	maintainer=$(dpkg-parsechangelog -l$1 | grep "Maintainer:" | sed 's/Maintainer: //')
	echo "Enter new version, old version was: $oldversion"
	echo "You can use @SNAPVERSION@ - that is snapshot version number(062 for example)"
	echo "and @SNAPREVISION@ - that is snapshot svn revision(41040 for example)"
	read newversion
	echo -e "$source ($newversion) $distribution; urgency=$urgency\n\n  * a SVN release.\n\n -- $maintainer  @DATE@"
	echo "OK? [Y/n]"
	read answer
	case $answer in
		n|N) echo "OK, skipping"
		;;
		*)
			echo -e "$source ($newversion) $distribution; urgency=$urgency\n\n  * a SVN release.\n\n -- $maintainer  @DATE@" > $1
		;;
	esac
}

# function for replacing variables in changelog
updatechangelog()
{
	echo "Replacing variables in changelog $1"
	date=$(LC_ALL=C date "+%a, %d %b %Y %T %z")
	if test "$snapversion" = "ENTER"; then
		echo "snapversion is set to ENTER, so enter snapshot version(062 for example)"
		read snapversion
	fi
	if test "$snaprevision" = "ENTER"; then
		echo "snaprevision is set to ENTER, so enter snapshot revision(41040 for example)"
		read snaprevision
	fi
	cat $1 | sed "s/@SNAPVERSION@/$snapversion/" | sed "s/@SNAPREVISION@/$snaprevision/" | sed "s/@DATE@/$date/"
	echo "OK? [Y/n]"
	read answer
	case $answer in
		n|N) echo "OK, skipping"
		;;
		*)
			cat $1 | sed "s/@SNAPVERSION@/$snapversion/" | sed "s/@SNAPREVISION@/$snaprevision/" | sed "s/@DATE@/$date/" > $1
		;;
	esac
}

# find changelogs function
findchangelogs()
{
	for dirs in $subdirs; do
		cd $dirs
		for changelogs in *; do
			[ -d "$changelogs" ] && case $1 in 
						initiate) initiatechangelog "$changelogs/changelog"
						;;
						update) updatechangelog "$changelogs/changelog"
						;;
						esac
		done
		cd ..
	done
}

# body

case $1 in
	initiate) findchangelogs initiate
	;;
	update) findchangelogs update
	;;
	*) echo -e "Usage: $0 initiate|update\n\n   initiate: generate a new changelog template\n   from working out-of-date changelog.\n\n   update: fill the changelog template generated with initiate." && exit 0
	;;
esac


exit 0
