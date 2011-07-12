#!/bin/bash
# Script to run EFL test-suite
# We have two ways of running this script:
# tsuite.sh TestFile
# This produces files for relevant test, then compares with .original files
# tsuite.sh Testfile o
# This produces .original files to be used later for comparison
# Test file format:
# TestName Delay TestEXE params
# For example:
# BtnTest 0.5 elementary_test "Buttons"

# Here we process each line of input file
# Each line composed of:
# TestName Delay Test-Exe params...

#__DEBUG="on"
function DEBUG()
{
   [ "$_DEBUG" == "on" ] &&  $@
}

# process_line func executes a single test
process_line () {
# Test input line format, should satisfy
# TestName Delay Test-Exe params...
local txt=`echo "$1" | sed -e 's/^[ \t]*//'`
local tmp=${txt:0:1}
if [ "x$tmp" = "x" ]
then
   DEBUG echo "Input line is empty."
   return 0
fi

if [ "x$tmp" = "x#" ]
then
   DEBUG echo "This is a comment line \"$1\""
   return 0
fi

# Reset txt to original input line
txt="$1"
DEBUG echo "reset line: $txt"
local ext=""
# if user specified 2nd param we produce file with ".original" extention
if [ $2 -gt 1  ]
then
   ext="$test_ext"
fi
DEBUG echo params "$txt"
DEBUG echo ext="$ext"

local testname=`echo "$txt" | cut -d ' ' -f1`
local timeout=`echo "$txt" | cut -d ' ' -f2`
# Take length of testname, timeout,  + 2 spaces to compose cmd string
local n=`expr length $testname`
(( n = n + `expr length $timeout` + 2))

local cmd=${txt:n}
DEBUG echo testname is: "$testname"
DEBUG echo timeout is: "$timeout"
DEBUG echo command is: \'"$cmd"\'

# Compose file names
local file_ext="png"
local file_to_write="${testname}${ext}.${file_ext}"
local file_to_test="${testname}${test_ext}.${file_ext}"
local file_of_comp="${testname}${comp_ext}.${file_ext}"

# Make test command
local exp="ELM_ENGINE=\"shot:delay="$timeout":file="$file_to_write"\""
DEBUG echo exp is: \'"$exp"\'
DEBUG echo "Producing: $file_to_write"
eval $exp $cmd
if [ $? -ne 0 ]
then
   echo "'$cmd' Failed to execute, aborting."
   exit 1
fi
(( noriginal++ ))

# if we don't have 2nd parameter, we need to compare files.
if [ $2 -eq 1  ]
then
# if file was written, check if test-file existes
   if [ -e "$file_to_write" ]
   then
      local md_to_write=`md5sum $file_to_write`

      if [ -e "$file_to_test" ]
# if test file exsites, compare them
      then
         (( ncomp++ ))
         local md_to_test=`md5sum $file_to_test`
         local mdw=`echo "$md_to_write" | cut -d ' ' -f1`
         local mdt=`echo "$md_to_test" | cut -d ' ' -f1`
# if md5 is different, files are NOT identical
         if [ "x$mdw" != "x$mdt" ]
         then
            DEBUG echo "$file_to_write IS NOT identical to $file_to_test"
            if [ $comp_unavail -eq 0 ]
            then
               compare "$file_to_write" "$file_to_test" "$file_of_comp"
               echo "Created $file_of_comp"
            else
               echo "$testname does not match."
            fi
            (( nfail++ ))
         fi
      else
         (( nerr++ ))
         echo "Test file was not found $file_to_test"
      fi
   else
      (( nerr++ ))
      echo "Failed to write $file_to_write"
   fi
fi

return 0
}


# Script Entry point
DEBUG echo "$0" STARTED `date +'%Y-%m-%d-%H-%M-%S'`

DEBUG echo PARAMETERS:

filename="$1"
# Test if found input-file
if [ -e "$filename" ]
then
   DEBUG echo "$filename - input file exist"
else
   echo "$filename - No input file, exit"
   exit -1
fi
which compare > /dev/null
comp_unavail=$?
DEBUG echo "comp_unavail = \"$comp_unavail\""
if [ $comp_unavail -ne 0 ]
then
   if [ $# -eq 1  ]
   then
      echo "Compare software missing."
      echo "Install \"ImageMagick\" if you like to procduce \"comp\" files."
      echo "Printing diffs to output"
   fi
fi

test_ext=".original"
comp_ext=".comp"
nlines=0
noriginal=0
nerr=0
ncomp=0
nfail=0
# Process all lines in txt file:
while read line
do
   (( nfiles++ ))
   process_line "$line" "$#"
   if [ $? -ne 0 ]
   then
      (( nerr++ ))
   fi
done < "$filename"

if [ $# -eq 1  ]
then
   echo "Compared $ncomp with $nfail diffs."
else
   echo "Finished producing ($noriginal) \"$test_ext\" files."
fi

DEBUG echo "$0" DONE `date +'%Y-%m-%d-%H-%M-%S'`

exit $nfail
