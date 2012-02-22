#!/bin/sh
# This script creates exactness script that later is copied to $(bindir)
# by Makefile.
# The reason to do it this was is that we would like to get $OUR_LIBPATH as
# a parameter from Makefile.
cat <<-ENDOFMESSAGE>exactness
#!/usr/bin/env bash
# tsuite_script.sh -i this makes new 'orig' folder
# tsuite_script.sh -i -b [BaseDir] TestName1 [TestName2 ...] rewrite files for selcted tests in 'orig' folder
# tsuite_script.sh -r -b [BaseDir] [TestName1 TestName2 ...] ; this means record [all tests or TestName additional arg]
# tsuite_script.sh -b [BaseDir] -d FolderName -p [TestName1 TestName2 ...] this means play a record and put screenshots FolderName
# tsuite_script.sh -d FolderName -c [TestName1 TestName2 ...] this means compare "orig" with screenshots in FolderName
# When omitting DestDir we will use 'current' as default DestDir

#_DEBUG="on"
function DEBUG()
{
   [ "\$_DEBUG" == "on" ] &&  \$@
}

do_help () {
echo "Use \$0 to test application screen-layout."
echo "First, you need to compose a tests file as follows:"
echo "Each line begins with test name"
echo "second field is test-command and [optional] params."
echo "Any line starting with '#' is a comment (ignored):"
echo
echo "# This is a comment line"
echo "TestName TestCmd [param1] [param2]"
echo
echo "Later, you run \$0 with the tests file as parameter."
echo
echo "By default, exactness runs through test file running all tests specified."
echo "You may run selected tests by adding test name as param to exactness."
echo "Usage:"
echo "\$0 -s TestsFile TestName1 [TestName2] [...]"
echo "Use this option to run selected tests without modifying your test file."
echo "TestName param has to match test name given in tests file (1st field)"
echo
echo
echo "Two additional parameters that \$0 accepts:"
echo "BaseDir - This is where '.rec' files reside (gets pwd by default)"
echo "DestDir - Where \$0 creates test screen shots."
echo "          Gets 'current' under 'pwd' by default ('orig' on init)"
echo
echo
echo "Use the following options:"
echo "To record tests:"
echo "\$0 -r [-b BaseDir] TestsFile"
echo "Use BaseDir arg to create record files in specific folder."
echo "Otherwise pwd is used."
echo
echo "Pressing F2 while recording, sets screen shot at this stage of test."
echo "You may define env-var 'TSUITE_SHOT_KEY' to alter shot-key."
echo "'.rec' file is produced for each test in your TestsFile."
echo "File name is defined as 'TestName.rec' for each test."
echo
echo "You may test your record files with simulate option:"
echo "\$0 -s [-b BaseDir] TestsFile"
echo
echo "You need to run \$0 with init option prior"
echo "to using play option."
echo "Later, when doing play, PNG files are compared with"
echo "PNG files reside in 'orig' folder create when init."
echo
echo "To use init option:"
echo "\$0 -i [-b BaseDir] TestsFile"
echo "Do not use DestDir param with init, target always 'orig'."
echo
echo "Use Play tests option to produce PNG files of screen shot:"
echo "\$0 -p [-b BaseDir] [-d DestDir] TestsFile"
echo "Play option produces PNG files in DestDir."
echo "These are compares with PNGs in 'orig'."
echo "(created in 'init' phase)"
echo
echo "Use -v option for detailed flow-report."
echo "Thus, when running many tests, the output format makes"
echo "it easy to match output to a running test."
echo "Example:"
echo "\$0 -v -p [-b BaseDir] [-d DestDir] TestsFile"
}

get_test_params () {
# This function analyze input line and sets test-file-name, rec-file-name
# reset globals
_test_name=
_test_cmd=
local line="\$1"
local c=\${line:0:1}
if [ "\$c" = "#" ]
then
# This line is a comment
   return 1
fi

local p=\`expr index "\$line" \\ \`
if [ \$p -ne 0 ]
then
   (( p-- ))
fi
_test_name=\${line:0:p}
(( p++ ))
_test_cmd=\${line:p}

# Test that input is valid
if [ -z "\$_test_name" ]
then
   _test_name=
   _test_cmd=
   return 1
fi

if [ -z "\$_test_cmd" ]
then
   _test_name=
   _test_cmd=
   return 1
fi

DEBUG echo test name=\""\$_test_name"\"
DEBUG echo test cmd=\""\$_test_cmd"\"
return 0
}

do_record () {
DEBUG printf "do_record()\n"
# This will run record for all test if no specific test specified
# or run recording of specified tests (names passed as parameter).
# run ALL tests to record
DEBUG echo do_record "\$*"
get_test_params "\$1"
if [ \$? -ne 0 ]
then
   return 0
fi

if [ "\$_verbose" -ne 0 ]
then
   echo "do_record: \$_test_name"
fi
TSUITE_RECORDING='rec' TSUITE_BASE_DIR=\${_base_dir} TSUITE_DEST_DIR=\${_dest_dir} TSUITE_FILE_NAME=\${_base_dir}/\${_test_name}.rec TSUITE_TEST_NAME=\${_test_name} LD_PRELOAD=\${OUR_LIBPATH}/libexactness.so eval \${_test_cmd}
}

do_simulation () {
# This will play simulation
# this will NOT produce screenshots
DEBUG echo do_simulation "\$*"
get_test_params "\$1"
if [ \$? -ne 0 ]
then
   return 0
fi

local file_name=\${_base_dir}/\${_test_name}.rec

if [ ! -e "\$file_name" ]
then
   echo Rec file "\$file_name" not found.
   return 1
fi


if [ "\$_verbose" -ne 0 ]
then
   echo "do_simulation: \$_test_name"
fi
TSUITE_BASE_DIR=\${_base_dir} TSUITE_DEST_DIR=\${_dest_dir} TSUITE_FILE_NAME=\${file_name} TSUITE_TEST_NAME=\${_test_name} LD_PRELOAD=\${OUR_LIBPATH}/libexactness.so eval \${_test_cmd}
}

do_play () {
# This will play record for all test if '-a' specified.
# or play record of tests specified as parameter.
# run ALL tests to record
DEBUG echo base dir: "\$_base_dir"
DEBUG echo dest dir: "\$_dest_dir"
DEBUG echo do_play "\$_dest_dir" "\$*"
# Play recorded tests and produce PNG files.
# this will produce screenshots in "_dest_dir" folder
get_test_params "\$1"
if [ \$? -ne 0 ]
then
   return 0
fi

local file_name=\${_base_dir}/\${_test_name}.rec

if [ ! -e "\$file_name" ]
then
   echo Rec file "\$file_name" not found.
   return 1
fi

if [ -e "\$_dest_dir" ]
then
# Remove PNG files according to tests played
   rm "\$_dest_dir"/\${_test_name}_[0-9]*.png &> /dev/null
else
# Create dest dir
   mkdir -p "\$_dest_dir" &> /dev/null
fi

if [ "\$_verbose" -ne 0 ]
then
   echo "do_play: \$_test_name"
fi
ELM_ENGINE="buffer" TSUITE_BASE_DIR=\${_base_dir} TSUITE_DEST_DIR=\${_dest_dir} TSUITE_FILE_NAME=\${file_name} TSUITE_TEST_NAME=\${_test_name} LD_PRELOAD=\${OUR_LIBPATH}/libexactness.so eval \${_test_cmd}
}

compare_files () {
if [ "\$_verbose" -ne 0 ]
then
   echo "compare_files: <\$1> and <\$2>"
fi

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
DEBUG echo orig dir: "\$_orig_dir"
DEBUG echo dest dir: "\$_dest_dir"
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
for test_name in \$*
do
   rm "\$_dest_dir"/comp_"\$test_name"_[0-9]*.png &> /dev/null
   files_list=( \`ls "\$_dest_dir"/"\$test_name"_[0-9]*.png\` )
   process_compare "\${files_list[@]}"
done

if [ "\$ncomp" -ne 0 ]
then
   echo "Processed \$ncomp comparisons"
fi

if [ "\$nfail" -ne 0 ]
then
   echo "with \$nfail diff errors"
fi

if [ "\$nerr" -ne 0 ]
then
   echo "\$nerr PNG-files were not found"
fi

return 0
}

name_in_args () {
# This function gets curline as first arg
# Then list of args to find if test name is first field of curline
get_test_params "\$1"
if [ \$? -ne 0 ]
then
   return 0
fi

if [ -z "\$_test_name" ]
then
   return 0
fi

shift
while (( "\$#" ));
do
   if [ "\$_test_name" = "\$1" ]
# found test name in list of args
   then
      return 1
   fi

   shift
done

# Not found
return 0
}

# Script Entry Point
OUR_LIBPATH="$1"

_verbose=0
_record=
_play=
_compare=
_init=
_simulation=
_remove_fail=
_orig_dir="orig"
# Init dest_dir - should change on the fly
_dest_dir=
_test_all=1
_base_dir=\`pwd\`
_test_name=
_test_cmd=

nerr=0
ncomp=0
nfail=0
_n_exe_err=0

# Test that compare is insatlled
which compare &> /dev/null
comp_unavail=\$?

while getopts 'ab:cd:hprisv?' OPTION
do
   case \$OPTION in
      b)  _base_dir="\$OPTARG"
         ;;
      c)  _compare=1
         ;;
      d)  _dest_dir="\$OPTARG"
         ;;
      p)  _play=1
          _compare=1
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
      s)  _dest_dir="\$_orig_dir"
         _simulation=1
         ;;
      h)  do_help
         exit 0
         ;;
      v)  _verbose=1
         ;;
      ?)  do_help
         exit 0
         ;;
   esac
done
shift \$((\$OPTIND - 1))

_test_file_name="\$1"
shift

# Test if user added test-names as arguments
if [ ! -z "\$*" ]
then
   _test_all=0
fi

# when using -o option, we can loop now on tests names
# given as arguments to this script

DEBUG echo _test_file_name="\$_test_file_name"
DEBUG echo _base_dir="\$_base_dir"
DEBUG echo _dest_dir="\$_dest_dir"


if [ "\$_simulation" ]
then
# When in simulation mode, we will just commit play (ignore other options)
   _init=
   _record=
   _compare=
   _remove_fail=
   _play=
   while read curline;
   do
      name_in_args "\$curline" \$*
      _run_test=\$(( \$? + \$_test_all ))
      if [ \$_run_test -ne 0 ]
      then
         do_simulation "\$curline"
         if [ \$? -ne 0 ]
         then
            (( _n_exe_err++ ))
         fi
      fi
   done < "\$_test_file_name"
# This will cause render simulation
fi

if [ ! -e "\$_base_dir" ]
then
   echo "Base dir <\$_base_dir> - not found."
   exit 1
fi

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
   while read curline;
   do
      name_in_args "\$curline" \$*
      _run_test=\$(( \$? + \$_test_all ))
      if [ \$_run_test -ne 0 ]
      then
         do_record "\$curline"
         if [ \$? -ne 0 ]
         then
            (( _n_exe_err++ ))
         fi
      fi
   done < "\$_test_file_name"
fi

if [ "\$_play" ]
then
   while read curline;
   do
      name_in_args "\$curline" \$*
      _run_test=\$(( \$? + \$_test_all ))
      if [ \$_run_test -ne 0 ]
      then
         do_play "\$curline"
         if [ \$? -ne 0 ]
         then
            (( _n_exe_err++ ))
         fi
      fi
   done < "\$_test_file_name"
fi

if [ "\$_compare" ]
then
   while read curline;
   do
      name_in_args "\$curline" \$*
      _run_test=\$(( \$? + \$_test_all ))
      if [ \$_run_test -ne 0 ]
      then
         get_test_params "\$curline"
         if [ \$? -eq 0 ]
         then
            do_compare "\$_test_name"
         fi
      fi
   done < "\$_test_file_name"
fi

_n_tests_failed=0
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

# Add up total-error and emit user message.
total_errors=\$(( \$nfail + \$nerr + \$_n_tests_failed + \$_n_exe_err ))
echo "\$_n_exe_err tests returned exit-code <> 0"
echo "\$0 finished with \$total_errors errors."

status=0
# Compute exit code
if [ "\$nfail" -ne 0 ]
then
   status=\$(( \$status | 1 ))
fi

if [ "\$nerr" -ne 0 ]
then
   status=\$(( \$status | 2 ))
fi

if [ "\$_n_tests_failed" -ne 0 ]
then
   status=\$(( \$status | 4 ))
fi

exit \$status
ENDOFMESSAGE

chmod +x exactness
