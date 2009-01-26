#!/usr/bin/env python

from ez_setup import use_setuptools
use_setuptools('0.6c3')

from setuptools import setup, find_packages

trove_classifiers = [
    "Development Status :: 3 - Alpha",
    "Environment :: Console :: Framebuffer",
    "Environment :: X11 Applications",
    "Intended Audience :: Developers",
    "License :: OSI Approved :: GNU Library or Lesser General Public License (LGPL)",
    "Operating System :: MacOS :: MacOS X",
    "Operating System :: POSIX",
    "Programming Language :: C",
    "Programming Language :: Python",
    "Topic :: Software Development :: Libraries :: Python Modules",
    "Topic :: Software Development :: User Interfaces",
    ]

long_description = """\
Utilities to aid development of EFL applications.

These utilities include transition effects, timeline animators and more.
"""

setup(name = 'python-efl-utils',
      version = '0.1.0',
      license = 'LGPL',
      author='Gustavo Sverzut Barbieri',
      author_email='barbieri@gmail.com',
      url='http://www.enlightenment.org/',
      description = 'Python utilities to EFL development',
      long_description = long_description,
      keywords = 'enlightenment graphics raster evas canvas utilities efl',
      classifiers = trove_classifiers,
      packages = find_packages(),
      zip_safe=True,
      )

