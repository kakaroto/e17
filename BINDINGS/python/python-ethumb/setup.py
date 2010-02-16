import sys
import os

try:
    from Cython.Distutils import build_ext
    # work around stupid setuptools that insists on just checking pyrex
    sys.modules['Pyrex'] = sys.modules['Cython']
    have_cython = True
except ImportError:
    have_cython = False

if not have_cython and not os.path.exists("ethumb/ethumb.c_ethumb.c"):
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


ethumbmodule = Extension('ethumb.c_ethumb',
                          sources=['ethumb/ethumb.c_ethumb.pyx',
                                   ],
                          depends=['include/ethumb/c_ethumb.pxd',
                                   ],
                          **pkgconfig('"ethumb >= 0.1"'))


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
Python bindings for Ethumb, part of Enlightenment Foundation Libraries.

Ethumb is a small, display independent, and quick thumbnailing library.

The lib itself conforms to the standard put forth by freedesktop.org You
can find out more information about it at
http://triq.net/~jens/thumbnail-spec/index.html.  It seemed better to
break it out into a component that only depended on what was absolutely
necessary.

Ethumb uses Evas to create thumbnails.
"""


class ethumb_build_ext(build_ext):
    def finalize_options(self):
        build_ext.finalize_options(self)
        self.include_dirs.insert(0, 'include')
        if hasattr(self, "pyrex_include_dirs"):
            self.pyrex_include_dirs.extend(self.include_dirs)


module_list = [ethumbmodule]

if int(os.environ.get("ETHUMB_BUILD_CLIENT", 1)):
    ethumbclientmodule = Extension(
        'ethumb.client',
        sources=['ethumb/ethumb.client.pyx'],
        depends=['include/ethumb/c_ethumb.pxd',
                 'include/ethumb/client.pxd',
                 ],
        **pkgconfig('"ethumb >= 0.1" "ethumb_client >= 0.1"'))
    module_list.append(ethumbclientmodule)
else:
    print "NOTICE: not building ethumb.client module as requested " \
          "by ETHUMB_BUILD_CLIENT=0!"

setup(name='python-ethumb',
      version='0.2',
      license='LGPL',
      author='Rafael Antognolli',
      author_email='antognolli@profusion.mobi',
      url='http://www.enlightenment.org/',
      description='Python bindings for Ethumb',
      long_description=long_description,
      keywords='wrapper binding enlightenment graphics jpg jpeg png thumbnail freedesktop.org',
      classifiers=trove_classifiers,
      packages=find_packages(),
      ext_modules=module_list,
      zip_safe=False,
      cmdclass={'build_ext': ethumb_build_ext,},
      )
