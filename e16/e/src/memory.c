#include "E.h"

/*#define DBUG_MEM 1 */

/*#define MEM_OUT 1 */

#ifdef DBUG_MEM
#define POINTERS_SIZE 256000
static unsigned int num_pointers = 0;
static void        *pointers_ptr[POINTERS_SIZE];
static unsigned int pointers_size[POINTERS_SIZE];

#endif

void
EDisplayMemUse()
{
#ifdef DBUG_MEM
   unsigned int        i, min, max, sum;

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

#if defined(__FILE__) && defined(__LINE__)
void               *
__Emalloc(int size, const char *file, int line)
{
   void               *p;

   EDBUG(9, "Emalloc");
   if (size <= 0)
      return NULL;
   p = malloc(size);
   if (!p)
     {
	if (disp)
	   UngrabX();
	ASSIGN_ALERT("Cannot allocate enough memory",
		     "Ignore this",
		     "Restart Enlightenment",
		     "Quit Enlightenment");
	Alert("WARNING!!!!!!\n"
	      "\n"
	   "Allocation for %i bytes (%3.0f Kb or %3.1f Mb) did not succeed.\n"
	      "\n"
	      "Either this is a bug where rediculous amounts of memory\n"
	      "are being allocated, or your system has run out of both\n"
	      "real and virtual memory and is unable to satisfy the request.\n"
	      "\n"
	      "If you have a low memory system it is suggested to either\n"
	      "purchase more memory, increase SWAP space, or reconfigure\n"
	      "Enlightenment to use less resources by turning features off.\n"
	      "\n"
	      "The malloc requested was at %s, line %d\n ",
	   size, (float)size / 1024, (float)size / (1024 * 1024), file, line);
	RESET_ALERT;
     }
#ifdef DBUG_MEM
   if (p)
     {
	num_pointers++;
	pointers_ptr[num_pointers - 1] = p;
	pointers_size[num_pointers - 1] = size;
     }
#endif
#ifdef MEM_OUT
   {
      FILE               *f;

      f = fopen("e.mem.out", "a");
      if (f)
	{
#ifdef DEBUG
	   int                 i_call_level;

	   fprintf(f, "%i ", call_level);
	   for (i_call_level = 0; i_call_level < call_level; i_call_level++)
	      fprintf(f, "%s ", call_stack[i_call_level]);
#endif
	   fprintf(f, " = MALLOC %s %i : %i = %p\n", file, line, size, p);
	   fclose(f);
	}
   }
#endif
   EDBUG_RETURN(p);
}

void               *
__Erealloc(void *ptr, int size, const char *file, int line)
{
   void               *p;

#ifdef DBUG_MEM
   char                bad = 0;

#endif

   if (ptr == NULL)
     {
	if (size > 0)
	   return __Emalloc(size, file, line);
	else
	   return NULL;
     }
   /* If we get here, we know ptr != NULL, so don't test for that case -- mej */
   if (size <= 0)
     {
	__Efree(ptr, file, line);
	return NULL;
     }
   EDBUG(9, "Erealloc");
#ifdef DBUG_MEM
   if (ptr)
     {
	unsigned int        i;

	bad = 1;
	for (i = 0; i < num_pointers; i++)
	  {
	     if (pointers_ptr[i] == ptr)
	       {
		  bad = 0;
		  i = num_pointers;
	       }
	  }
     }
   if (bad)
     {
	if (disp)
	   UngrabX();
	ASSIGN_ALERT("Error in reallocating memory that hasn't been allocated",
		     "Ignore this",
		     "Restart Enlightenment",
		     "Quit Enlightenment");
	Alert("WARNING!!!!!!\n"
	      "\n"
	      "Re-allocation for %i bytes (%3.0f Kb or %f3.1 Mb)\n"
	      "for pointer %x is attempting to re-allocate memory for a\n"
	      "memory chunk that has not been allocated or has already been\n"
	      "freed.\n"
	      "\n"
	      "This is definitely a bug. Please report it.\n"
	      "\n"
	      "The error occured at %s, line %d.\n",
	      size, (float)size / 1024, (float)size / (1024 * 1024), ptr, file, line);
	RESET_ALERT;
	EDBUG_RETURN(NULL);
     }
#endif
   p = realloc(ptr, size);
   if ((!p) && (size != 0))
     {
	if (disp)
	   UngrabX();
	ASSIGN_ALERT("Cannot allocate enough memory",
		     "Ignore this",
		     "Restart Enlightenment",
		     "Quit Enlightenment");
	Alert("WARNING!!!!!!\n"
	      "\n"
	"Re-allocation for %i bytes (%3.0f Kb or %3.1f Mb) did not succeed.\n"
	      "\n"
	      "Either this is a bug where rediculous amounts of memory\n"
	      "are being allocated, or your system has run out of both\n"
	      "real and virtual memory and is unable to satisfy the request.\n"
	      "\n"
	      "If you have a low memory system it is suggested to either\n"
	      "purchase more memory, increase SWAP space, or reconfigure\n"
	      "Enlightenment to use less resources by turning features off.\n"
	      "\n"
	      "The realloc requested was at %s, line %d\n ",
	   size, (float)size / 1024, (float)size / (1024 * 1024), file, line);
	RESET_ALERT;
     }
#ifdef DBUG_MEM
   if (p)
     {
	unsigned int        i;

	bad = 1;
	for (i = 0; i < num_pointers; i++)
	  {
	     if (pointers_ptr[i] == ptr)
	       {
		  pointers_size[i] = size;
		  pointers_ptr[i] = p;
		  i = num_pointers;
	       }
	  }
     }
#endif
#ifdef MEM_OUT
   {
      FILE               *f;

      f = fopen("e.mem.out", "a");
      if (f)
	{
#ifdef DEBUG
	   int                 i_call_level;

	   fprintf(f, "%i ", call_level);
	   for (i_call_level = 0; i_call_level < call_level; i_call_level++)
	      fprintf(f, "%s ", call_stack[i_call_level]);
#endif
	   fprintf(f, " = FREE %s %i : %p\n", file, line, ptr);
#ifdef DEBUG
	   fprintf(f, "%i ", call_level);
	   for (i_call_level = 0; i_call_level < call_level; i_call_level++)
	      fprintf(f, "%s ", call_stack[i_call_level]);
#endif
	   fprintf(f, " = MALLOC %s %i : %i = %p\n", file, line, size, p);
	   fclose(f);
	}
   }
#endif
   EDBUG_RETURN(p);
}

void
__Efree(void *ptr, const char *file, int line)
{
#ifdef DBUG_MEM
   char                bad = 0;

#endif
   EDBUG(9, "Efree");
#ifdef DBUG_MEM
   {
      unsigned int        i, j;

      bad = 1;
      for (i = 0; i < num_pointers; i++)
	{
	   if (pointers_ptr[i] == ptr)
	     {
		for (j = i; j < num_pointers - 1; j++)
		  {
		     pointers_ptr[j] = pointers_ptr[j + 1];
		     pointers_size[j] = pointers_size[j + 1];
		  }
		bad = 0;
		i = num_pointers;
		num_pointers--;
		break;
	     }
	}
   }
   if (bad)
     {
	if (disp)
	   UngrabX();
	ASSIGN_ALERT("Error in freeing memory that hasn't been allocated",
		     "Ignore this",
		     "Restart Enlightenment",
		     "Quit Enlightenment");
	Alert("WARNING!!!!!!\n"
	      "\n"
	      "freeing for pointer %x is attempting to free memory for a\n"
	      "memory chunk that has not been allocated, or has already been\n"
	      "freed.\n"
	      "\n"
	      "This is definitely a bug. Please report it.\n"
	      "\n"
	      "The error occured at %s, line %d.\n",
	      ptr, file, line);
	RESET_ALERT;
	EDBUG_RETURN_;
     }
#endif
   if (!ptr)
     {
	if (disp)
	   UngrabX();
	ASSIGN_ALERT("Error in attempting to free NULL pointer",
		     "Ignore this (safe)",
		     "Restart Enlightenment",
		     "Quit Enlightenment");
	Alert("WARNING!!!!!!\n"
	      "\n"
	      "Enlightenment attempted to free a NULL pointer.\n"
	      "\n"
	      "This is definitely a bug. Please report it.\n"
	"It is safe to ignore this error and continue running Enlightenment.\n"
	      "\n"
	      "The pointert value was %x.\n"
	      "The error occured at %s, line %d.\n",
	      ptr, file, line);
	RESET_ALERT;
	EDBUG_RETURN_;
     }
   free(ptr);
#ifdef MEM_OUT
   {
      FILE               *f;

      f = fopen("e.mem.out", "a");
      if (f)
	{
#ifdef DEBUG
	   int                 i_call_level;

	   fprintf(f, "%i ", call_level);
	   for (i_call_level = 0; i_call_level < call_level; i_call_level++)
	      fprintf(f, "%s ", call_stack[i_call_level]);
#endif
	   fprintf(f, " = FREE %s %i : %p\n", file, line, ptr);
	   fclose(f);
	}
   }
#endif
   EDBUG_RETURN_;
}
#endif

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
