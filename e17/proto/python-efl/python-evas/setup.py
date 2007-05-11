import sys
import os

from ez_setup import use_setuptools
use_setuptools('0.6c3')

from setuptools import setup, find_packages, Extension
import commands


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


evasmodule = Extension('evas.c_evas',
                       sources=['evas/evas.c_evas.pyx',
                                ],
                       depends=['evas/evas.c_evas.pxd',
                                'evas/evas.c_evas_canvas.pxi',
                                'evas/evas.c_evas_object.pxi',
                                'evas/evas.c_evas_object_smart.pxi',
                                'evas/evas.c_evas_object_rectangle.pxi',
                                'evas/evas.c_evas_object_line.pxi',
                                'evas/evas.c_evas_object_image.pxi',
                                'evas/evas.c_evas_object_gradient.pxi',
                                'evas/evas.c_evas_object_polygon.pxi',
                                'evas/python.pxd',
                                ],
                       **pkgconfig('"evas >= 0.9.9.038"'))

headers = ['evas/evas.c_evas.pxd',
           'evas/python_evas_utils.h',
           ]

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
Python bindings for Evas, part of Enlightenment Foundation Libraries.

Evas is a hardware-accelerated canvas API for X-Windows that can draw
anti-aliased text, smooth super and sub-sampled images, alpha-blend,
as well as drop down to using normal X11 primitives such as pixmaps,
lines and rectangles for speed if your CPU or graphics hardware are
too slow.

Evas abstracts any need to know much about what the characteristics of
your XServer's display are, what depth or what magic visuals etc, it
has. The most you need to tell Evas is how many colors (at a maximum)
to use if the display is not a truecolor display. By default it is
suggested to use 216 colors (as this equates to a 6x6x6 color cube -
exactly the same color cube Netscape, Mozilla, gdkrgb etc. use so
colors will be shared). If Evas can't allocate enough colors it keeps
reducing the size of the color cube until it reaches plain black and
white. This way, it can display on anything from a black and white
only terminal to 16 color VGA to 256 color and all the way up through
15, 16, 24 and 32bit color.
"""

setup(name = 'python-evas',
      version = '0.1.0',
      license = 'BSD',
      author='Gustavo Sverzut Barbieri',
      author_email='barbieri@gmail.com',
      url='http://www.enlightenment.org/',
      description = 'Python bindings for Evas',
      long_description = long_description,
      keywords = 'wrapper binding enlightenment graphics raster evas canvas',
      classifiers = trove_classifiers,
      packages = find_packages(),
      headers = headers,
      ext_modules = [evasmodule],
      zip_safe=False,
      )
