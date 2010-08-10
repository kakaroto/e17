// A pointer should not be compared to zero
//
// Copyright: (C) Gilles Muller, Julia Lawall, EMN, INRIA, DIKU.  GPLv2.
// Copyright: (C) 2010 ProFUSION Embedded Systems
// Based on: badzero.cocci
//      http://coccinelle.lip6.fr/rules/badzero.html
//
// This script is not made to be alone. Use badnull.cocci after this one.

@ disable is_zero,isnt_zero @
expression *E;
@@

(
  E ==
- 0
+ NULL
|
  E !=
- 0
+ NULL
|
- 0
+ NULL
  == E
|
- 0
+ NULL
  != E
)
