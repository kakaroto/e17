###############################################################################
## $Id$
###############################################################################

dnl
dnl Check whether Ecore_Con was compiled with OpenSSL support
dnl
AC_DEFUN(AC_ECORE_CON_CHECK_OPENSSL, [
	LIBS_SAVE="$LIBS"
	CPPFLAGS_SAVE="$CPPFLAGS"
	success=no

	LIBS="$LIBS $ECORE_CFLAGS"
	CPPFLAGS="$CPPFLAGS $ECORE_LIBS"

	AC_MSG_CHECKING(whether Ecore_Con supports OpenSSL)

	AC_TRY_RUN([#include <Ecore_Con.h>
		int main() {
			return !HAVE_ECORE_CON_OPENSSL;
		}], success=yes)

    LIBS="$LIBS_SAVE"
	CPPFLAGS="$CPPFLAGS_SAVE"

	AC_MSG_RESULT($success)

	if test $success = yes; then
		ifelse([$1], , :, [$1])
	else
		ifelse([$2], ,
			AC_MSG_ERROR([Ecore_Con has been built without OpenSSL support.]),
			[$2])
	fi
])
