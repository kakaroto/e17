// Check if pointer is null before deref
//
// Copyright: (C) 2010 ProFUSION Embedded Systems
// Author: Lucas De Marchi <lucas.demarchi@profusion.mobi>
//
//
// Refactor expressions like
//    if (ptr->var && ptr)
// to
//    if (ptr && ptr->var)
//


@@
identifier fld, x;
expression E1, E2, E3;
@@

(
- x->fld && x
+ x && x->fld
|
- E1 && x->fld && x
+ E1 && x && x->fld
|
- x->fld && x && E2
+ x && x->fld && E2
|
- E1 && x->fld && x && E2
+ E1 && x && x->fld && E2
|
- E1 && x->fld && E3 && x && E2
+ E1 && x && E3 && x->fld && E2
|
- x->fld && E3 && x && E2
+ x && E3 && x->fld && E2
|
- E1 && x->fld && E3 && x
+ E1 && x && E3 && x->fld
)

@@
identifier fld, x;
expression E1, E2, E3;
@@

(
- x->fld || x
+ x || x->fld
|
- E1 || x->fld || x
+ E1 || x || x->fld
|
- x->fld || x || E2
+ x || x->fld || E2
|
- E1 || x->fld || x || E2
+ E1 || x || x->fld || E2
|
- E1 || x->fld || E3 || x || E2
+ E1 || x || E3 || x->fld || E2
|
- x->fld || E3 || x || E2
+ x || E3 || x->fld || E2
|
- E1 || x->fld || E3 || x
+ E1 || x || E3 || x->fld
)

// if (x && b->a || b)
// if (x || b->a || b && y)
@@
identifier fld, x;
expression E1, E2;
@@

(
- E1 && x->fld || x
+ x || E1 && x->fld
|
- E1 || x->fld || x && E2
+ E1 || x && E2 || x->fld
)

