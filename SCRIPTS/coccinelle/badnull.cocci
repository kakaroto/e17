// Do not compare pointers to NULL, use !E or E instead
//
// Copyright: (C) Gilles Muller, Julia Lawall, EMN, INRIA, DIKU.  GPLv2.
// Copyright: (C) 2010 ProFUSION Embedded Systems
// Based on: badzero.cocci
//      http://coccinelle.lip6.fr/rules/badzero.html

//
// run badzero.cocci first
//

// There are some exceptions ('good' rules), that could be handled by
// using !!E/!E, but here we let them as is.

// Returning a comparison to NULL is good.
@good1@
expression *E;
position p;
@@

  return <+...E@p...+>;

// Assignments are good, do not change them
@good4@
expression *E;
position p2;
identifier x;
@@
(
  x = (E ==@p2 NULL);
|
  x = (E !=@p2 NULL);
)

// Boolean comparison as a function parameter is good too.
@good2 disable is_zero,isnt_zero @
expression *E;
identifier f;
position p;
@@

(
  f(<+...E@p == NULL ...+>)
|
  f(<+...E@p != NULL...+>)
)

@fix disable is_zero,isnt_zero @
expression *E;
position p != {good1.p, good2.p};
position p2 != {good4.p2};
@@

(
-  E@p ==@p2 NULL
+  !E
|
-  E@p !=@p2 NULL
+  E
|
- NULL ==@p2 E@p
+  !E
|
- NULL !=@p2 E@p
+  E
)

