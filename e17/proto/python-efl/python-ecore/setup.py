import sys
import os

from ez_setup import use_setuptools
use_setuptools('0.6c3')

import distutils.sysconfig
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


ecoremodule = Extension('ecore.c_ecore',
                        sources=['ecore/ecore.c_ecore.pyx'],
                        depends=['ecore/ecore.c_ecore_timer.pxi',
                                 'ecore/ecore.c_ecore_animator.pxi',
                                 'ecore/ecore.c_ecore_idler.pxi',
                                 'ecore/ecore.c_ecore_idle_enterer.pxi',
                                 'ecore/ecore.c_ecore_idle_exiter.pxi',
                                 'ecore/ecore.c_ecore.pxd',
                                 'ecore/python.pxd',
                                 ],
                        **pkgconfig('"ecore >= 0.9.9.038"'))

ecoreevasmodule = Extension('ecore.evas.c_ecore_evas',
                            sources=['ecore/evas/ecore.evas.c_ecore_evas.pyx'],
                            depends=['ecore/evas/ecore.evas.c_ecore_evas.pxd',
                                     'ecore/evas/ecore.evas.c_ecore_evas_base.pxi',
                                     'ecore/evas/ecore.evas.c_ecore_evas_software_x11.pxi',
                                     'ecore/evas/ecore.evas.c_ecore_evas_gl_x11.pxi',
                                     'ecore/evas/ecore.evas.c_ecore_evas_xrender_x11.pxi',
                                     'ecore/evas/ecore.evas.c_ecore_evas_fb.pxi',
#                                     'ecore/evas/ecore.evas.c_ecore_evas_directfb.pxi',
                                     'ecore/evas/ecore.evas.c_ecore_evas_buffer.pxi',
                                     'ecore/python.pxd',
                                     ],
                            **pkgconfig('"ecore-evas >= 0.9.9.038"'))


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
Python bindings for Ecore and Ecore/Evas, part of Enlightenment Foundation Libraries.

Ecore is the core event abstraction layer and X abstraction layer that
makes doing selections, Xdnd, general X stuff, and event loops,
timeouts and idle handlers fast, optimized, and convenient. It's a
separate library so anyone can make use of the work put into Ecore to
make this job easy for applications.

Ecore/Evas binds Evas to its underlying output and event systems, like
X, Framebuffer, DirectFB, OpenGL and possible more, taking care of
converting events to an uniform structure and handling them to
applications, also updating the screen when necessary (expose events,
for instance), toggling fullscreen, setting window shape, border and
other parameters.
"""

setup(name = 'python-ecore',
      version = '0.1.0',
      license = 'BSD',
      author = 'Gustavo Sverzut Barbieri',
      author_email = 'barbieri@gmail.com',
      url = 'http://www.enlightenment.org/',
      description = 'Python bindings for Ecore',
      long_description = long_description,
      keywords = 'wrapper binding enlightenment abstraction event ecore',
      classifiers = trove_classifiers,
      packages = find_packages(),
      install_requires = ['python-evas>=0.1.0'],
      setup_requires = ['python-evas>=0.1.0'],
      ext_modules = [ecoremodule, ecoreevasmodule],
      zip_safe = False,
      )
