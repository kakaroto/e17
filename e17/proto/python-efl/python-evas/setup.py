import sys
import os

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


def pkgconfig_version(package):
    return commands.getoutput('pkg-config --modversion %s' % package).strip()


evasmodule = Extension('evas.c_evas',
                       sources=['evas/c_evas.pyx',
                                ],
                       depends=['evas/c_evas.pxd',
                                'evas/c_evas_canvas.pyx',
                                'evas/c_evas_object.pyx',
                                'evas/c_evas_object_rectangle.pyx',
                                'evas/c_evas_object_line.pyx',
                                'evas/c_evas_object_image.pyx',
                                'evas/python.pxd',
                                ],
                       **pkgconfig('"evas >= 0.9.9.037"'))


setup(name = 'python-evas',
      version = '0.1.0',
      license = 'LGPL',
      description = 'Python bindings for Evas',
      packages = find_packages(),
      ext_modules = [evasmodule],
      )
