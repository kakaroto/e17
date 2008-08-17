/*
 * Copyright (C) 2004-2008 Kim Woelders
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
#include "econfig.h"
#include "emodule.h"
#include <ctype.h>

/*
 * Braindead flat ASCII config file implementation
 */

typedef struct {
   char               *key;
   char               *value;
} ECfgFileItem;

typedef struct {
   FILE               *fs;
   int                 nitms;
   ECfgFileItem       *pitms;
} ECfgFile;

#define E_DB_File ECfgFile

static ECfgFile    *
e16_db_open(const char *name)
{
   ECfgFile           *ecf;
   FILE               *fs;

   fs = fopen(name, "w");
   if (!fs)
      return NULL;

   ecf = ECALLOC(ECfgFile, 1);
   if (!ecf)
      goto done;

   ecf->fs = fs;

 done:
   if (!ecf)
      fclose(fs);
   return ecf;
}

static ECfgFile    *
e16_db_open_read(const char *name)
{
   ECfgFile           *ecf;
   FILE               *fs;
   char                buf[4096], key[128], *s;
   int                 i, len;

   fs = fopen(name, "r");
   if (!fs)
      return NULL;

   ecf = ECALLOC(ECfgFile, 1);
   if (!ecf)
      goto done;

   for (;;)
     {
	s = fgets(buf, sizeof(buf), fs);
	if (!s)
	   break;

	/* Strip comment and trailing whitespace */
	i = strcspn(s, "#\r\n");
	for (; i > 0; i--)
	   if (!isspace(s[i - 1]))
	      break;
	s[i] = '\0';

	len = 0;
	i = sscanf(s, "%100s = %n", key, &len);
	if (i <= 0 || len <= 0)
	   continue;		/* Ignore bad format */

	i = ecf->nitms++;
	ecf->pitms = EREALLOC(ECfgFileItem, ecf->pitms, ecf->nitms);
	ecf->pitms[i].key = Estrdup(key);
	ecf->pitms[i].value = Estrdup(s + len);
     }

 done:
   fclose(fs);
   return ecf;
}

static void
e16_db_close(ECfgFile * ecf)
{
   int                 i;

   if (ecf->pitms)
     {
	for (i = 0; i < ecf->nitms; i++)
	  {
	     Efree(ecf->pitms[i].key);
	     Efree(ecf->pitms[i].value);
	  }
	Efree(ecf->pitms);
     }
   if (ecf->fs)
      fclose(ecf->fs);
   Efree(ecf);
}

static void
e16_db_flush(void)
{
}

static const char  *
ECfgFileFindValue(ECfgFile * ecf, const char *key)
{
   int                 i;

   for (i = 0; i < ecf->nitms; i++)
      if (!strcmp(key, ecf->pitms[i].key))
	 return ecf->pitms[i].value;

   return NULL;
}

static int
e16_db_int_get(ECfgFile * ecf, const char *key, int *pint)
{
   const char         *value;

   value = ECfgFileFindValue(ecf, key);
   if (!value)
      return 0;

   if (sscanf(value, "%i", pint) < 1)
      return 0;

   return 1;
}

static int
e16_db_float_get(ECfgFile * ecf, const char *key, float *pflt)
{
   const char         *value;

   value = ECfgFileFindValue(ecf, key);
   if (!value)
      return 0;

   if (sscanf(value, "%f", pflt) < 1)
      return 0;

   return 1;
}

static char        *
e16_db_str_get(ECfgFile * ecf, const char *key)
{
   const char         *value;

   value = ECfgFileFindValue(ecf, key);
   if (!value || !value[0])
      return NULL;

   return Estrdup(value);
}

static void
e16_db_int_set(ECfgFile * ecf, const char *key, int value)
{
   fprintf(ecf->fs, "%s = %d\n", key, value);
}

static void
e16_db_hex_set(ECfgFile * ecf, const char *key, unsigned int value)
{
   fprintf(ecf->fs, "%s = %#x\n", key, value);
}

static void
e16_db_float_set(ECfgFile * ecf, const char *key, float value)
{
   fprintf(ecf->fs, "%s = %f\n", key, value);
}

static void
e16_db_str_set(ECfgFile * ecf, const char *key, const char *value)
{
   fprintf(ecf->fs, "%s = %s\n", key, (value) ? value : "");
}

/*
 * Configuration handling.
 */

static void
CfgItemLoad(E_DB_File * edf, const char *prefix, const CfgItem * ci)
{
   char                buf[1024];
   const char         *name = buf;
   int                 my_int;
   float               my_float;
   char               *s;

   if (prefix)
      Esnprintf(buf, sizeof(buf), "%s.%s", prefix, ci->name);
   else
      name = ci->name;

   if (EDebug(EDBUG_TYPE_CONFIG) > 1)
      Eprintf("CfgItemLoad %s\n", name);

   if (!ci->ptr)
      return;

   switch (ci->type)
     {
     case ITEM_TYPE_BOOL:
	if (!edf || !e16_db_int_get(edf, name, &my_int))
	   my_int = (ci->dflt) ? 1 : 0;
	*((char *)ci->ptr) = my_int;
	break;
     case ITEM_TYPE_INT:
     case ITEM_TYPE_HEX:
	if (!edf || !e16_db_int_get(edf, name, &my_int))
	   my_int = ci->dflt;
	*((int *)ci->ptr) = my_int;
	break;
     case ITEM_TYPE_FLOAT:
	if (!edf || !e16_db_float_get(edf, name, &my_float))
	   my_float = ci->dflt;
	*((float *)ci->ptr) = my_float;
	break;
     case ITEM_TYPE_STRING:
	s = (edf) ? e16_db_str_get(edf, name) : NULL;
	*((char **)ci->ptr) = s;
	break;
     }
}

static void
CfgItemSave(E_DB_File * edf, const char *prefix, const CfgItem * ci)
{
   char                buf[1024];
   const char         *name = buf;
   char               *s;

   if (prefix)
      Esnprintf(buf, sizeof(buf), "%s.%s", prefix, ci->name);
   else
      name = ci->name;

   if (EDebug(EDBUG_TYPE_CONFIG) > 1)
      Eprintf("CfgItemSave %s\n", name);

   if (!ci->ptr)
      return;

   switch (ci->type)
     {
     case ITEM_TYPE_BOOL:
	e16_db_int_set(edf, name, *((char *)ci->ptr));
	break;
     case ITEM_TYPE_INT:
	e16_db_int_set(edf, name, *((int *)ci->ptr));
	break;
     case ITEM_TYPE_HEX:
	e16_db_hex_set(edf, name, *((unsigned int *)ci->ptr));
	break;
     case ITEM_TYPE_FLOAT:
	e16_db_float_set(edf, name, *((float *)ci->ptr));
	break;
     case ITEM_TYPE_STRING:
	s = *(char **)(ci->ptr);
	e16_db_str_set(edf, name, s);
	break;
     }
}

static const char  *
ConfigurationGetFile(char *buf, int len)
{
   Esnprintf(buf, len, "%s.cfg", EGetSavePrefix());
   return buf;
}

void
ConfigurationLoad(void)
{
   int                 i, nml, j, ncl;
   const EModule     **pml, *pm;
   const CfgItem      *pcl;
   char                buf[4096];
   E_DB_File          *edf;

   if (EDebug(EDBUG_TYPE_CONFIG))
      Eprintf("ConfigurationLoad\n");

   memset(&Conf, 0, sizeof(EConf));

   edf = e16_db_open_read(ConfigurationGetFile(buf, sizeof(buf)));
   /* NB! We have to assign the defaults even if it doesn't exist */

   /* Load module configs */
   pml = ModuleListGet(&nml);
   for (i = 0; i < nml; i++)
     {
	pm = pml[i];
	ncl = pm->cfg.num;
	pcl = pm->cfg.lst;
	for (j = 0; j < ncl; j++)
	   CfgItemLoad(edf, pm->name, pcl + j);
     }
   ModuleListFree(pml);

   if (edf)
      e16_db_close(edf);
}

void
ConfigurationSave(void)
{
   int                 i, nml, j, ncl;
   const EModule     **pml, *pm;
   const CfgItem      *pcl;
   char                buf[4096];
   E_DB_File          *edf;

   if (EDebug(EDBUG_TYPE_CONFIG))
      Eprintf("ConfigurationSave\n");

   edf = e16_db_open(ConfigurationGetFile(buf, sizeof(buf)));
   if (edf == NULL)
      return;

   /* Load module configs */
   pml = ModuleListGet(&nml);
   for (i = 0; i < nml; i++)
     {
	pm = pml[i];
	ncl = pm->cfg.num;
	pcl = pm->cfg.lst;
	for (j = 0; j < ncl; j++)
	   CfgItemSave(edf, pm->name, pcl + j);
     }
   ModuleListFree(pml);

   e16_db_close(edf);
   e16_db_flush();
}

const CfgItem      *
CfgItemFind(const CfgItem * pcl, int ncl, const char *name)
{
   int                 i;

   for (i = 0; i < ncl; i++, pcl++)
      if (!strcmp(name, pcl->name))
	 return pcl;
   return NULL;
}

static void
CfgItemSetFromString(const CfgItem * ci, const char *str)
{
   int                 n, ival;
   float               fval;

   switch (ci->type)
     {
     case ITEM_TYPE_BOOL:
	n = sscanf(str, "%i", &ival);
	if (n <= 0)
	   break;
	*((char *)ci->ptr) = ival;
	break;
     case ITEM_TYPE_INT:
     case ITEM_TYPE_HEX:
	n = sscanf(str, "%i", &ival);
	if (n <= 0)
	   break;
	*((int *)ci->ptr) = ival;
	break;
     case ITEM_TYPE_FLOAT:
	n = sscanf(str, "%f", &fval);
	if (n <= 0)
	   break;
	*((float *)ci->ptr) = fval;
	break;
     case ITEM_TYPE_STRING:
	Efree(*(char **)ci->ptr);
	if (*str == '\0')
	   str = NULL;
	*((char **)ci->ptr) = Estrdup(str);
	break;
     }
}

void
CfgItemToString(const CfgItem * ci, char *buf, int len)
{
   switch (ci->type)
     {
     case ITEM_TYPE_BOOL:
	Esnprintf(buf, len, "%d", *((char *)ci->ptr));
	break;
     case ITEM_TYPE_INT:
	Esnprintf(buf, len, "%d", *((int *)ci->ptr));
	break;
     case ITEM_TYPE_HEX:
	Esnprintf(buf, len, "%#x", *((unsigned int *)ci->ptr));
	break;
     case ITEM_TYPE_FLOAT:
	Esnprintf(buf, len, "%.3f", *((float *)ci->ptr));
	break;
     case ITEM_TYPE_STRING:
	if (*((char **)ci->ptr))
	   Esnprintf(buf, len, "%s", *((char **)ci->ptr));
	else
	   buf[0] = '\0';
	break;
     }
}

int
CfgItemListNamedItemSet(const CfgItem * pcl, int ncl, const char *item,
			const char *value)
{
   const CfgItem      *ci;

   ci = CfgItemFind(pcl, ncl, item);
   if (!ci)
      return -1;

   if (ci->func)
      ci->func(ci->ptr, value);
   else
      CfgItemSetFromString(ci, value);

   return 0;
}

#if 0				/* Unused */
int
CfgItemListNamedItemToString(const CfgItem * pcl, int ncl, const char *item,
			     char *buf, int len)
{
   const CfgItem      *ci;

   ci = CfgItemFind(pcl, ncl, item);
   if (!ci)
      return -1;
   CfgItemToString(ci, buf, len);

   return 0;
}
#endif

/*
 * Set <module>.<item> <value>
 */
void
ConfigurationSet(const char *params)
{
   const char         *p;
   char                name[1024];
   char                item[1024];
   unsigned int        len;

   if (!params)
      return;

   p = strchr(params, '.');
   if (!p)
     {
	Eprintf("ConfigurationSet - missed: %s\n", params);
	return;
     }

   len = p - params;
   if (len >= sizeof(name))
      len = sizeof(name) - 1;
   memcpy(name, params, len);
   name[len] = '\0';
   p++;
   len = 0;
   sscanf(p, "%1000s %n", item, &len);
   p += len;
   ModuleConfigSet(name, item, p);

   /* Save changed configuration */
   autosave();
}

/*
 * Show <module>.<item> <value>
 */
void
ConfigurationShow(const char *params)
{
   const char         *p;
   char                name[1024];
   char                item[1024];
   unsigned int        len;

   /* No parameters - All */
   if (!params || params[0] == '\0')
     {
	ModulesConfigShow();
	return;
     }

   /* No '.' - All for module */
   p = strchr(params, '.');
   if (!p)
     {
	ModuleConfigShow(params, NULL);
	return;
     }

   /* Specific module, specific item. */
   len = p - params;
   if (len >= sizeof(name))
      len = sizeof(name) - 1;
   memcpy(name, params, len);
   name[len] = '\0';
   p++;
   sscanf(p, "%s", item);
   ModuleConfigShow(name, item);
}
