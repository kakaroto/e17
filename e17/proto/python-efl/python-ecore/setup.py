import sys
import os

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
                                 'ecore/ecore.c_ecore.pxd',
                                 'ecore/python.pxd',
                                 ],
                        **pkgconfig('"ecore >= 0.9.9.037"'))

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
                            **pkgconfig('"ecore-evas >= 0.9.9.037"'))


headers = ['ecore/ecore.c_ecore.pxd',
           'ecore/evas/ecore.evas.c_ecore_evas.pxd',
           ]

include_dirs = [os.path.join(python_inc, "python-evas")]


setup(name = 'python-ecore',
      version = '0.1.0',
      license = 'BSD',
      description = 'Python bindings for Ecore',
      packages = find_packages(),
      install_requires = ['python-evas>=0.1.0'],
      setup_requires = ['python-evas>=0.1.0'],
      headers = headers,
      include_dirs = include_dirs,
      ext_modules = [ecoremodule, ecoreevasmodule],
      )
