import sys
import os

from ez_setup import use_setuptools
use_setuptools('0.6c3')

import distutils.sysconfig
import pkg_resources
from setuptools import setup, find_packages, Extension
import commands


python_inc = distutils.sysconfig.get_python_inc()


def pkgconfig(*packages, **kw):
    flag_map = {'-I': 'include_dirs', '-L': 'library_dirs', '-l': 'libraries'}
    pkgs = ' '.join(packages)
    cmdline = 'pkg-config --libs --cflags %s' % pkgs

    status, output = commands.getstatusoutput(cmdline)
    if status != 0:
        raise ValueError("could not find pkg-config module: %s" % pkgs)

    for token in output.split():
        kw.setdefault(flag_map.get(token[:2]), []).append(token[2:])
    return kw


edjemodule = Extension('edje.c_edje',
                       sources=['edje/edje.c_edje.pyx',
                                ],
                       depends=['edje/edje.c_edje.pxd',
                                'edje/python.pxd',
                                ],
                       **pkgconfig('"edje >= 0.5.0.038"'))

include_dirs = [os.path.join(python_inc, "python-evas")]


trove_classifiers = [
    "Development Status :: 3 - Alpha",
    "Environment :: Console :: Framebuffer",
    "Environment :: X11 Applications",
    "Intended Audience :: Developers",
    "License :: OSI Approved :: BSD License",
    "Operating System :: MacOS :: MacOS X",
    "Operating System :: POSIX",
    "Programming Language :: C",
    "Programming Language :: Python",
    "Topic :: Software Development :: Libraries :: Python Modules",
    "Topic :: Software Development :: User Interfaces",
    ]

long_description = """\
Python bindings for Edje, part of Enlightenment Foundation Libraries.

Edje is a complex graphical design & layout library, designed to be
used with "Evas" canvas system, also part of EFL.

It doesn't pretend to do containering and regular layout like a widget
set. For the purposes of Enlightenment 0.17, Edje should serve all the
purposes of creating visual elements (borders of windows, scrollbars,
etc.) and allow the designer the ability to animate, layout and
control the look and feel of any program using Edje as its basic GUI
constructor. This library allows for multiple collections of layouts
in one file, sharing the same image database and thus allowing a whole
theme to be conveneintly packaged into 1 file and shipped around.

Edje separates the layout and behavior logic. Edje files ship with an
image database, used by all the parts in all the collections to source
graphical data. It has a directory of logical part names pointing to
the part collection entry ID in the file (thus allowing for multiple
logical names to point to the same part collection, allowing for the
sharing of data betwene display elements). Each part collection
consists of a list of visual parts, as well as a list of programs. A
program is a conditionally run program that if a particular event
occurs (a button is pressed, a mouse enters or leaves a part) will
trigger an action that may affect other parts. In this way a part
collection can be "programmed" via its file as to hilight buttons when
the mouse passes over them or show hidden parts when a button is
clicked somewhere etc. The actions performed in changing from one
state to another ar also allowed to transition over a period of time,
allowing animation.

This separation and simplistic event driven style of programming can
produce almost any look and feel one could want for basic visual
elements. Anything more complex is likely the domain of an application
or widget set that may use Edje as a convenient way of being able to
configure parts of the display.

Edje files may contain small scripts written in Embryo (a variant of
C, originally called Small, later renamed to PAWN).
"""


setup(name = 'python-edje',
      version = '0.1.0',
      license = 'BSD',
      author = 'Gustavo Sverzut Barbieri',
      author_email = 'barbieri@gmail.com',
      url = 'http://www.enlightenment.org/',
      description = 'Python bindings for Edje',
      long_description = long_description,
      keywords = 'wrapper binding enlightenment graphics raster evas canvas theme',
      classifiers = trove_classifiers,
      packages = find_packages(),
      install_requires = ['python-evas>=0.1.0'],
      setup_requires = ['python-evas>=0.1.0'],
      include_dirs = include_dirs,
      ext_modules = [edjemodule],
      zip_safe = False,
      )
