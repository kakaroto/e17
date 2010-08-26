// Checking twice for the same expression is not safer

// Copyright: (C) 2010 ProFUSION Embedded Systems
// Author: Lucas De Marchi <lucas.demarchi@profusion.mobi>
//
// 'if (a)' is better than 'if (a && a)'
//
// It could be either a bug (author was willing to check something else) or
// just insanity. We assume it was insanity and let the programmer check the
// generated patch later.

@@
expression E, E1;
@@

(
-  E && E
+  E
|
-  E || E
+  E
|
-  E && E1 && E
+  E && E1
|
-  E && E1 && E
+  E && E1

)
