/*
 * Copyright 2006-2008 Ondrej Jirman <ondrej.jirman@zonio.net>
 * Copyright 2010 Mike Blumenkrantz <mike@zentific.com>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <string.h>
#include <stdio.h>

#include "azy.h"
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#elif defined __GNUC__
#define alloca __builtin_alloca
#elif defined _AIX
#define alloca __alloca
#else
#include <stddef.h>
void *alloca (size_t);
#endif


const char *
azy_stringshare_toupper(const char *str)
{
   char *tmp;

   tmp = strdupa(str);
   eina_str_toupper(&tmp);
   return eina_stringshare_add(tmp);
}

static Azy_Typedef *
azy_typedef_new(int         type,
                 const char *name,
                 const char *cname,
                 const char *ctype,
                 const char *cnull,
                 const char *dem_name,
                 const char *mar_name,
                 const char *free_func,
                 const char *fmt_str)
{
   Azy_Typedef *t = calloc(1, sizeof(Azy_Typedef));
   t->type = type;
   t->name = eina_stringshare_add(name);
   t->cname = eina_stringshare_add(cname);
   t->ctype = eina_stringshare_add(ctype);
   t->cnull = eina_stringshare_add(cnull);
   if (free_func) t->free_func = eina_stringshare_add(free_func);

   if (type == TD_BASE && (!strcmp(name, "string") || !strcmp(name, "time") || !strcmp(name, "base64")))
     {
        t->copy_func = eina_stringshare_add("eina_stringshare_ref");
     }
   else if (type == TD_ANY)
     t->copy_func = eina_stringshare_add("azy_value_ref");
   else if (type == TD_STRUCT || type == TD_ARRAY)
     {
        t->copy_func = eina_stringshare_printf("%s_copy", t->cname);
        t->eq_func = eina_stringshare_printf("%s_eq", t->cname);
        if (type == TD_STRUCT)
          t->isnull_func = eina_stringshare_printf("%s_isnull", t->cname);
        t->fmt_str = fmt_str ? eina_stringshare_add(fmt_str) : NULL;
        t->print_func = eina_stringshare_printf("%s_print", t->cname);
     }

   if ((!t->fmt_str) && (fmt_str))
     t->fmt_str = eina_stringshare_add(fmt_str);

   if (dem_name)
     t->demarch_name = eina_stringshare_add(dem_name);
   else
     t->demarch_name = eina_stringshare_printf("azy_value_to_%s", t->cname);

   if (mar_name)
     t->march_name = eina_stringshare_add(mar_name);
   else
     t->march_name = eina_stringshare_printf("%s_to_azy_value", t->cname);

   return t;
}

Azy_Model *
azy_new(void)
{
   Azy_Model *c;

   c = calloc(1, sizeof(Azy_Model));
   EINA_SAFETY_ON_NULL_RETURN_VAL(c, NULL);
   c->types = eina_list_append(c->types, azy_typedef_new(TD_BASE, "int", "int", "int", "0", "azy_value_int_get", "azy_value_int_new", NULL, "%i"));
   c->types = eina_list_append(c->types, azy_typedef_new(TD_BASE, "boolean", "boolean", "Eina_Bool", "EINA_FALSE", "azy_value_bool_get", "azy_value_bool_new", NULL, "%s"));
   c->types = eina_list_append(c->types, azy_typedef_new(TD_BASE, "string", "string", "const char *", "NULL", "azy_value_string_get", "azy_value_string_new", "eina_stringshare_del", "%s"));
   c->types = eina_list_append(c->types, azy_typedef_new(TD_BASE, "double", "double", "double", "0.0", "azy_value_double_get", "azy_value_double_new", NULL, "%.3lf"));
   c->types = eina_list_append(c->types, azy_typedef_new(TD_BASE, "time", "time", "const char *", "NULL", "azy_value_string_get", "azy_value_time_new", "eina_stringshare_del", "%s"));
   c->types = eina_list_append(c->types, azy_typedef_new(TD_BASE, "base64", "base64", "const char *", "NULL", "azy_value_base64_get", "azy_value_base64_new", "eina_stringshare_del", "%s"));
   c->types = eina_list_append(c->types, azy_typedef_new(TD_ANY, "any", "any", "Azy_Value *", "NULL", "azy_value_value_get", "azy_value_ref", "azy_value_unref", NULL));
   return c;
}

Azy_Typedef *
azy_typedef_find(Azy_Model         *azy,
                  Azy_Server_Module *module,
                  const char         *name)
{
   Eina_List *l;
   Azy_Typedef *t;

   if (module != NULL)
     EINA_LIST_FOREACH(module->types, l, t)
       {
          if (t->name == name)
            return t;
       }

   EINA_LIST_FOREACH(azy->types, l, t)
     {
        if (t->name == name)
          return t;
     }
   return NULL;
}

Azy_Typedef *
azy_typedef_new_array(Azy_Model         *azy,
                       Azy_Server_Module *module,
                       Azy_Typedef       *item)
{
   Eina_List *l;
   Azy_Typedef *t;

   if (module != NULL)
     EINA_LIST_FOREACH(module->types, l, t)
       {
          if (t->type == TD_ARRAY && t->item_type == item)
            return t;
       }

   EINA_LIST_FOREACH(azy->types, l, t)
     {
        if (t->type == TD_ARRAY && t->item_type == item)
          return t;
     }
   Azy_Typedef *a = azy_typedef_new(TD_ARRAY,
                                      NULL, eina_stringshare_printf("Array_%s", item->cname),
                                      "Eina_List*", "NULL", NULL, NULL,
                                      eina_stringshare_printf("Array_%s_free", item->cname),
                                      "%s"
                                      );
   a->item_type = item;

   if (module && item->type == TD_STRUCT)
     module->types = eina_list_append(module->types, a);
   else
     azy->types = eina_list_append(azy->types, a);

   return a;
}

Azy_Typedef *
azy_typedef_new_struct(Azy_Model                *azy,
                        Azy_Server_Module *module __UNUSED__,
                        const char                *name)
{
   Azy_Typedef *s;
   const char *n, *sep;

   n = (azy->name) ? azy->name : "";
   sep = (azy->name) ? "_" : "";

   s = azy_typedef_new(TD_STRUCT,
                       name,
                       eina_stringshare_printf("%s%s%s", n, sep, name),
                       eina_stringshare_printf("%s%s%s *", n, sep, name),
                       "NULL", NULL, NULL,
                       eina_stringshare_printf("%s%s%s_free", n, sep, name),
                       "%s"
                       );
   return s;
}

int
azy_method_compare(Azy_Method *m1,
                    Azy_Method *m2)
{
   return strcmp(m1->name, m2->name);
}

void
azy_process(Azy_Model *azy __UNUSED__)
{
}

Azy_Error_Code *
azy_error_new(Azy_Model         *azy,
               Azy_Server_Module *module,
               const char         *name,
               int                 code,
               const char         *msg)
{
   Azy_Error_Code *e = calloc(1, sizeof(Azy_Error_Code));
   e->name = eina_stringshare_add(name);
   e->msg = eina_stringshare_add(msg);
   e->code = code;

   if (module != NULL)
     {
        e->cenum = eina_stringshare_printf("%s_RPC_ERROR_%s_%s", (azy->name) ? azy_stringshare_toupper(azy->name) : "AZY", azy_stringshare_toupper(module->name), azy_stringshare_toupper(name));
        module->errors = eina_list_append(module->errors, e);
     }
   else
     {
        e->cenum = eina_stringshare_printf("%s_RPC_ERROR_%s", (azy->name) ? azy_stringshare_toupper(azy->name) : "AZY", azy_stringshare_toupper(name));
        azy->errors = eina_list_append(azy->errors, e);
     }

   return e;
}

const char *
azy_typedef_azy_name(Azy_Typedef *t)
{
   if (!t)
     return "";

   if (t->type == TD_ARRAY)
     return eina_stringshare_printf("array<%s>", azy_typedef_azy_name(t->item_type));

   return t->name;
}

