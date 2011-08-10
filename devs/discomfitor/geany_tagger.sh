#!/bin/bash
#
# Author:  Mike Blumenkrantz <mike@zentific.com>
# License: GPL v2 or later
#
# Create a C tags file for Geany from a C/C++ header file
#

header="$1"
tagfile="$2"

progname="$(basename $0)"
if [[ -z $header ]] || [[ -z $tagfile ]];then
	echo "Usage: $progname header_file tag_file"
	exit
elif [[ "${tagfile/.c.tags}" = "$tagfile" ]];then
	echo "tag_file must end with .c.tags!"
	exit
fi

stdheaders=(assert.h complex.h ctype.h errno.h fenv.h float.h inttypes.h iso646.h limits.h locale.h math.h setjmp.h signal.h stdarg.h stdbool.h stddef.h stdint.h stdio.h stdlib.h string.h time.h wchar.h wctype.h)

grab_headers(){
	echo -e "***Finding more headers from: ${new[@]}\n"
	for h in ${new[@]};do
		echo -e "\t***Grabbing headers from $h"
		new=($(sed -n 's/^#include ["<]\(.*\)[">]/\1/p' "$h"|awk '{print $1}'))
		[[ -z "${new[@]}" ]] && return
		#remove c99 headers
		for x in ${stdheaders[@]};do
			new=(${new[@]//$x})
		done
		[[ -z "${new[@]}" ]] && return
		echo -e "\t***Pruning pre-existing headers from new list [1/2]..."
		for x in ${headers[@]};do
#			echo $x
			new=(${new[@]//$x})
		done
		[[ -z "${new[@]}" ]] && return
		echo -e "\t***Pruning pre-existing headers from new list [2/2]..."
		for x in ${new[@]};do
#			echo $x
			if (echo "${headers[@]}"|grep -q $x) ; then
				new=(${new[@]//$x})
			fi
		done
		[[ -z "${new[@]}" ]] && return
		#remove duplicates
		echo -e "\t***Removing duplicates from new list..."
		new=($(printf "%s\n" ${new[@]}|sort -u))
		[[ -z "${new[@]}" ]] && return
		echo -e "\t***Mapping the following headers to absolute paths: ${new[@]}\n"
		for x in ${new[@]};do
			#if already mapped, ignore
			[[ -f $x ]] && continue

			#try looking only in /usr/include first
			y=$(find /usr/include -maxdepth 1 -path "*/$x"|head -n1)
			#if that fails then look in other dirs
			[[ -n "$y" && -f "$y" ]] || y=$(find /usr/include -mindepth 1 -path "*/$x"|head -n1)
			#replace with absolute path
			new=(${new[@]/$x/$y})
		done
		for x in ${new[@]};do
			[[ -f $x ]] || new=(${new[@]/$x/$(find /usr/include -mindepth 1 -path "*/$x"|head -n1)})
		done
		headers+=(${new[@]})
	done
}

#grab initial headers to include
if [[ ! -f $(readlink -f $header) ]];then
	echo "ERROR: $progname requires the header to be passed with its absolute path" 
	exit 1
fi
new=($header)
headers=(${new[@]})
#run at least twice
grab_headers
grab_headers
#run while grabbing is still possible
while [[ -n "${new[@]}" ]];do
	grab_headers
done
echo -e "***Generating tags for the following headers: ${headers[@]}\n"
geany -P -g "$tagfile" ${headers[@]}

#remove static variables
sed -i '/^_/d' "$tagfile"

