# -*- python -*-
# ex: set syntax=python:

import engine
from engine import Package

# add description of packages and dependencies here
# Packages() is defined in engine.py in this same folder.
#
# Package parameters:
#    name: string name of the package
#    dependencies: hash of list of strings, hash key is the platform
#       or "common" for the always-used dependencies
#    configure_flags: hash of list of strings, hash key is the platform
#       or "common" for the always-used configure options
#    env: hash of hash of strings, parent hash key is the platform or
#       "common" for the always-used environment variables.
#    doc_target: make target to build docs
#    test_target: make target to build and run tests
#    exclusive_platforms: list of platforms to limit this package,
#        no list means it will be used in all platforms
#

engine.platforms_set(["win", "linux"])
engine.env_default_set({
    "common": {
        "CFLAGS": "${CFLAGS} -O2 -Wall -Wextra -Wshadow",
        "CXXFLAGS": "${CXXFLAGS} -O2 -Wall -Wextra -Wshadow",
        "LDFLAGS": "${LDFLAGS}",
        },
    "linux": {
        "CFLAGS": "-fvisibility=hidden -fdata-sections -ffunction-sections",
        "CXXFLAGS": "-fvisibility=hidden -fdata-sections -ffunction-sections",
        "LDFLAGS": "-fvisibility=hidden -fdata-sections -ffunction-sections -Wl,--gc-sections -Wl,--as-needed"
        },
    })


Package(
    name="evil",
    exclusive_platforms=["win"],
    )

Package(
    name="eina",
    test_target="check",
    doc_target="doc",
    dependencies={
        "win": ["evil"],
        },
    configure_flags={
        "common": [
            "--enable-doc",
            "--enable-tests",
            #"--enable-coverage",
            "--enable-benchmark",
            ],
        },
    )

Package(
    name="eet",
    test_target="check",
    doc_target="doc",
    dependencies={
        "common": ["eina"],
        "win": ["evil"],
        },
    configure_flags={
        "common": [
            "--enable-doc",
            "--enable-tests",
            #"--enable-coverage",
            ],
        },
    )

Package(
    name="evas",
    test_target="check",
    doc_target="doc",
    dependencies={
        "common": ["eina", "eet"],
        "win": ["evil"],
        },
    configure_flags={
        "common": [
            "--enable-doc",
            "--enable-tests",
            #"--enable-coverage",
            "--enable-buffer",
            "--enable-convert-yuv",
            "--enable-async-events",
            "--enable-async-preload",
            ],
        "linux": [
            "--enable-gl-xlib",
            "--enable-software-xlib",
            ],
        },
    )

Package(
    name="ecore",
    test_target="check",
    doc_target="doc",
    dependencies={
        "common": ["eina", "evas"],
        "win": ["evil"],
        },
    configure_flags={
        "common": [
            "--enable-tests",
            "--enable-doc",
            "--enable-thread-safety",
            "--enable-ecore-evas",
            "--enable-ecore-evas-software-buffer",
            "--enable-ecore-imf",
            "--enable-ecore-imf-evas",
            "--enable-ecore-input",
            "--enable-ecore-input-evas",
            ],
        "linux": [
            "--enable-ecore-evas-software-x11",
            "--enable-ecore-evas-opengl-x11",
            ],
        },
    )

Package(
    name="embryo",
    dependencies={
        "common": ["eina"],
        "win": ["evil"],
        },
    )

Package(
    name="edje",
    test_target="check",
    doc_target="doc",
    dependencies={
        "common": ["eina", "eet", "evas", "ecore", "embryo"],
        "win": ["evil"],
        },
    configure_flags={
        "common": [
            "--enable-tests",
            "--enable-doc",
            #"--enable-coverage",
            ],
        },
    )

Package(
    name="efreet",
    test_target="check",
    doc_target="doc",
    dependencies={
        "common": ["eina", "eet", "ecore"],
        "win": ["evil"],
        },
    configure_flags={
        "common": [
            "--enable-tests",
            "--enable-doc",
            #"--enable-coverage",
            ],
        },
    )

Package(
    name="eeze",
    doc_target="doc",
    exclusive_platforms=["linux"],
    dependencies={
        "common": ["eina", "eet", "ecore"],
        },
    configure_flags={
        "common": [
            "--enable-doc",
            "--with-mount",
            "--with-umount",
            "--with-eject",
            ],
        },
    )

Package(
    name="e_dbus",
    doc_target="doc",
    exclusive_platforms=["linux"],
    dependencies={
        "common": ["eina", "ecore", "evas"],
        },
    configure_flags={
        "common": [
            "--enable-doc",
            "--enable-ebluez",
            "--enable-econnman0_7x",
            "--enable-enotify",
            "--enable-eofono",
            "--enable-eukit",
            ],
        },
    )

Package(
    name="python-evas",
    srcdir="BINDINGS/python/python-evas",
    dependencies={"common": ["evas"]},
    )
