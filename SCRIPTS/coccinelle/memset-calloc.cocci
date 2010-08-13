// Prefer the use of calloc over malloc + memset
//
// Copyright: (C) 2010 ProFUSION Embedded Systems
// Author: Lucas De Marchi <lucas.demarchi@profusion.mobi>
//
// Cover all cases in which we can be 100% sure this conversion is right. There
// would be other possible places, but they'd need manual inspection.
//
// r1 uses a hack to correctly get the arguments of calloc, separating
// nmemb from size using only the argument of malloc. In cases we were
// previously initializing only part of the allocated memory (by using a
// different size in memset) we are now initializing the whole struct.
//

@r1@
expression *x;
expression E1, E2, E3;
statement S1, S2;
constant c1, c2, c3;
type T2;
position p;
@@

(
- x = malloc(sizeof(E2) * c1 * c2 *c3);
+ x = calloc(c1 * c2 * c3, sizeof(E2));
|
- x = malloc(sizeof(T2) * c1 * c2 *c3);
+ x = calloc(c1 * c2 * c3, sizeof(T2));

|
- x@p = malloc(sizeof(E2) * c1 * c2);
+ x = calloc(c1 * c2, sizeof(E2));
|
- x@p = malloc(sizeof(T2) * c1 * c2);
+ x = calloc(c1 * c2, sizeof(T2));

|
- x = malloc(sizeof(E2) * c1);
+ x = calloc(c1, sizeof(E2));
|
- x = malloc(sizeof(T2) * c1);
+ x = calloc(c1, sizeof(T2));

|
- x = malloc(E1 * sizeof(E2));
+ x = calloc(E1, sizeof(E2));
|
- x = malloc(E1 * sizeof(T2));
+ x = calloc(E1, sizeof(T2));

|
- x = malloc(E1 * c1);
+ x = calloc(E1, c1);

|
- x = malloc(sizeof(E1));
+ x = calloc(1, sizeof(E1));

|
- x = malloc(E1);
+ x = calloc(1, E1);
)
(
  ... when != x
- memset(x, 0, E3);
|
  ... when != x
  if (!x) S1 else S2
- memset(x, 0, E3);
|
  ... when != x
-  if (x) {
-    memset(x, 0, E3);
-  }
-  else
-    S2
+
+  if (!x)
+    S2
|
  ... when != x
-  if (x) {
-    memset(x, 0, E3);
-  }
|
  ... when != x
  if (x) {
    ... when != x
-   memset(x, 0, E3);
    ...
  } else S2
)


@script:python@
p << r1.p;
@@
print "%s:%s" % (p[0].line, p[0].column)
