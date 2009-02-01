              PYTHON ENLIGHTENMENT FOUNDATION LIBRARIES

Each folder in this directory contain a python binding, also known as
wrapper, that makes it possible to use the Enlightenment Foundation
Libraries (EFL) with the Python language.


BUILD REQUIREMENTS
==================

In order to build modules ensure you have the following installed in
your machine:

 * Pyrex: even if we use Cython, python's setuptools/distutils checks
   for Pyrex in order to convert .pyx -> .c in extensions.
 * Cython: converts high level definitions of .pyx into .c to be
   compiled as a loadable module.
 * Latest build of EFL


