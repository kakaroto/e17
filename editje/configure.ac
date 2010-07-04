AC_INIT([editje], [0.9.1], [enlightenment-devel@lists.sourceforge.net])

AM_INIT_AUTOMAKE([1.6 dist-bzip2])

AM_MAINTAINER_MODE

edje_cc=$(pkg-config --variable=prefix edje)/bin/edje_cc
AC_SUBST(edje_cc)

dnl check for python
dnl AM_PATH_PYTHON(2.2)
AM_PATH_PYTHON(2.5)

AC_OUTPUT([
Makefile
editje.spec
editje/Makefile
editje/sysconfig.py
data/Makefile
data/themes/Makefile
data/themes/default/Makefile
data/themes/default/images/Makefile
data/themes/default/elm_images/Makefile
data/templates/Makefile
data/templates/default/Makefile
test/Makefile
])
