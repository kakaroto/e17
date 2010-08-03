// Warn user about wrong usage of realloc.
// From realloc(3):
//	 realloc()  returns a pointer to the newly allocated memory, which is
//	 suitably aligned for any kind of variable and may be different  from
//	 ptr,  or NULL  if  the  request  fails.
//       ...
//       If realloc() fails the original block is left untouched; it is not
//       freed or moved.
// Thus, it's wrong to do "ptr = realloc(ptr, size)", since you are going to
// loose the reference to allocated memory in the case realloc fails..
//
// There are different contexts and with several fixes, difficult to get
// automated. So, this script just unveil them.

@@
identifier var;
@@
*var = realloc(var, ...)
