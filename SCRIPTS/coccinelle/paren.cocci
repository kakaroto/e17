// Use parenthesis whenever you use two expressions
//
// Copyright: (C) 2010 Julia Lawall


// ----------------------------------------------------------------------
// Part 1: right arguments of &&
@two disable paren@
position p;
expression E1,E2;
@@

E1 &&@p (E2)

@r disable paren@
position p1 != two.p;
position p;
expression E1,E2;
@@

E1 &&@p1 E2@p

@@
position r.p;
expression E;
@@

+(
 E@p
+)

// ----------------------------------------------------------------------
// Part 2: left arguments of &&

@three@
position p;
expression E1,E2,E3;
@@

E1 && E2 &&@p E3

@s disable paren@
position p1!=three.p;
position p;
expression E1,E2;
@@

(
 (E1) && E2
|
E1@p &&@p1 E2
)

@a@
position s.p;
expression E;
@@

+(
 E@p
+)
