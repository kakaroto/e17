// Do not compare pointers to NULL, use !E or E instead
//
// Copyright: (C) Gilles Muller, Julia Lawall, EMN, INRIA, DIKU.  GPLv2.
// Copyright: (C) 2010 ProFUSION Embedded Systems
// Based on: badzero.cocci
//      http://coccinelle.lip6.fr/rules/badzero.html

//
// run badzero.cocci first
//

@good1@
expression *E;
position p;
@@
  return <+...E@p...+>;

// a cast to a certain type complicates a bit,
// but here we are searching for expressions such as
//
// if ((a = test_fun()) == NULL)
//
// These ones will not be converted to E/!E
@good2 disable is_zero, isnt_zero@
expression *E;
identifier f;
position p2;
position p;
@@
(
 (E@p = f(...)) ==@p2 NULL
|
 (E@p = f(...)) !=@p2 NULL
|
 NULL ==@p2 (E@p = f(...))
|
 NULL !=@p2 (E@p = f(...))
)


@script:python@
p << good2.p;
@@

print "%s:%s" % (p[0].line, p[0].column)

@good3 disable is_zero,isnt_zero @
expression *E;
identifier f;
position p;
@@

(
  f(..., E@p == NULL, ...)
|
  f(..., E@p != NULL, ...)
)

@good4 disable is_zero,isnt_zero @
expression *E;
identifier f != assert;
position p2;
@@

(
  f(NULL ==@p2 E, ...)
|
  f(..., NULL ==@p2 E, ...)
|
  f(NULL !=@p2 E, ...)
|
  f(..., NULL !=@p2 E, ...)
)


@fix disable is_zero,isnt_zero @
expression *E;
position p != {good2.p, good1.p, good3.p};
position p2 != {good4.p2, good2.p2};
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

@script:python@
p << fix.p2;
@@

print "fix %s:%s" % (p[0].line, p[0].column)

