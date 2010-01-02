import sys
import os

if not os.path.exists("eupnp/eupnp.c_eupnp.c"):
    try:
        import Cython
    except ImportError:
        raise SystemExit("You need Cython -- http://cython.org/")
    try:
        import Pyrex
    except ImportError:
        raise SystemExit(
            "You need Pyrex -- "
            "http://www.cosc.canterbury.ac.nz/greg.ewing/python/Pyrex/")

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


eupnpmodule = Extension('eupnp.c_eupnp',
                       sources=['eupnp/eupnp.c_eupnp.pyx',
                                ],
                       depends=['include/eupnp/c_eupnp.pxd',
                                ],
                       **pkgconfig('"eupnp-0"'))

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
Python bindings for eupnp, part of Enlightenment Foundation Libraries.
"""


class eupnp_build_ext(build_ext):
    def finalize_options(self):
        build_ext.finalize_options(self)
        self.include_dirs.insert(0, 'include')
        self.pyrex_include_dirs.extend(self.include_dirs)


setup(name='python-eupnp',
      version='0.0.1',
      license='LGPL',
      author='Andre Dieb Martins',
      author_email='andre.dieb@gmail.com',
      url='http://trac.enlightenment.org/e/wiki/Eupnp',
      description='Python bindings for eupnp',
      long_description=long_description,
      keywords='wrapper binding eupnp upnp',
      classifiers=trove_classifiers,
      packages=find_packages(),
      install_requires=[],
      setup_requires=[],
      ext_modules=[eupnpmodule],
      zip_safe=False,
      cmdclass={'build_ext': eupnp_build_ext,},
      )
