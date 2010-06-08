import sys
import os

try:
    from Cython.Distutils import build_ext
    # work around stupid setuptools that insists on just checking pyrex
    sys.modules['Pyrex'] = sys.modules['Cython']
    have_cython = True
except ImportError:
    have_cython = False

if not have_cython and not os.path.exists("elementary/elementary.c_elementary.c"):
    raise SystemExit("You need Cython -- http://cython.org/")

from ez_setup import use_setuptools
use_setuptools('0.6c9')

if not have_cython:
    print "No cython installed, using existing generated C files."
    from setuptools.command.build_ext import build_ext

from setuptools import setup, find_packages, Extension
import subprocess
import shlex

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

depends = ['include/elementary/c_elementary.pxd']
for root, dirs, files in os.walk('elementary'):
    for file in files:
        if file.endswith('.pxi'):
            depends.append('elementary/' + file)

elementary_mod = Extension('elementary.c_elementary',
                           sources=['elementary/elementary.c_elementary.pyx'],
                           depends=depends,
                           **pkgconfig('"elementary >= 0.7.0.49539"'))


class elementary_build_ext(build_ext):
    def finalize_options(self):
        build_ext.finalize_options(self)
        self.include_dirs.insert(0, 'include')
        if hasattr(self, "pyrex_include_dirs"):
            self.pyrex_include_dirs.extend(self.include_dirs)


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
Python bindings for Elementary, part of Enlightenment Foundation Libraries.

Elementary is a widget set. It is a new-style of widget set much more
canvas object based than anything else. Why not ETK? Why not EWL? Well
they both tend to veer away from the core of Evas, Ecore and Edje a
lot to build their own worlds. Also I wanted something focused on
embedded devices - specifically small touchscreens. Unlike GTK+ and
Qt, 75% of the 'widget set' is already embodied in a common core -
Ecore, Edje, Evas etc. So this fine-grained library splitting means
all of this is shared, just a new widget 'personality' is on top. And
that is...

Elementary, my dear Watson. Elementary.
"""

setup(
    name='python-elementary',
    version='0.5.0',
    license='LGPL',
    author='Simon Busch',
    author_email='morphis@gravedo.de',
    url='http://www.freeesmartphone.org',
    description='Python bindings for Elementary',
    long_description='',
    keywords='wrapper bindings ui elementary graphics',
    packages=find_packages(),
    classifiers=trove_classifiers,
    install_requires=['python-evas>=0.5.0'],
    setup_requires=['python-evas>=0.5.0'],
    zip_safe=False,
    cmdclass={"build_ext": elementary_build_ext},
    ext_modules=[elementary_mod]
)
