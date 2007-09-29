import sys
import os

from ez_setup import use_setuptools
use_setuptools("0.6c3")

from setuptools import setup, Extension
import commands


def pkgconfig(*packages, **kw):
    flag_map = {"-I": "include_dirs", "-L": "library_dirs", "-l": "libraries"}
    pkgs = " ".join(packages)
    cmdline = "pkg-config --libs --cflags %s" % pkgs

    status, output = commands.getstatusoutput(cmdline)
    if status != 0:
        raise ValueError("could not find pkg-config module: %s" % pkgs)

    for token in output.split():
        kw.setdefault(flag_map.get(token[:2]), []).append(token[2:])
    return kw


e_dbus_module = Extension("e_dbus",
                          sources=["module.c",],
                          **pkgconfig('"edbus >= 0.1.0.003"'))


setup(name="python-e_dbus",
      version="0.1.0",
      license="BSD",
      author="Ulisses Furquim",
      author_email="ulisses.silva@openbossa.org",
      url="http://www.enlightenment.org/",
      description="D-Bus python integration for Ecore main loop",
      keywords="d-bus python integration ecore mainloop",
      ext_modules=[e_dbus_module],
      zip_safe=False,
      )
