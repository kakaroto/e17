Ensure
======

Validator application for EFL programs.


Setup
-----

For now you need to tweak your ecore_evas:
	/static Ecore_Evas *ecore_evases/s/static//;
Then make and install ecore again.

Building Ensure
---------------

Just a 'Makefile' at the moment, and no install.
    make

Running Ensure
--------------

    ./ensure /path/to/your/app --your --app -args

Using
-----

Once you have run ensure, you can choose the lists of tests you wish to run
with by clicking 'config'.  This will show you all the tests loaded by ensure
sorted by priority.  By default all tests are enabled, and you can disable any
by clicking on the little tick.  One you have selected the tests you can
return to the main list by selecting 'report'.

To run the test program click 'run'.  This will start the new process as a
subprocess of ensure.  Use the program as you normally would to reach the
place you wish to test, then clock 'check' in the ensure window.

This will send a signal (USR2 at the moment) to the child process which will
send back a report of all objects in the application.  Ensure will then run
all enabled tests on the objects, and produce a report.

Each object which has at least one issue will be displayed.  You can click
'View' to view an object, which will display it in a new window.  You can also
display it's clip and parents from the popup window (and their clip/parents
etc).

Adding Assurances
-----------------

Assurances are currently implemented as dynmically loaded objects.  The code
for each is in ${ENSURE}/src/assurances.

To add a new assurance:
 - Copy template.c to the name of the assurance you wish to write.
 - Choose a severity for the problem.  Critical are for issues that are likely
   to cause a crash, or the application to misbehave.  Bugs are for things
   that are wrong, but won't cause a crash.  Bad form is for things that
   should not be done, but that are handled okay by the engine.  Pedantic
   problems are for minor issues that work fine, but people prefer it if not
   done.  Finally local policy is for application or organisation specific
   rules: Perfect for rules like 'all text must be green' or similar.
 - Pick a name for the rule, and the summary.  Write these in your assurance
   file in struct assurances.
 - If you rule needs some iniitialisation code, write that in the init
   function.  The data returned from init will be passed to the main check
   function and the fini function.
 - Write the main rule.  This rule will be called once for each object in the
   system.  If it reports a bug it should return 1, else 0.  To report a bug
   call ensure_bug().  This takes the object with the bug (not necessarily the
   object passed to object_check), the class of bug, and a printf style format
   string and arguments.
 - Finally if you need a fini function to clean up anything write that.
 - Add your assurance to the Makefile list of assurances.
 - Recompile
 - Run ensure and try your new test.



Tests (Assurances)
------------------

Currently checks for:

  * Object of size 0
  * Invalid colour
     - A > max(r,g,b)
     - A == 0 (invisible)
  * Text with trailing spaces
  * Object of negative size
  * Object off screen
  * Object outside it's clip
  * Object & clip are both smart members
  * Empty text
  * Font 5 pts or less
  * Font 100 pts of more
  * Object outside of smart object
  * Object clipped to a smart's clip, but not a member
  * Edje or image objects with load errors


Future Work
-----------

More tests:

  * Object not clipped to it's smart parent (subtly different)
  * Objects of size 1x1: Probably default not resized
  * Objects of size 32x32: Probably a smart object not resized
  * Smart clip's size != smart object's size
  * Scaled images

Other:

  * Allow multiple check runs in one test run
  * Allow signaling through other means (sockets etc) so tests can be run
    remotely.
  * More details about objects, for richer testing
  * Improved appearance: Icons etc
  * Support multiple child windows.


