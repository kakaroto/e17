Index: PROTO/autotools/efl_compiler_flag.m4
===================================================================
--- PROTO/autotools/efl_compiler_flag.m4	(revision 0)
+++ PROTO/autotools/efl_compiler_flag.m4	(revision 0)
@@ -0,0 +1,24 @@
+dnl Checks if a given compiler switch is supported.
+dnl If so, this macro adds the flag to the CFLAGS
+
+AC_DEFUN([EFL_COMPILER_FLAG],
+[
+
+CFLAGS_save="${CFLAGS}"
+CFLAGS="${CFLAGS} $1"
+  
+AC_LANG_PUSH([C])
+AC_MSG_CHECKING([whether the compiler supports $1])
+
+AC_COMPILE_IFELSE(
+   [AC_LANG_PROGRAM([[]])],
+   [have_flag="yes"],
+   [have_flag="no"])
+AC_MSG_RESULT([${have_flag}])
+
+if test "x${have_flag}" = "xno" ; then
+   CFLAGS="${CFLAGS_save}"
+fi
+AC_LANG_POP([C])
+
+])
