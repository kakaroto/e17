import sys
import os

from ez_setup import use_setuptools
use_setuptools('0.6c3')

from setuptools import setup, find_packages, Extension
from distutils.command.install_headers import install_headers
from distutils.sysconfig import get_python_inc
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


evasmodule = Extension('evas.c_evas',
                       sources=['evas/evas.c_evas.pyx',
                                'evas/evas_object_image_rotate.c',
                                'evas/evas_object_image_mask.c'
                                ],
                       depends=['evas/evas.c_evas_canvas.pxi',
                                'evas/evas.c_evas_object.pxi',
                                'evas/evas.c_evas_object_smart.pxi',
                                'evas/evas.c_evas_object_rectangle.pxi',
                                'evas/evas.c_evas_object_line.pxi',
                                'evas/evas.c_evas_object_image.pxi',
                                'evas/evas.c_evas_object_gradient.pxi',
                                'evas/evas.c_evas_object_polygon.pxi',
                                'evas/evas.c_evas_object_text.pxi',
                                'include/evas/c_evas.pxd',
                                'include/evas/python.pxd',
                                ],
                       **pkgconfig('"evas >= 0.9.9.043"')
                       )


headers = ['evas/evas.c_evas.h',
           'include/evas/evas_object_image_python_extras.h',
           'include/evas/python_evas_utils.h',
           'include/evas/c_evas.pxd',
           'include/evas/python.pxd',
           'include/evas/__init__.py',
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


class evas_build_ext(build_ext):
    def finalize_options(self):
        build_ext.finalize_options(self)
        self.include_dirs.insert(0, 'include')
        self.pyrex_include_dirs.extend(self.include_dirs)


class evas_install_headers(install_headers):
    def finalize_options(self):
        if self.install_dir is None:
            instd = get_python_inc()
        else:
            instd = self.install_dir

        self.install_dir = os.path.join(instd, 'evas')
        install_headers.finalize_options(self)


setup(name='python-evas',
      version='0.3.0',
      license='LGPL',
      author='Gustavo Sverzut Barbieri',
      author_email='barbieri@gmail.com',
      url='http://www.enlightenment.org/',
      description='Python bindings for Evas',
      long_description=long_description,
      keywords='wrapper binding enlightenment graphics raster evas canvas',
      classifiers=trove_classifiers,
      packages=find_packages(),
      headers=headers,
      ext_modules=[evasmodule],
      zip_safe=False,
      cmdclass={'build_ext': evas_build_ext,
                'install_headers': evas_install_headers},
      )
