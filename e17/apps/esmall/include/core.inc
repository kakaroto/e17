/* Core functions
 *
 * (c) Compyright 1998-1999, ITB CompuPhase
 * This file is provided as is (no warranties).
 */
#if defined _core_included
  #endinput
#endif
#define _core_included

native heapspace();

native funcidx(name[]);

native numargs();
native getarg(arg, index=0);
native setarg(arg, index=0, value);

native strlen(string[]);
native strpack(dest[], source[]);
native strunpack(dest[], source[]);

native tolower(c);
native toupper(c);

native getproperty(id=0, name[]="", value=cellmin, string[]="");
native setproperty(id=0, name[]="", value=cellmin, string[]="");
native deleteproperty(id=0, name[]="", value=cellmin);
native existproperty(id=0, name[]="", value=cellmin);

