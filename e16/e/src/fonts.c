/*
 * Copyright (C) 2004-2006 Kim Woelders
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
#include "e16-ecore_list.h"

typedef struct
{
   char               *name;
   char               *font;
} FontAlias;

static Ecore_List  *font_list = NULL;

static void
_FontAliasDestroy(void *data)
{
   FontAlias          *fa = data;

   if (!fa)
      return;
   _EFREE(fa->name);
   _EFREE(fa->font);
   Efree(fa);
}

static FontAlias   *
FontAliasCreate(const char *name, const char *font)
{
   FontAlias          *fa;

   fa = Emalloc(sizeof(FontAlias));
   if (!fa)
      return NULL;

   fa->name = Estrdup(name);
   fa->font = Estrdup(font);

   if (!font_list)
     {
	font_list = ecore_list_new();
	ecore_list_set_free_cb(font_list, _FontAliasDestroy);
     }
   ecore_list_prepend(font_list, fa);

   return fa;
}

static int
_FontMatchName(const void *data, const void *match)
{
   return strcmp(((const FontAlias *)data)->name, match);
}

const char         *
FontLookup(const char *name)
{
   FontAlias          *fa;

   fa = ecore_list_find(font_list, _FontMatchName, name);

   return (fa) ? fa->font : NULL;
}

/*
 * Configuration load
 */
#include "conf.h"

int
FontConfigLoad(FILE * fs)
{
   int                 err = 0;
   FontAlias          *fa;
   char                s[FILEPATH_LEN_MAX];
   char                s1[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   int                 i1, ret;

   while (GetLine(s, sizeof(s), fs))
     {
	i1 = -1;
	ret = sscanf(s, "%d", &i1);
	if (ret == 1)
	  {
	     switch (i1)
	       {
	       case CONFIG_VERSION:
	       case CONFIG_OPEN:
		  break;
	       case CONFIG_CLOSE:
		  goto done;
	       }
	  }
	else
	  {
	     s1[0] = s2[0] = '\0';
	     ret = sscanf(s, "%4000s %4000s", s1, s2);
	     if (ret != 2)
	       {
		  Eprintf("Ignoring line: %s\n", s);
		  break;
	       }
	     fa = FontAliasCreate(s1, s2);
	  }
     }

 done:
   return err;
}

void
FontConfigUnload(void)
{
   ecore_list_destroy(font_list);
   font_list = NULL;
}
