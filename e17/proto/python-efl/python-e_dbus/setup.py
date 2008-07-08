import sys
import os

from ez_setup import use_setuptools
use_setuptools("0.6c3")

from setuptools import setup, Extension
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


e_dbus_module = Extension("e_dbus",
                          sources=["module.c",],
                          **pkgconfig('"edbus >= 0.1.0.042" '
                                      '"dbus-python"'))


setup(name="python-e_dbus",
      version="0.1.1",
      license="BSD",
      author="Ulisses Furquim",
      author_email="ulisses.silva@openbossa.org",
      url="http://www.enlightenment.org/",
      description="D-Bus python integration for Ecore main loop",
      keywords="d-bus python integration ecore mainloop",
      ext_modules=[e_dbus_module],
      zip_safe=False,
      )
