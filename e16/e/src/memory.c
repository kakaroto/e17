/*
 * Copyright (C) 2000-2003 Carsten Haitzler, Geoff Harrison and various contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "E.h"

/* uncomment DBUG_MEM to get rudamentary pointer checking                    */
/* uncomment MEM_OUT to get full debug output. to make this work you have to */
/* put a file in the directory (cwd) you run E from calle e.sym              */
/* to generate the e.sym file do:                                            */
	 /*                                                                */
	 /* nm enlightenment | grep " T " > e.sym                          */
	 /* nm enlightenment | grep " t " > e.sym2                         */
	 /* cat e.sym >> e.sym2                                            */
	 /* sort < e.sym2 > e.sym                                          */
	 /* rm e.sym2                                                      */
	 /*                                                                */
/* do that whenever you recompile enlightenment and place the e.sym file in  */
/* the current working directory of enlightenment                            */
/*                                                                           */

/*#define DBUG_MEM 1 */
/*#define MEM_OUT 1 */

#ifdef DBUG_MEM
#define POINTERS_SIZE 50000
static unsigned int num_pointers = 0;
static void        *pointers_ptr[POINTERS_SIZE];
static unsigned int pointers_size[POINTERS_SIZE];

#ifdef MEM_OUT
static void        *pointers_stack[POINTERS_SIZE][32];
static char         pointers_file[POINTERS_SIZE][16];
static int          pointers_line[POINTERS_SIZE];
static time_t       pointers_time[POINTERS_SIZE];

#define PSTK(y, x) if (__builtin_frame_address(x)) \
                    pointers_stack[y][x] = __builtin_return_address(x);\
                else goto end;
#define PST(y)\
{\
   int _pi;\
   for (_pi = 0; _pi < 32; _pi++)\
      pointers_stack[y][_pi] = NULL;\
   PSTK(y, 0); PSTK(y, 1); PSTK(y, 2); PSTK(y, 3); PSTK(y, 4);\
   PSTK(y, 5); PSTK(y, 6); PSTK(y, 7); PSTK(y, 8); PSTK(y, 9);\
   PSTK(y, 10); PSTK(y, 11); PSTK(y, 12); PSTK(y, 13); PSTK(y, 14);\
   PSTK(y, 15); PSTK(y, 16); PSTK(y, 17); PSTK(y, 18); PSTK(y, 19);\
   PSTK(y, 20); PSTK(y, 21); PSTK(y, 22); PSTK(y, 23); PSTK(y, 24);\
   PSTK(y, 25); PSTK(y, 26); PSTK(y, 27); PSTK(y, 28); PSTK(y, 29);\
   PSTK(y, 30); PSTK(y, 31);\
end: \
}

static struct _symtab
{
   void               *val;
   char               *symbol;
}
                   *sym = NULL;

static int          sym_count = 0;

char               *
getsym(void *p)
{
   int                 i;

   for (i = 0; i < sym_count - 1; i++)
     {
	if ((p > sym[i].val) && (p < sym[i + 1].val))
	   return sym[i].symbol;
     }
   return "";
}
#endif

#endif

void
EDisplayMemUse()
{
#ifdef DBUG_MEM
   FILE               *f;
   unsigned int        i, min, max, sum;

   max = 0;
   min = 0x7ffffff;
   sum = 0;
   if (!sym)
     {
	f = fopen("e.sym", "r");
	if (f)
	  {
	     void               *p;
	     char                buf[256];

	     while (fscanf(f, "%x %*s %250s\n", &p, buf) != EOF)
	       {
		  sym_count++;
		  sym = realloc(sym, sizeof(struct _symtab) * sym_count);

		  sym[sym_count - 1].val = p;
		  sym[sym_count - 1].symbol = strdup(buf);
	       }
	     fclose(f);
	  }
     }
   f = fopen("e.mem.out", "w");
   for (i = 0; i < num_pointers; i++)
     {
	char                tm[32];
	struct tm           tim;
	struct tm          *tim2;
	time_t              t2;

	sum += pointers_size[i];
	if (pointers_size[i] < min)
	   min = pointers_size[i];
	if (pointers_size[i] > max)
	   max = pointers_size[i];
	t2 = pointers_time[i];
	tim2 = localtime(&t2);
	if (tim2)
	  {
	     memcpy(&tim, tim2, sizeof(struct tm));

	     strftime(tm, 31, "%T", &tim);
	  }
	fprintf(f,
		"%6i > %p %5i @ %s - %16s line %5i : "
		"%s %s %s %s %s %s %s %s " "%s %s %s %s %s %s %s %s "
		"%s %s %s %s %s %s %s %s " "%s %s %s %s %s %s %s %s\n", i,
		pointers_ptr[i], pointers_size[i], tm, pointers_file[i],
		pointers_line[i], getsym(pointers_stack[i][0]),
		getsym(pointers_stack[i][1]), getsym(pointers_stack[i][2]),
		getsym(pointers_stack[i][3]), getsym(pointers_stack[i][4]),
		getsym(pointers_stack[i][5]), getsym(pointers_stack[i][6]),
		getsym(pointers_stack[i][7]), getsym(pointers_stack[i][8]),
		getsym(pointers_stack[i][9]), getsym(pointers_stack[i][10]),
		getsym(pointers_stack[i][11]), getsym(pointers_stack[i][12]),
		getsym(pointers_stack[i][13]), getsym(pointers_stack[i][14]),
		getsym(pointers_stack[i][15]), getsym(pointers_stack[i][16]),
		getsym(pointers_stack[i][17]), getsym(pointers_stack[i][18]),
		getsym(pointers_stack[i][19]), getsym(pointers_stack[i][20]),
		getsym(pointers_stack[i][21]), getsym(pointers_stack[i][22]),
		getsym(pointers_stack[i][23]), getsym(pointers_stack[i][24]),
		getsym(pointers_stack[i][25]), getsym(pointers_stack[i][26]),
		getsym(pointers_stack[i][27]), getsym(pointers_stack[i][28]),
		getsym(pointers_stack[i][29]), getsym(pointers_stack[i][30]),
		getsym(pointers_stack[i][31]));
     }
   if (num_pointers > 0)
     {
	fprintf(f, "Num:%6i Sum:%8i Av:%8i Min:%8i Max%6i\n", num_pointers, sum,
		sum / num_pointers, min, max);
     }
   fclose(f);
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
	ASSIGN_ALERT(_("Cannot allocate enough memory"), _("Ignore this"),
		     _("Restart Enlightenment"), _("Quit Enlightenment"));
	Alert(_
	      ("WARNING!!!!!!\n" "\n"
	       "Allocation for %i bytes (%3.0f kB or %3.1f MB) did not succeed.\n"
	       "\n" "Either this is a bug where ridiculous amounts of memory\n"
	       "are being allocated, or your system has run out of both\n"
	       "real and virtual memory and is unable to satisfy the request.\n"
	       "\n"
	       "If you have a low memory system it is suggested to either\n"
	       "purchase more memory, increase SWAP space, or reconfigure\n"
	       "Enlightenment to use less resources by turning features off.\n"
	       "\n" "The malloc requested was at %s, line %d\n "), size,
	      (float)size / 1024, (float)size / (1024 * 1024), file, line);
	RESET_ALERT;
     }
#ifdef DBUG_MEM
   if (p)
     {
	num_pointers++;
	pointers_ptr[num_pointers - 1] = p;
	pointers_size[num_pointers - 1] = size;
#ifdef MEM_OUT
	strcpy(pointers_file[num_pointers - 1], file);
	pointers_line[num_pointers - 1] = line;
	pointers_time[num_pointers - 1] = time(NULL);
	PST(num_pointers - 1);
#endif
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
	ASSIGN_ALERT(_
		     ("Error in reallocating memory that hasn't been allocated"),
		     _("Ignore this"), _("Restart Enlightenment"),
		     _("Quit Enlightenment"));
	Alert(_
	      ("WARNING!!!!!!\n" "\n"
	       "Re-allocation for %i bytes (%3.0f kB or %3.1f MB)\n"
	       "for pointer %x is attempting to re-allocate memory for a\n"
	       "memory chunk that has not been allocated or has already been\n"
	       "freed.\n" "\n" "This is definitely a bug. Please report it.\n"
	       "\n" "The error occurred at %s, line %d.\n"), size,
	      (float)size / 1024, (float)size / (1024 * 1024), ptr, file, line);
	RESET_ALERT;
	EDBUG_RETURN(NULL);
     }
#endif
   p = realloc(ptr, size);
   if ((!p) && (size != 0))
     {
	if (disp)
	   UngrabX();
	ASSIGN_ALERT(_("Cannot allocate enough memory"), _("Ignore this"),
		     _("Restart Enlightenment"), _("Quit Enlightenment"));
	Alert(_
	      ("WARNING!!!!!!\n" "\n"
	       "Re-allocation for %i bytes (%3.0f kB or %3.1f MB) did not succeed.\n"
	       "\n" "Either this is a bug where ridiculous amounts of memory\n"
	       "are being allocated, or your system has run out of both\n"
	       "real and virtual memory and is unable to satisfy the request.\n"
	       "\n"
	       "If you have a low memory system it is suggested to either\n"
	       "purchase more memory, increase SWAP space, or reconfigure\n"
	       "Enlightenment to use less resources by turning features off.\n"
	       "\n" "The realloc requested was at %s, line %d\n "), size,
	      (float)size / 1024, (float)size / (1024 * 1024), file, line);
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
      unsigned int        i, j, k;

      bad = 1;
      for (i = 0; i < num_pointers; i++)
	{
	   if (pointers_ptr[i] == ptr)
	     {
		for (j = i; j < num_pointers - 1; j++)
		  {
		     pointers_ptr[j] = pointers_ptr[j + 1];
		     pointers_size[j] = pointers_size[j + 1];
#ifdef MEM_OUT
		     for (k = 0; k < 32; k++)
			pointers_stack[j][k] = pointers_stack[j + 1][k];
		     strcpy(pointers_file[j], pointers_file[j + 1]);
		     pointers_line[j] = pointers_line[j + 1];
		     pointers_time[j] = pointers_time[j + 1];
#endif
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
	ASSIGN_ALERT(_("Error in freeing memory that hasn't been allocated"),
		     _("Ignore this"), _("Restart Enlightenment"),
		     _("Quit Enlightenment"));
	Alert(_
	      ("WARNING!!!!!!\n" "\n"
	       "freeing for pointer %x is attempting to free memory for a\n"
	       "memory chunk that has not been allocated, or has already been\n"
	       "freed.\n" "\n" "This is definitely a bug. Please report it.\n"
	       "\n" "The error occurred at %s, line %d.\n"), ptr, file, line);
	RESET_ALERT;
	EDBUG_RETURN_;
     }
#endif
   if (!ptr)
     {
	if (disp)
	   UngrabX();
	ASSIGN_ALERT(_("Error in attempting to free NULL pointer"),
		     _("Ignore this (safe)"), _("Restart Enlightenment"),
		     _("Quit Enlightenment"));
	Alert(_
	      ("WARNING!!!!!!\n" "\n"
	       "Enlightenment attempted to free a NULL pointer.\n" "\n"
	       "This is definitely a bug. Please report it.\n"
	       "It is safe to ignore this error and continue running Enlightenment.\n"
	       "\n" "The pointer value was %x.\n"
	       "The error occurred at %s, line %d.\n"), ptr, file, line);
	RESET_ALERT;
	EDBUG_RETURN_;
     }
   free(ptr);
   EDBUG_RETURN_;
}
#endif

#if !(defined(USE_STRDUP) && defined(HAVE_STRDUP))
char               *
duplicate(const char *s)
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
#endif
