#!/bin/sh
# This script creates exactness script that later is copied to $(bindir)
# by Makefile.
# The reason to do it this was is that we would like to get $OUR_LIBPATH as
# a parameter from Makefile.
cat <<-ENDOFMESSAGE>exactness
#!/usr/bin/env bash
# tsuite_script.sh -i this makes new 'orig' folder
# tsuite_script.sh -i TestName1 [TestName2 ...] rewrite files for selcted tests in 'orig' folder
# tsuite_script.sh -r [TestName1 TestName2 ...] ; this means record [all tests or TestName additional arg]
# tsuite_script.sh -d FolderName -p [TestName1 TestName2 ...] this means play a record and put screenshots FolderName
# tsuite_script.sh -d FolderName -c [TestName1 TestName2 ...] this means compare "orig" with screenshots in FolderName
# When omitting DestDir we will use 'current' as default DestDir

#_DEBUG="on"
function DEBUG()
{
   [ "\$_DEBUG" == "on" ] &&  \$@
}

do_help () {
echo "Record tests to produce input-stream to play and set screenshot timing"
echo "To record all tests:"
echo "\$0 -r"
echo "To record specific tests:"
echo "\$0 -r TestName1 [TestName2 ...]"
echo
echo "Play tests to produce PNG files of screenshot defined while recording."
echo "To play all tests:"
echo "\$0 -p d DestDir"
echo "To play specific tests:"
echo "\$0 -p -d DestDir TestName1 [TestName2 ...]"
echo "To play and rewrite 'orig' dir, use init option for all test: \$0 -i"
echo "To play and rewrite 'orig' dir, use init option for selected tests: \$0 -i [TestName1 ...]"
echo
echo "Run comprison to produce comp_*.png files"
echo "To compare all tests:"
echo "\$0 -c [-d DestDir]"
echo "To compare specific tests:"
echo "\$0 -c -d DestDir TestName1 [TestName2 ...]"
echo
echo "NOTE:"
echo "For all actions require DestDir, when omitting this param we use 'current' as default"
}

do_record () {
DEBUG printf "do_record()\n"
# This will run record for all test if no specific test specified
# or run recording of specified tests (names passed as parameter).
# run ALL tests to record
DEBUG echo do_record "\$*"

OUR_LIBPATH="$1"

if [ -z "\$*" ]
then
   LD_PRELOAD=\${OUR_LIBPATH}/libexactness.so exactness_raw --record
else
   LD_PRELOAD=\${OUR_LIBPATH}/libexactness.so exactness_raw --record \$*
fi
return 0
}

do_play () {
# This will play record for all test if '-a' specified.
# or play record of tests specified as parameter.
# run ALL tests to record
DEBUG echo dest dir: "\$_dest_dir"
DEBUG echo do_play "\$_dest_dir" "\$*"

# Play recorded tests and produce PNG files.
# this will produce screenshots in "_dest_dir" folder
if [ -z "\$*" ]
then
# Clear all files before producing all PNG files.
   rm -rf "\$_dest_dir" &> /dev/null
   mkdir -p "\$_dest_dir"
   ELM_ENGINE="buffer" exactness_raw --destdir "\$_dest_dir"
else
   if [ -e "\$_dest_dir" ]
   then
# Remove PNG files according to tests played
      for i in \$*
      do
         rm "\$_dest_dir"/"\$i"_[0-9]*.png &> /dev/null
      done
   else
# Create dest dir
      mkdir -p "\$_dest_dir"
   fi

   ELM_ENGINE="buffer" exactness_raw --destdir "\$_dest_dir" \$*
fi

return 0
}

compare_files () {
if [ -e "\$1" ]
# First file exists
then
   local md_file1=\`md5sum \$1\`
   if [ -e "\$2" ]
   then
# Second file exists
      local md_file2=\`md5sum \$2\`

# Get md5 of both files
      local md1=\`echo "\$md_file1" | cut -d ' ' -f1\`
      local md2=\`echo "\$md_file2" | cut -d ' ' -f1\`

# Increase counter of comparisons
      (( ncomp++ ))

      # Compare md5 of both files
      if [ "x\$md1" != "x\$md2" ]
      then
         if [ \$comp_unavail -eq 0 ]
         then
            # Create diff-file with 'comp_' prefix.
            local name=\`basename "\$1"\`
            compare "\$1" "\$2" "\$_dest_dir"/comp_"\$name"
         else
            echo "\$name does not match."
         fi
# Increment counter of files not identical.
         (( nfail++ ))
      fi
   else
# Failed to find second file
      echo "Test file was not found \$2"
      (( nerr++ ))
   fi
else
# Failed to find first file
   echo "Test file was not found \$1"
   (( nerr++ ))
fi
}

process_compare () {
   # Process all files listed in array (param)
   local files_list=( "\$@" )
   for line in "\${files_list[@]}"
   do
      local name=\`basename "\$line"\`
      DEBUG echo "comparing \$name"
      compare_files "\$_orig_dir"/"\$name" "\$_dest_dir"/"\$name"
   done
}

do_compare () {
DEBUG printf "do_compare()\n"
# This will compare files in 'orig' folder with files in _dest_dir
if [ \$comp_unavail -ne 0 ]
then
   if [ \$# -eq 1  ]
   then
      echo "Compare software missing."
      echo "Install \"ImageMagick\" if you like to procduce \"comp\" files."
      echo "Printing diffs to output"
   fi
fi

if [ -z "\$_dest_dir" ]
then
      printf "For comparing, Usage: %s -c -a -d DirName\nor\n%s -c -d DirName TestName1, TestName2,...\n" \$(basename \$0) \$(basename \$0) >&2
fi

if [ "\$_dest_dir" = "\$_orig_dir" ]
then
   printf "Dest-Dir is \$_dest_dir, exiting.\n"
   return 0
fi

local files_list=
if [ -z "\$*" ]
# No test names given, compare all
then
   rm -f "\$_dest_dir"/comp_* &> /dev/null
   files_list=( \`ls "\$_dest_dir"/test_*\` )
   process_compare "\${files_list[@]}"
else
   for test_name in \$*
   do
      rm -f "\$_dest_dir"/comp_"\$test_name"_[0-9]*.png &> /dev/null
      files_list=( \`ls "\$_dest_dir"/"\$test_name"_[0-9]*.png\` )
      process_compare "\${files_list[@]}"
   done
fi

echo "Processed \$ncomp comparisons"
echo "with \$nfail file that did not match"
echo "and \$nerr errors"
return 0
}

# Script Entry Point
_record=
_play=
_compare=
_init=
_remove_fail=
_orig_dir="orig"
# Init dest_dir - should change on the fly
_dest_dir=

nerr=0
ncomp=0
nfail=0

# Test that compare is insatlled
which compare &> /dev/null
comp_unavail=\$?

while getopts 'acd:hpri?' OPTION
do
   case \$OPTION in
      c)  _compare=1
         ;;
      d)  _dest_dir="\$OPTARG"
         ;;
      p)  _play=1
          _remove_fail=1
         ;;
      r)  _record=1
          _remove_fail=1
         ;;
      i)  _dest_dir="\$_orig_dir"
          _init=1
          _play=1
          _remove_fail=1
         ;;
      h)  do_help
         exit 0
         ;;
      ?)  do_help
         exit 0
         ;;
   esac
done
shift \$((\$OPTIND - 1))
# printf "Remaining arguments are: %s\n" "\$*"
if [ -z "\$_dest_dir" ]
then
   if [ ! -z "\$_play" ]
   then
      _dest_dir="current"
   fi
   if [ ! -z "\$_compare" ]
   then
      _dest_dir="current"
   fi
else
   if [ ! -z "\$_init" ]
   then
      if [ "\$_dest_dir" != "\$_orig_dir" ]
      then
         echo "Cannot use '-i' option with a DestDir that is not 'orig'"
         echo "No need to specify DestDir when using '-i'"
         echo "For help: \$0 -h"
         exit 2
      fi
   fi
fi

if [ ! -z "\$_compare" ]
then
   if [ "\$_dest_dir" = "\$_orig_dir" ]
   then
      echo "Cannot use 'orig' dir with compare '-c' option"
      echo "Please select different DestDir"
      echo "For help: \$0 -h"
      exit 3
   fi
fi

# Here we clear all fail_*.txt files before running test
# because those files are opened with append ("a") mode.
# We don't want to accumlate old tests fail text.
if [ "\$_remove_fail" ]
then
   if [ -z "\$*" ]
   then
      # Remove all file_*.txt files before init/play/record
      DEBUG echo "removing fail_*.txt"
      rm fail_*.txt &> /dev/null
   else
      # Remove file_*.txt files before init/play/record
      for i in \$*
      do
         DEBUG echo "removing fail_\$i.txt"
         rm fail_"\$i".txt &> /dev/null
      done
   fi
fi

if [ "\$_record" ]
then
   do_record "\$*"
fi

if [ "\$_play" ]
then
   do_play "\$*"
fi

if [ "\$_compare" ]
then
   do_compare "\$*"
fi

if [ "\$_remove_fail" ]
then
   ls fail_*.txt  &> /dev/null
   if [ "\$?" -eq 0 ]
   then
      _n_tests_failed=\`ls fail_*.txt -1 | wc -l\`

      if [ "\$_n_tests_failed" ]
      then
         echo "\$_n_tests_failed tests had errors while running."
         echo "Please examine fail_*.txt files in CWD"
      fi
   fi
fi
ENDOFMESSAGE

chmod +x exactness
