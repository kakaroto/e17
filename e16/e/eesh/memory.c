/*
 Copyright (C) 2000-2004 Carsten Haitzler, Geoff Harrison and various contributors

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to
 deal in the Software without restriction, including without limitation the
 rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies of the Software, its documentation and marketing & publicity
 materials, and acknowledgment shall be given in the documentation, materials
 and software packages that this Software was used.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "E.h"

/*#define DBUG_MEM 1 */

#ifdef DBUG_MEM
#define POINTERS_SIZE 10240
static unsigned int num_pointers = 0;
static void        *pointers_ptr[POINTERS_SIZE];
static unsigned int pointers_size[POINTERS_SIZE];

#endif

void
EDisplayMemUse()
{
#ifdef DBUG_MEM
   int                 i, min, max, sum;

   max = 0;
   min = 0x7ffffff;
   sum = 0;
   for (i = 0; i < num_pointers; i++)
     {
	sum += pointers_size[i];
	if (pointers_size[i] < min)
	   min = pointers_size[i];
	if (pointers_size[i] > max)
	   max = pointers_size[i];
     }
   if (num_pointers > 0)
     {
	fprintf(stderr, "Num:%6i Sum:%8i Av:%8i Min:%8i Max%6i\n",
		num_pointers, sum, sum / num_pointers, min, max);
     }
#endif
}

void               *
__Emalloc(int size)
{
   void               *p;

   EDBUG(9, "Emalloc");
   p = malloc(size);
   if (!p)
      Alert("Warning! malloc for %i bytes failed \n ", size);
#ifdef DBUG_MEM
   if (p)
     {
	num_pointers++;
	pointers_ptr[num_pointers - 1] = p;
	pointers_size[num_pointers - 1] = size;
     }
#endif
   EDBUG_RETURN(p);
}

void               *
__Erealloc(void *ptr, int size)
{
   void               *p;

#ifdef DBUG_MEM
   char                bad = 0;

#endif

   EDBUG(9, "Erealloc");
   p = realloc(ptr, size);
   if (!p)
      Alert("Warning! realloc for %i bytes failed\n", size);
#ifdef DBUG_MEM
   if (p)
     {
	int                 i;

	bad = 1;
	for (i = 0; i < num_pointers; i++)
	  {
	     if (pointers_ptr[i] == ptr)
	       {
		  pointers_size[i] = size;
		  pointers_ptr[i] = p;
		  bad = 0;
		  i = num_pointers;
	       }
	  }
     }
   if (bad)
      Alert("WARNING!\n Attempt to free memory that hasn't been allocated.\n");
#endif
   EDBUG_RETURN(p);
}

void
__Efree(void *ptr)
{
#ifdef DBUG_MEM
   char                bad = 0;

#endif
   EDBUG(9, "Efree");
   free(ptr);
#ifdef DBUG_MEM
   {
      int                 i, j;

      bad = 1;
      for (i = 0; i < num_pointers; i++)
	{
	   if (pointers_ptr[i] == ptr)
	     {
		for (j = i; j < num_pointers - 1; j++)
		  {
		     pointers_ptr[j] = pointers_ptr[j + 1];
		  }
		bad = 0;
		i = num_pointers;
		num_pointers--;
		break;
	     }
	}
   }
#endif
#ifdef DBUG_MEM
   if (bad)
      Alert("WARNING!\n Attempt to free memory that hasn't been allocated.\n");
#endif
   EDBUG_RETURN_;
}

char               *
duplicate(char *s)
{
   char               *ss;
   int                 sz;

   EDBUG(9, "duplicate");
   if (!s)
      EDBUG_RETURN(NULL);
   sz = strlen(s);
   ss = Emalloc(sz + 1);
   strncpy(ss, s, sz + 1);
   EDBUG_RETURN(ss);
}
