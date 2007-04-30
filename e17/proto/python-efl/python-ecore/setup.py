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

ecoremodule = Extension('ecore.c_ecore',
                        sources=['ecore/c_ecore.pyx'],
                        depends=['ecore/c_ecore_timer.pyx',
                                 'ecore/c_ecore_animator.pyx',
                                 'ecore/c_ecore.pxd',
                                 'ecore/python.pxd',
                                 ],
                        **pkgconfig('"ecore >= 0.9.9.037"'))

ecoreevasmodule = Extension('ecore.evas.c_ecore_evas',
                            sources=['ecore/evas/c_ecore_evas.pyx'],
                            depends=['ecore/evas/c_ecore_evas.pxd',
                                     'ecore/evas/c_ecore_evas_base.pyx',
                                     'ecore/evas/c_ecore_evas_software_x11.pyx',
                                     'ecore/evas/c_ecore_evas_gl_x11.pyx',
                                     'ecore/evas/c_ecore_evas_xrender_x11.pyx',
                                     'ecore/evas/c_ecore_evas_fb.pyx',
#                                     'ecore/evas/c_ecore_evas_directfb.pyx',
                                     'ecore/evas/c_ecore_evas_buffer.pyx',
                                     'ecore/python.pxd',
                                     ],
                            **pkgconfig('"ecore-evas >= 0.9.9.037"'))

setup(name = 'python-ecore',
      version = '0.1.0',
      license = 'LGPL',
      description = 'Python bindings for Ecore',
      packages = find_packages(),
      ext_modules = [ecoremodule, ecoreevasmodule],
      )
