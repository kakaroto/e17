// Do not compare pointers to NULL, use !E or E instead
//
// Copyright: (C) Gilles Muller, Julia Lawall, EMN, INRIA, DIKU.  GPLv2.
// Copyright: (C) 2010 ProFUSION Embedded Systems
// Based on: badzero.cocci
//      http://coccinelle.lip6.fr/rules/badzero.html

//
// run badzero.cocci first
//

@fix disable is_null,isnt_null1 @
expression *E, *x;
identifier x2;
identifier fnassert ~= "\(EXALT_ASSERT.*\|ASSERT.*\|assert\|fail_if\|fail_unless\|g_return_if_fail\|g_return_val_if_fail\|REQUIRE_RVAL\|REQUIRE\|TEST_FAIL_IF\)";
identifier fn;
type T;
@@

(
  return
	<+...
-               E != NULL
+               !!E
        ...+>;
|
-  x = (E != NULL);
+  x = !!E;
|
-  T x2 = (E != NULL);
+  T x2 = !!E;
|
   fnassert(...)
|
-  E == NULL
+  !E
|
 fn(<+...
-       E != NULL
+       !!E
    ...+>, ...)
|
 fn(<+...
-       E != NULL
+       !!E
    ...+>)
|
 fn(..., <+...
-       E != NULL
+       !!E
    ...+>)
|
-  E != NULL
+  E
)

