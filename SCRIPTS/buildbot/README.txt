
                               BUILDBOT

This directory provides configuration for our buildbot. It is composed
of a make-generator.py and desc.py

desc.py:

        Describes EFL inter-dependencies and desired compile options
        for each platform. It uses the types defined in engine.py


make-generator.py:

        Uses engine.py and definitions from desc.py to create
        makefiles to build EFL. Its purpose is to express in a single
        makefile how to build the whole tree for a given
        platform. Usually its output should be kept in our SVN as it
        should change just occasionally.

        The generated makefile is dependent on the following
        variables (see make help -- paths must be ABSOLUTE!:

         * SRCROOT=absolute-path        where sources are located.
         * BUILDROOT=absolute-path      where to store built files.
         * INSTALLROOT=absolute-path    where to install built files.

        For each package, multiple rules are generated, usually the
        buildbot will call them as different stages to highlight
        errors properly:

          * make PACKAGE-dependencies (required)
          * make PACKAGE-compile (required)
          * make PACKAGE-test (optional)
          * make PACKAGE-doc (optional)
          * make PACKAGE-install (optional)
          * make PACKAGE-all-users-compile (optional)

        A "quick run" mode would do:

          * make PACKAGE-dependencies (required)
          * make PACKAGE-compile (required)
          * make PACKAGE-test (optional)
          * make PACKAGE-direct-users-compile (optional)

