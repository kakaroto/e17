#!/usr/bin/python

# system wide modules we need
import sys
import os
import time
import argparse

# local module we need
import engine

# where to load package descriptions from
import desc

# parsing command line arguments
# see: http://docs.python.org/dev/library/argparse.html
parser = argparse.ArgumentParser(description='Generates Build Makefiles.')

parser.add_argument(
    'platform',
    choices=engine.platforms_get(),
    help='Defines platform to generate build makefile'
    )
parser.add_argument(
    '-o',
    "--output",
    default=None,
    type=str,
    help='Output to file instead of stdout')

args = parser.parse_args()
if not args.output:
    out = sys.stdout
else:
    try:
        os.unlink(args.output)
    except OSError:
        pass
    out = open(args.output, "wb")
    out.write("""\
# -*- mode: makefile -*-
# Generated for %s at %s.
# DO NOT EDIT! IT WILL BE OVERWRITTEN ON NEXT RUN!
#
# Command: %s
#
""" % (args.platform, time.asctime(), " ".join(sys.argv)))

pkgs = engine.packages_get(args.platform)
out.write("# Packages used:\n")
phony_targets = []
all_packages = []
all_targets = []
clean_targets = []
test_targets = []
doc_targets = []
distcheck_targets = []
for p in pkgs:
    out.write("#   - %s\n" % p.name)
    phony_targets.extend([p.name + "-compile",
                          p.name + "-clean",
                          p.name + "-dependencies",
                          p.name + "-install",
                          p.name + "-test",
                          p.name + "-doc",
                          p.name + "-distcheck",
                          p.name + "-all-users-compile",
                          p.name + "-direct-users-compile",
                          ])
    all_packages.append(p.name)
    all_targets.append(p.name + "-compile")
    clean_targets.append(p.name + "-clean")
    test_targets.append(p.name + "-test")
    doc_targets.append(p.name + "-doc")
    distcheck_targets.append(p.name + "-distcheck")

out.write("""
SRCROOT = $(shell pwd)
BUILDROOT = $(shell pwd)/build
INSTALLROOT = $(shell pwd)/install
MAKEOPTS = --print-directory -j2

.DEFAULT: missing-target
.PHONY: missing-target help all clean %(phony_targets)s

missing-target:
\t@echo "Missing target."
\t@exit 1

help:
\t@echo "Variables:"
\t@echo "   SRCROOT=absolute-path        where sources are located."
\t@echo "   BUILDROOT=absolute-path      where to store built files."
\t@echo "   INSTALLROOT=absolute-path    where to install built files."
\t@echo ""
\t@echo "Toplevel Targets:"
\t@echo "   clean                        remove all built files."
\t@echo "   all                          compile all packages."
\t@echo "   test                         test all packages."
\t@echo "   doc                          generate docs for all packages."
\t@echo "   distcheck                    check distribution of all packages."
\t@echo ""
\t@echo "Package Targets:"
\t@echo "   PACKAGE-compile              compile package."
\t@echo "   PACKAGE-clean                remove all built files."
\t@echo "   PACKAGE-dependencies         compile and install package dependencies."
\t@echo "   PACKAGE-install              install package to \\$$INSTALLROOT"
\t@echo "   PACKAGE-test                 test package."
\t@echo "   PACKAGE-doc                  generated documentation."
\t@echo "   PACKAGE-distcheck            check package distribution."
\t@echo "   PACKAGE-all-users-compile    compile all users of package."
\t@echo "   PACKAGE-direct-users-compile compile all direct users of package."
\t@echo ""
\t@echo "Known packages:"
\t@echo "   %(all_packages)s"
\t@echo ""

all: %(all_targets)s
clean: %(clean_targets)s
test: %(test_targets)s
doc: %(doc_targets)s
distcheck: %(distcheck_targets)s
""" % {"phony_targets": " ".join(phony_targets),
       "all_targets": " ".join(all_targets),
       "clean_targets": " ".join(clean_targets),
       "test_targets": " ".join(test_targets),
       "doc_targets": " ".join(doc_targets),
       "distcheck_targets": " ".join(distcheck_targets),
       "all_packages": " ".join(all_packages),
       })

def build_dir_get(pkgname):
    return "$(BUILDROOT)/%s" % pkgname

def stamp_dir_get(pkgname):
    return "%s/stamps" % build_dir_get(pkgname)

def stamp_name_get(pkgname, stampname):
    return "%s/%s" % (stamp_dir_get(pkgname), stampname)

def compile_dir_get(pkgname):
    return "%s/compile" % build_dir_get(pkgname)

def configure_flags_get(pkg):
    return " \\\n\t\t".join(pkg.configure_flags_get(args.platform))

def env_export_get(pkg):
    env = pkg.env_get(args.platform)
    lst = []
    for k, v in env.iteritems():
        lst.append((k, v.replace("$", "$$")))

    s = ";\\\n\texport ".join("%s=\"%s\"" % (k, v) for k, v in lst)
    if s:
        s = "export " + s + ";\\\n\t"
    return "export PKG_CONFIG_PATH=\"$(INSTALLROOT)/lib/pkgconfig\";\\\n\t" + s


for p in pkgs:
    out.write("""
########################################################################
# %s rules
""" % p.name)

    # its deps must be installed:
    stamp = stamp_name_get(p.name, "dependencies")

    deps_install = []
    deps_install_stamps = []
    for d in p.dependencies_get(args.platform):
        deps_install.append(d + "-install")
        deps_install_stamps.append(stamp_name_get(d, "install"))

    out.write("""\
%(name)s-dependencies: %(stamp)s %(deps_install)s
%(stamp)s: %(deps_install_stamps)s""" % {
            "name": p.name,
            "stamp": stamp,
            "deps_install": " ".join(deps_install),
            "deps_install_stamps": " ".join(deps_install_stamps),
            })
    out.write("\n\t@mkdir -p %s\n" % stamp_dir_get(p.name))
    out.write("\ttouch %s\n" % stamp)

    # clean it:
    out.write("""
%(name)s-clean:
\tchmod -R u+w %(compile_dir)s
\trm -fr %(compile_dir)s
\trm -fr %(stampsdir)s
""" % {"name": p.name,
       "stampsdir": stamp_dir_get(p.name),
       "compile_dir": compile_dir_get(p.name),
       })

    # autogen it:
    stamp = stamp_name_get(p.name, "autogen")
    out.write("""
%(stamp)s: $(SRCROOT)/%(srcdir)s/autogen.sh $(SRCROOT)/%(srcdir)s/configure.ac
\t@echo "Running 'autogen.sh' for %(name)s..."
\t@rm -f %(stamp)s
\tcd $(SRCROOT)/%(srcdir)s && NOCONFIGURE=1 ./autogen.sh
\t@mkdir -p %(stampsdir)s
\ttouch %(stamp)s
\t@echo "Success running 'autogen.sh' for %(name)s."
""" % {"name": p.name,
       "srcdir": p.srcdir,
       "stamp": stamp_name_get(p.name, "autogen"),
       "stampsdir": stamp_dir_get(p.name),
       })

    # configure it:
    stamp = stamp_name_get(p.name, "configure")
    out.write("""
%(stamp)s: %(autogen_stamp)s %(dependencies_stamp)s
\t@echo "Configuring %(name)s..."
\t@rm -f %(stamp)s
\t@mkdir -p %(compile_dir)s
\t%(env_export)scd %(compile_dir)s && \\
\t$(SRCROOT)/%(srcdir)s/configure --prefix=$(INSTALLROOT) %(configure_flags)s
\t@mkdir -p %(stampsdir)s
\ttouch %(stamp)s
\t@echo "Success configuring %(name)s."
""" % {"name": p.name,
       "srcdir": p.srcdir,
       "stamp": stamp_name_get(p.name, "configure"),
       "autogen_stamp": stamp_name_get(p.name, "autogen"),
       "dependencies_stamp": stamp_name_get(p.name, "dependencies"),
       "stampsdir": stamp_dir_get(p.name),
       "compile_dir": compile_dir_get(p.name),
       "configure_flags": configure_flags_get(p),
       "env_export": env_export_get(p),
       })

    # compile it (phony target)
    # don't delete stamp and just update if something linked
    # assume it compiled if libtool was to be executed:
    #
    #    make --dry-run | grep -e 'libtool '
    #
    out.write("""
%(stamp)s: %(configure_stamp)s %(name)s-compile
%(name)s-compile: %(configure_stamp)s
\t@echo "Compiling %(name)s..."
\t@rm -f %(stamp)s-updated
\t@mkdir -p %(stampsdir)s
\t@if make --dry-run $(MAKEOPTS) all -C %(compile_dir)s 2>/dev/null | grep -e 'libtool ' >/dev/null 2>/dev/null; then\\
\t\ttouch %(stamp)s-updated;\\
\tfi
\t@if test -f %(stamp)s-updated; then\\
\t\techo "make $(MAKEOPTS) all -C %(compile_dir)s";\\
\t\tmake $(MAKEOPTS) all -C %(compile_dir)s;\\
\telse\\
\t\techo "%(name)s is up to date.";\\
\tfi
\t@if test ! -f %(stamp)s -o -f %(stamp)s-updated; then\\
\t\ttouch %(stamp)s;\\
\t\techo "Compile updated, needs reinstall.";\\
\tfi
\t@echo "Success compiling %(name)s."
""" % {"name": p.name,
       "srcdir": p.srcdir,
       "stamp": stamp_name_get(p.name, "compile"),
       "configure_stamp": stamp_name_get(p.name, "configure"),
       "install_stamp": stamp_name_get(p.name, "install"),
       "stampsdir": stamp_dir_get(p.name),
       "compile_dir": compile_dir_get(p.name),
       })

    # install it:
    # don't delete stamp and just update if something was compiled
    # (stamps/compile is newer). This is being checked here instead of
    # dependencies as it may change meanwhile and GNU make is not
    # catching 'touch stamps/compile'
    out.write("""
%(stamp)s: %(compile_stamp)s %(name)s-install
%(name)s-install: %(compile_stamp)s
\t@echo "Installing %(name)s..."
\t@if test ! %(stamp)s -o %(compile_stamp)s -nt %(stamp)s; then\\
\t\trm -f %(stamp)s;\\
\t\techo "make $(MAKEOPTS) install -C %(compile_dir)s";\\
\t\tmake $(MAKEOPTS) install -C %(compile_dir)s;\\
\t\tmkdir -p %(stampsdir)s;\\
\t\ttouch %(stamp)s;\\
\telse\\
\t\techo "%(name)s did not change, not need to reinstall.";\\
\tfi
\t@echo "Success installing %(name)s."
""" % {"name": p.name,
       "srcdir": p.srcdir,
       "stamp": stamp_name_get(p.name, "install"),
       "compile_stamp": stamp_name_get(p.name, "compile"),
       "stampsdir": stamp_dir_get(p.name),
       "compile_dir": compile_dir_get(p.name),
       })

    if not p.test_target:
        out.write("""
%(stamp)s: %(compile_stamp)s %(name)s-test
%(name)s-test: %(compile_stamp)s
\t@mkdir -p %(stampsdir)s
\ttouch %(stamp)s
\t@echo 'No test target defined for %(name)s'
""" % {"name": p.name,
       "stamp": stamp_name_get(p.name, "test"),
       "compile_stamp": stamp_name_get(p.name, "compile"),
       "stampsdir": stamp_dir_get(p.name),
       })
    else:
        # test it:
        out.write("""
%(stamp)s: %(compile_stamp)s %(name)s-test
%(name)s-test: %(compile_stamp)s
\t@echo "Testing (make %(target)s) %(name)s..."
\t@rm -f %(stamp)s
\tmake $(MAKEOPTS) %(target)s -C %(compile_dir)s
\t@mkdir -p %(stampsdir)s
\ttouch %(stamp)s
\t@echo "Success testing (make %(target)s) %(name)s."
""" % {"name": p.name,
       "srcdir": p.srcdir,
       "stamp": stamp_name_get(p.name, "test"),
       "compile_stamp": stamp_name_get(p.name, "compile"),
       "stampsdir": stamp_dir_get(p.name),
       "compile_dir": compile_dir_get(p.name),
       "target": p.test_target,
       })


    if not p.doc_target:
        out.write("""
%(stamp)s: %(compile_stamp)s %(name)s-doc
%(name)s-doc: %(compile_stamp)s
\t@mkdir -p %(stampsdir)s
\ttouch %(stamp)s
\t@echo 'No doc target defined for %(name)s'
""" % {"name": p.name,
       "stamp": stamp_name_get(p.name, "doc"),
       "compile_stamp": stamp_name_get(p.name, "compile"),
       "stampsdir": stamp_dir_get(p.name),
       })
    else:
        # doc it:
        out.write("""
%(stamp)s: %(compile_stamp)s %(name)s-doc
%(name)s-doc: %(compile_stamp)s
\t@echo "Generating documentation (make %(target)s) %(name)s..."
\t@rm -f %(stamp)s
\tmake $(MAKEOPTS) %(target)s -C %(compile_dir)s
\t@mkdir -p %(stampsdir)s
\ttouch %(stamp)s
\t@echo "Success generating documentation (make %(target)s) %(name)s."
""" % {"name": p.name,
       "srcdir": p.srcdir,
       "stamp": stamp_name_get(p.name, "doc"),
       "compile_stamp": stamp_name_get(p.name, "compile"),
       "stampsdir": stamp_dir_get(p.name),
       "compile_dir": compile_dir_get(p.name),
       "target": p.doc_target,
       })

    # distcheck it:
    out.write("""
%(stamp)s: %(compile_stamp)s %(name)s-distcheck
%(name)s-distcheck: %(compile_stamp)s
\t@echo "Checking distribution %(name)s..."
\t@rm -f %(stamp)s
\t%(env_export)smake $(MAKEOPTS) distcheck -C %(compile_dir)s
\t@mkdir -p %(stampsdir)s
\ttouch %(stamp)s
\t@echo "Success checking distribution %(name)s."
""" % {"name": p.name,
       "srcdir": p.srcdir,
       "stamp": stamp_name_get(p.name, "distcheck"),
       "compile_stamp": stamp_name_get(p.name, "compile"),
       "stampsdir": stamp_dir_get(p.name),
       "compile_dir": compile_dir_get(p.name),
       "env_export": env_export_get(p),
       })

    # rule to recompile every direct user of this package
    out.write("\n%s-direct-users-compile:" % p.name)
    for u in p.users_get(args.platform):
        out.write(" %s" % u)
    out.write("\n")

    # rule to recompile every user (direct and indirect) of this package
    out.write("\n%s-all-users-compile:" % p.name)
    for u in p.all_users_get(args.platform):
        out.write(" %s" % u)
    out.write("\n")
