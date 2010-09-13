              PYTHON ENLIGHTENMENT FOUNDATION LIBRARIES

Each folder in this directory contain a python binding, also known as
wrapper, that makes it possible to use the Enlightenment Foundation
Libraries (EFL) with the Python language.


BUILD REQUIREMENTS
==================

In order to build modules ensure you have the following installed in
your machine:

 * Cython: converts high level definitions of .pyx into .c to be
   compiled as a loadable module. As of now, version 0.13 is
   required, not older, not newer.
 * Latest build of EFL

NOTE:
There seems to be an annoying bug with distutils in Python 2.6 and maybe later,
where it decides to ignore the --include-dirs from the command line.
To build the bindings you will need to set your CFLAGS to use
-Iprefix/include/python2.6
