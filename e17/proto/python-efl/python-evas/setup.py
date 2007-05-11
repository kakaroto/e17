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
                                'evas/python.pxd',
                                ],
                       **pkgconfig('"evas >= 0.9.9.038"'))

headers = ['evas/evas.c_evas.pxd',
           'evas/python_evas_utils.h',
           ]

setup(name = 'python-evas',
      version = '0.1.0',
      license = 'BSD',
      author='Gustavo Sverzut Barbieri',
      author_email='barbieri@gmail.com',
      url='http://www.enlightenment.org/',
      description = 'Python bindings for Evas',
      packages = find_packages(),
      headers = headers,
      ext_modules = [evasmodule],
      zip_safe=False,
      )
