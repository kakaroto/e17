import sys
import os

from ez_setup import use_setuptools
use_setuptools('0.6c3')

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


epsilonmodule = Extension('epsilon.c_epsilon',
                          sources=['epsilon/epsilon.c_epsilon.pyx',
                                   ],
                          depends=['include/epsilon/c_epsilon.pxd',
                                   ],
                          **pkgconfig('"epsilon >= 0.3.1.012" ''"eina-0 >= 0.0.1"'))

epsilonrequestmodule = Extension('epsilon.request',
                          sources=['epsilon/epsilon.request.pyx',
                                   ],
                          depends=['include/epsilon/request.pxd',
                                   ],
                          **pkgconfig('"epsilon >= 0.3.1.012" ''"eina-0 >= 0.0.1"'))


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
Python bindings for Epsilon, part of Enlightenment Foundation Libraries.

Epsilon is a small, display independent, and quick thumbnailing library.

The lib itself conforms to the standard put forth by freedesktop.org You
can find out more information about it at
http://triq.net/~jens/thumbnail-spec/index.html.  It seemed better to
break it out into a component that only depended on what was absolutely
necessary.

Epsilon will use EPEG if it was compiled with that support, EPEG is the
fastest JPEG thumbnail library around. This is highly recommended.
"""


class epsilon_build_ext(build_ext):
    def finalize_options(self):
        build_ext.finalize_options(self)
        self.include_dirs.insert(0, 'include')
        self.pyrex_include_dirs.extend(self.include_dirs)


setup(name='python-epsilon',
      version='0.3.1',
      license='LGPL',
      author='Gustavo Sverzut Barbieri',
      author_email='barbieri@gmail.com',
      url='http://www.enlightenment.org/',
      description='Python bindings for Epsilon',
      long_description=long_description,
      keywords='wrapper binding enlightenment graphics jpg jpeg png thumbnail freedesktop.org',
      classifiers=trove_classifiers,
      packages=find_packages(),
      install_requires=['python-ecore>=0.3.1'],
      setup_requires=['python-ecore>=0.3.1'],
      ext_modules=[epsilonmodule, epsilonrequestmodule],
      zip_safe=False,
      cmdclass={'build_ext': epsilon_build_ext,},
      )
