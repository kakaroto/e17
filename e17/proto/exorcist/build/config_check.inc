header_check() {
    rm -f conftest*
    cat > conftest.c << EOF
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <$1>
int main () { return 0; }
EOF
    $CC $CPPFLAGS $CFLAGS conftest.c -o conftest 2>$DEVNULL
    TMP="$?"
    rm -f conftest.c conftest
    return $TMP
}

toolkit_check() {
    rm -f conftest*
    cat > conftest.c << EOF
#include <$2>
int main () { $3(NULL, NULL); return 0; }
EOF
    $CC $CPPFLAGS $CFLAGS `$1 --cflags --libs` conftest.c -o conftest 2>$DEVNULL
    TMP="$?"
    rm -f conftest.c conftest
    return $TMP
}

epdf_check() {
    rm -f conftest*
    cat > conftest.c << EOF
#include <Epdf.h>
EOF
    if test "x$2" = "xyes" ; then
        cat >> conftest.c << EOF
#include <Ewl.h>
#include "ewl_pdf.h"
EOF
    fi
    if test "x$4" = "xyes" ; then
        cat >> conftest.c << EOF
#include <Etk.h>
#include "etk_pdf.h"
EOF
    fi
    cat >> conftest.c << EOF
int main () {
EOF
    if test "x$2" = "xyes" ; then
        cat >> conftest.c << EOF
  Ewl_Widget *ewl_pdf;
EOF
    fi
    if test "x$4" = "xyes" ; then
        cat >> conftest.c << EOF
  Etk_Widget *etk_pdf;
EOF
    fi
    cat >> conftest.c << EOF
  epdf_document_new (NULL);
EOF
    if test "x$2" = "xyes" ; then
        cat >> conftest.c << EOF
  ewl_pdf = ewl_pdf_new ();
EOF
    fi
    if test "x$4" = "xyes" ; then
        cat >> conftest.c << EOF
  etk_pdf = etk_pdf_new ();
EOF
    fi
    cat >> conftest.c << EOF
  return 0;
}
EOF
    EPDF_FLAGS=`$1 --cflags --libs`
    EWL_FLAGS=""
    if test "x$2" = "xyes" ; then
        EWL_FLAGS=`$3 --cflags --libs`
    fi
    ETK_FLAGS=""
    if test "x$4" = "xyes" ; then
        ETK_FLAGS=`$5 --cflags --libs`
    fi
    $CC $CPPFLAGS $CFLAGS $EPDF_FLAGS $EWL_FLAGS $ETK_FLAGS conftest.c -o conftest 2>$DEVNULL
    TMP="$?"
    rm -f conftest.c conftest
    return $TMP
}

library_check() {
    rm -f conftest*
    cat > conftest.c << EOF
#include <$1>
int main () { return 0; }
EOF
    $CC $CPPFLAGS $CFLAGS $LDFLAGS $2 conftest.c -o conftest 2>$DEVNULL
    TMP="$?"
    rm -f conftest.c conftest
    return $TMP
}

function_check() {
    rm -f conftest*
    cat > conftest.c << EOF
#include <$1>
int main () {
  $2 ($3);
  return 0;
}
EOF
    $CC $CPPFLAGS $CFLAGS $LDFLAGS $4 conftest.c -o conftest 2>$DEVNULL
    TMP="$?"
    rm -f conftest.c conftest
    return $TMP
}

std_header_check() {
    rm -f conftest*
    cat > conftest.c << EOF
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <alloca.h>
#include <setjmp.h>
#include <locale.h>
#ifndef LCCDIR
#define LCCDIR "/usr/local/lib/lcc/"
#endif

int main () { return 0; }
EOF
    $CC $CPPFLAGS $CFLAGS conftest.c -o conftest 2>$DEVNULL
    TMP="$?"
    rm -f conftest.c conftest
    return $TMP
    echo -n "Checking for standard headers... "
    have_std_header="no"
    header_check $1 && have_std_header="yes"
    if test "$have_std_header" = "no" ; then
        error_header_msg $1
    fi
    echo "yes"
}
