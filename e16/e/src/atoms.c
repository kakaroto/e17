
#include "E.h"

void               *
AtomGet(Window win, Atom to_get, Atom type, int *size)
{
   unsigned char      *retval;
   Atom                type_ret;
   unsigned long       bytes_after, num_ret;
   int                 format_ret;
   long                length;
   void               *data;

   EDBUG(5, "AtomGet");

   retval = NULL;
   length = 0x7fffffff;
   XGetWindowProperty(disp, win, to_get, 0,
		      length,
		      False, type,
		      &type_ret,
		      &format_ret,
		      &num_ret,
		      &bytes_after,
		      &retval);
   if ((retval) && (num_ret > 0) && (format_ret > 0))
     {
	if (format_ret == 32)
	  {
	     int                 i;

	     *size = num_ret * sizeof(unsigned long);

	     data = Emalloc(*size);
	     for (i = 0; i < (int)num_ret; i++)
		((unsigned long *)data)[i] = ((unsigned long *)retval)[i];
	  }
	else if (format_ret == 16)
	  {
	     int                 i;

	     *size = num_ret * sizeof(unsigned short);

	     data = Emalloc(*size);
	     for (i = 0; i < (int)num_ret; i++)
		((unsigned short *)data)[i] = ((unsigned short *)retval)[i];
	  }
	else
	  {
	     /* format_ret == 8 */
	     *size = num_ret;
	     data = Emalloc(num_ret);
	     if (data)
		memcpy(data, retval, num_ret);
	  }
	XFree(retval);
	EDBUG_RETURN(data);
     }
   EDBUG_RETURN(NULL);

}

void
setSimpleHint(Window win, Atom atom, long value)
{

   EDBUG(5, "setSimpleHint");

   XChangeProperty(disp, win, atom, atom, 32, PropModeReplace,
		   (unsigned char *)&value, 1);

   EDBUG_RETURN_;

}

long               *
getSimpleHint(Window win, Atom atom)
{

   long               *data = 0;
   Atom                type_ret;
   int                 fmt_ret;
   unsigned long       nitems_ret;
   unsigned long       bytes_after_ret;

   EDBUG(5, "getSimpleHint");

   if (!atom)
      EDBUG_RETURN(NULL);

   if (XGetWindowProperty(disp, win, atom, 0, 1, False, atom,
			  &type_ret, &fmt_ret, &nitems_ret, &bytes_after_ret,
			  (unsigned char **)&data) != Success || !data)
     {
	EDBUG_RETURN(NULL);
     }
   if (atom != AnyPropertyType && atom != type_ret)
     {
	XFree(data);
	EDBUG_RETURN(NULL);
     }
   EDBUG_RETURN((long *)data);

}
