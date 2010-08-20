// Do not compare pointers to NULL, use !E or E instead
//
// Copyright: (C) Gilles Muller, Julia Lawall, EMN, INRIA, DIKU.  GPLv2.
// Copyright: (C) 2010 ProFUSION Embedded Systems
// Based on: badzero.cocci
//      http://coccinelle.lip6.fr/rules/badzero.html

// This version of badnull plugin does not rely on expression being a pointer.
// Run badnull.cocci and watch for false positives here
//
// run badzero.cocci first
// run badnull.cocci first
//

@fix disable is_null,isnt_null1 @
expression E, x;
identifier x2;
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

