import sys
import os

from ez_setup import use_setuptools
use_setuptools('0.6c9')

from setuptools import setup, find_packages, Extension
import subprocess
import shlex

from Cython.Distutils import build_ext

def getstatusoutput(cmdline):
    cmd = shlex.split(cmdline)
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE)
    out, err = p.communicate()
    return p.returncode, out


def pkgconfig(*packages, **kw):
    flag_map = {'-I': 'include_dirs', '-L': 'library_dirs', '-l': 'libraries',
                '-D': 'prepro_vars'}
    pkgs = ' '.join(packages)
    cmdline = 'pkg-config --libs --cflags %s' % pkgs

    status, output = getstatusoutput(cmdline)
    if status != 0:
        raise ValueError("could not find pkg-config module: %s" % pkgs)

    for token in output.split():
        flag  = flag_map.get(token[:2], None)
        if flag is not None:
            kw.setdefault(flag, []).append(token[2:])
        elif token.startswith("-Wl,"):
            kw.setdefault("extra_link_args", []).append(token)
        else:
            kw.setdefault("extra_compile_args", []).append(token)

    if "extra_link_args" in kw:
        print "Using extra_link_args: %s" % " ".join(kw["extra_link_args"])
    if "extra_compile_args" in kw:
        print "Using extra_compile_args: %s" % " ".join(kw["extra_compile_args"])

    return kw


edjemodule = Extension('edje.c_edje',
                       sources=['edje/edje.c_edje.pyx',
                                ],
                       depends=['edje/edje.c_edje_object.pxi',
                                'edje/edje.c_edje_message.pxi',
                                'include/edje/c_edje.pxd',
                                ],
                       **pkgconfig('"edje >= 0.9.9.050"'))


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


class edje_build_ext(build_ext):
    def finalize_options(self):
        build_ext.finalize_options(self)
        self.include_dirs.insert(0, 'include')
        self.pyrex_include_dirs.extend(self.include_dirs)


setup(name='python-edje',
      version='0.3.1',
      license='LGPL',
      author='Gustavo Sverzut Barbieri',
      author_email='barbieri@gmail.com',
      url='http://www.enlightenment.org/',
      description='Python bindings for Edje',
      long_description=long_description,
      keywords='wrapper binding enlightenment graphics raster evas canvas theme',
      classifiers=trove_classifiers,
      packages=find_packages(),
      install_requires=['python-evas>=0.3.1'],
      setup_requires=['python-evas>=0.3.1'],
      ext_modules=[edjemodule],
      zip_safe=False,
      cmdclass={'build_ext': edje_build_ext,},
      )
