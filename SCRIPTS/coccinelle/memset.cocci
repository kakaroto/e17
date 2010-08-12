// Fix wrong memset usage
//
// Copyright: (C) 2010 ProFUSION Embedded Systems
// Author: Lucas De Marchi <lucas.demarchi@profusion.mobi>

@r1@
type T;
T *x;
expression E;
@@
 memset(x, E, <+...sizeof(
-                          x
+                         *x
                            )...+>)


@r2@
type T, T2;
T *x;
expression E;
@@
(
  memset(x, E, sizeof(T))
|
  memset(x, E, <+...sizeof(
-                          T*
+	                   T
                             )...+>)
// No easy way to know if it was a type inherited from another one,
// in which case the memset might be right. So, disable it for now.
//|
//  memset(x, E, <+...sizeof(
//-                          T2
//+	                   T
//                             )...+>)
)
