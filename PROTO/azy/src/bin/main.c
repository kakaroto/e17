/*
 * Parts copyright 2006-2008 Ondrej Jirman <ondrej.jirman@zonio.net>
 * Copyright 2010 Mike Blumenkrantz <mike@zentific.com>
 */


#include <Eina.h>
#include <Ecore.h>
#include <Ecore_Getopt.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "azy.h"

/* codegen helpers */

#define EL(i, fmt, args ...) \
  line_fprintf(i, fmt "\n", ## args)

#define E(i, fmt, args ...) \
  line_fprintf(i, fmt, ## args)

#define NL \
  line_fprintf(0, "\n")

#define S(args ...) \
  eina_stringshare_printf(args)

#define OPEN(fmt, args ...) {                                     \
     f = fopen(current_file = S(fmt, ## args), "w");              \
     if (!f) {                                                    \
          fprintf(stderr, "Can't open output file for writing."); \
          exit(1); }                                              \
}                                                                 \
  current_line = 1

#define ORIG_LINE() \
  EL(0, "#line %d \"%s\"", current_line + 1, current_file)

#define FILE_LINE(_line, _file) \
  EL(0, "#line %d \"%s\"", _line, _file)

#define GET_NAME(func)                                             \
  if (s->stub_##func && s->stub_##func[0])                          \
    E(0, "%s%s%s_module_"#func, name, sep, s->name);                  \
  else                                                             \
    E(0, "NULL")

#define STUB(s)                                   \
  do {                                            \
       if (s)                                     \
         {                                        \
            FILE_LINE(s ## _line + 1, azy_file); \
            EL(0, "%s", s);                       \
            ORIG_LINE();                          \
         }                                        \
    } while(0)

static int current_line;
static const char *current_file = NULL;

static Azy_Model *azy;
static Eina_Bool client_headers;
static Eina_Bool client_impl;
static Eina_Bool common_headers;
static Eina_Bool common_impl;
static Eina_Bool server_impl;
static Eina_Bool server_headers;
static Eina_Bool azy_gen;
static char *out_dir = ".";
static char *azy_file;
static FILE *f;
static const char *i, *b;
static const char *sep;
static const char *name;

static const Ecore_Getopt opts = {
   "azy_parser",
   "azy_parser file.azy -o destination_directory/",
   "1.0alpha",
   "(C) 2010 Mike Blumenkrantz, previously others, see AUTHORS",
   "LGPL",
   "Parse an azy file into *.{c,h} files\n\n",
   1,
   {
      ECORE_GETOPT_STORE_STR('m', "modes", "Parser modes: all, server-impl, server-headers,\n"
                                           "\t\t\tclient-impl, client-headers,\n"
                                           "\t\t\tcommon-impl, common-headers"),
      ECORE_GETOPT_STORE_STR('o', "output", "Output directory."),
      ECORE_GETOPT_STORE_TRUE('d', "debug", "Print debugging output."),
      ECORE_GETOPT_VERSION('V', "version"),
      ECORE_GETOPT_COPYRIGHT('R', "copyright"),
      ECORE_GETOPT_LICENSE('L', "license"),
      ECORE_GETOPT_HELP('h', "help"),
      ECORE_GETOPT_SENTINEL
   }
};


void azyParserTrace(FILE *TraceFILE,
                     char *zTracePrompt);
static void
line_fprintf(unsigned int indent,
             const char  *fmt,
             ...)
{
   unsigned int x;
   va_list ap;

   for (x = 0; x < indent; x++)
     fprintf(f, "  ");

   va_start(ap, fmt);
   vfprintf(f, fmt, ap);
   va_end(ap);

   for (x = 0; x < strlen(fmt); x++)
     if (fmt[x] == '\n')
       current_line++;
}

static void
gen_type_marshalizers(Azy_Typedef *t,
                      Eina_Bool     header,
                      Eina_Bool     _static)
{
   Eina_List *l;
   Azy_Struct_Member *m;

   if (header)
     {
        if (t->mheader) return;
        if (t->type == TD_STRUCT)
          {
             EL(0, "%sAzy_Value *%s(%s _nstruct) EINA_WARN_UNUSED_RESULT;", (_static) ? "static " : "", t->march_name,
                   ((t->ctype == i) || (t->ctype == b)) ? "int32_t" : t->ctype);
             EL(0, "%sEina_Bool %s(Azy_Value *_struct, %s* _nstruct) EINA_WARN_UNUSED_RESULT;", (_static) ? "static " : "",
                t->demarch_name, t->ctype);
          }
        else
          {
             if (!strcmp(t->ctype, "Eina_Bool"))
               return;
             EL(0, "%sAzy_Value *%s(%s _narray) EINA_WARN_UNUSED_RESULT;", (_static) ? "static " : "",
                t->march_name, t->ctype);
             EL(0, "%sEina_Bool %s(Azy_Value *_array, %s* _narray) EINA_WARN_UNUSED_RESULT;", (_static) ? "static " : "",
                t->demarch_name, t->ctype);
          }
        t->mheader = EINA_TRUE;
        return;
     }

   if (t->mfunc) return;
   if (t->type == TD_STRUCT)
     {
        EL(0, "%sAzy_Value *%s(%s _nstruct)", (_static) ? "static " : "", t->march_name,
           ((t->ctype == i) || (t->ctype == b)) ? "int32_t" : t->ctype);
        EL(0, "{");
        EL(1, "Azy_Value *_struct = NULL;");

        EINA_LIST_FOREACH(t->struct_members, l, m)
          {
             EL(1, "Azy_Value *%s = NULL;", m->name);
          }

        NL;
        EL(1, "if (!_nstruct)");
        EL(2, "return NULL;");
        NL;

        EINA_LIST_FOREACH(t->struct_members, l, m)
          {
             EL(2, "%sif (!(%s = %s(_nstruct->%s)))", l->prev ? "else " : "", m->name, m->type->march_name, m->name);
             EL(3, "goto error;");
          }

        NL;
        EL(1, "_struct = azy_value_struct_new();");

        EINA_LIST_FOREACH(t->struct_members, l, m)
          {
             EL(1, "azy_value_struct_member_set(_struct, \"%s\", %s);", m->name, m->name);
          }

        EL(1, "return _struct;");
        EL(0, "error:");
        EINA_LIST_FOREACH(t->struct_members, l, m)
          EL(1, "if (%s) azy_value_unref(%s);", m->name, m->name);
        EL(1, "return NULL;");
        EL(0, "}");
        NL;

        
        EL(0, "%sEina_Bool %s(Azy_Value *_struct, %s *_nstruct)", (_static) ? "static " : "",
           t->demarch_name, t->ctype);
        EL(0, "{");
        EL(1, "%s _tmp_nstruct = NULL;", t->ctype);
        NL;
        EL(1, "EINA_SAFETY_ON_NULL_RETURN_VAL(_nstruct, EINA_FALSE);");
        EL(1, "EINA_SAFETY_ON_NULL_RETURN_VAL(_struct, EINA_FALSE);");
        NL;
        EL(1, "EINA_SAFETY_ON_TRUE_RETURN_VAL(azy_value_type_get(_struct) != AZY_VALUE_STRUCT, EINA_FALSE);");
        NL;
        EL(1, "_tmp_nstruct = %s_new();", t->cname);

        EINA_LIST_FOREACH(t->struct_members, l, m)
          {
             EL(1, "%sif (!%s(azy_value_struct_member_get(_struct, \"%s\"), &_tmp_nstruct->%s))", l->prev ? "else " : "",
                m->type->demarch_name, m->name, m->name);
             EL(2, "goto error;");
          }
          
        NL;
        EL(1, "*_nstruct = _tmp_nstruct;");
        EL(1, "return EINA_TRUE;");
        EL(0, "error:");
        EL(1, "%s(_tmp_nstruct);", t->free_func);
        EL(1, "return EINA_FALSE;");
        EL(0, "}");
        NL;
     }
   else if (t->type == TD_ARRAY)
     {
        EL(0, "%sAzy_Value *%s(%s _narray)", (_static) ? "static " : "", t->march_name, t->ctype);
        EL(0, "{");
        EL(1, "Eina_List *_item;");
        EL(1, "%s v;", ((t->item_type->ctype == i) || (t->item_type->ctype == b)) ? "int32_t*" : t->item_type->ctype);
        EL(1, "Azy_Value *_array = azy_value_array_new();");
        NL;
        EL(1, "EINA_LIST_FOREACH(_narray, _item, v)");
        EL(1, "{");

        if ((t->item_type->ctype == i) || (t->item_type->ctype == b))
          EL(2, "Azy_Value *_item_value = %s(*v);", t->item_type->march_name);
        else
          EL(2, "Azy_Value *_item_value = %s((%s)v);", t->item_type->march_name, t->item_type->ctype);

        NL;
        EL(2, "if (!_item_value)");
        EL(2, "{");
        EL(3, "azy_value_unref(_array);");
        EL(3, "return NULL;");
        EL(2, "}");
        NL;
        EL(2, "azy_value_array_push(_array, _item_value);");
        EL(1, "}");
        NL;
        EL(1, "return _array;");
        EL(0, "}");
        NL;

        EL(0, "%sEina_Bool %s(Azy_Value *_array, %s* _narray)", (_static) ? "static " : "", t->demarch_name, t->ctype);
        EL(0, "{");
        EL(1, "Eina_List *_tmp_narray = NULL, *_item;");
        EL(1, "Azy_Value *v;");
        NL;
        EL(1, "EINA_SAFETY_ON_NULL_RETURN_VAL(_narray, EINA_FALSE);");
        NL;
        EL(1, "if ((!_array) || (azy_value_type_get(_array) != AZY_VALUE_ARRAY))");
        EL(2, "return EINA_FALSE;");
        NL;
        EL(1, "EINA_LIST_FOREACH(azy_value_children_items_get(_array), _item, v)");
        EL(1, "{");
        EL(2, "%s _item_value = %s;", t->item_type->ctype, t->item_type->cnull);
        NL;
        EL(2, "if (!%s(v, &_item_value))", t->item_type->demarch_name);
        EL(2, "{");
        EL(3, "%s(_tmp_narray);", t->free_func);
        EL(3, "return EINA_FALSE;");
        EL(2, "}");
        NL;

        if ((t->item_type->ctype == i) || (t->item_type->ctype == b))
          EL(2, "_tmp_narray = eina_list_append(_tmp_narray, &_item_value);");
        else

          EL(2, "_tmp_narray = eina_list_append(_tmp_narray, _item_value);");

        EL(1, "}");
        NL;
        EL(1, "*_narray = _tmp_narray;");
        EL(1, "return EINA_TRUE;");
        EL(0, "}");
        NL;
     }
   t->mfunc = EINA_TRUE;
}

static void
gen_type_eq(Azy_Typedef *t,
            int           def)
{
   Eina_List *l;
   Azy_Struct_Member *m;

   if (def)
     {
        if (t->type == TD_STRUCT)
          EL(0, "Eina_Bool %s_eq(%s a, %s b);", t->cname, t->ctype,
             t->ctype);
        else if (t->type == TD_ARRAY)
          EL(0, "Eina_Bool %s(%s a, %s b);", t->eq_func, t->ctype, t->ctype);

        return;
     }

   if (t->type == TD_STRUCT)
     {
        EL(0, "Eina_Bool %s_eq(%s a, %s b)", t->cname, t->ctype, t->ctype);
        EL(0, "{");
        EL(1, "if (a == b)");
        EL(2, "return EINA_TRUE;");
        EL(1, "if ((!a) || (!b))");
        EL(2, "return EINA_FALSE;");

        EINA_LIST_FOREACH(t->struct_members, l, m)
          {
             if (m->type->eq_func)
               {
                  EL(1, "if (%s(a->%s, b->%s))", m->type->eq_func,
                     m->name, m->name);
               }
             else
               EL(1, "if (a->%s != b->%s)", m->name, m->name);

             EL(2, "return EINA_FALSE;");
          }

        NL;
        EL(1, "return EINA_TRUE;");
        EL(0, "}");
        NL;
     }
   else if (t->type == TD_ARRAY)
     {
        EL(0, "Eina_Bool %s(%s a, %s b)", t->eq_func, t->ctype, t->ctype);
        EL(0, "{");
        EL(1, "Eina_List *y, *z;");
        NL;
        EL(1, "if (a == b)");
        EL(2, "return EINA_TRUE;");
        EL(1, "if ((!a) || (!b))");
        EL(2, "return EINA_FALSE;");
        EL(1, "for (y = a, z = b;y && z; y = y->next, z = z->next)");
        EL(1, "{");

        if (t->item_type->eq_func)
          EL(2, "if (%s(y->data, z->data))",
             t->item_type->eq_func);
        else
          {
             if ((t->item_type->ctype == i) || (t->item_type->ctype == b))
               EL(2, "if ((intptr_t)(y->data) != (intptr_t)(z->data))");
             else
               EL(2, "if ((%s)(y->data) != (%s)(z->data))", t->item_type->ctype, t->item_type->ctype);
          }

        EL(3, "return EINA_FALSE;");
        EL(1, "}");
        NL;
        EL(1, "return EINA_TRUE;");
        EL(0, "}");
        NL;
     }
}

static void
gen_type_isnull(Azy_Typedef *t,
                 int           def)
{
   Eina_List *l;
   Azy_Struct_Member *m;

   if (def)
     {
        if (t->type == TD_STRUCT)
          EL(0, "Eina_Bool %s_isnull(%s a);", t->cname, t->ctype);
        return;
     }

   if (t->type == TD_STRUCT)
     {
        EL(0, "Eina_Bool %s_isnull(%s a)", t->cname, t->ctype);
        EL(0, "{");
        EL(1, "if (!a)");
        EL(2, "return EINA_TRUE;");

        EINA_LIST_FOREACH(t->struct_members, l, m)
          {
             if (m->type->isnull_func)
               EL(1, "if (%s(a->%s))", m->type->isnull_func, m->name);
             else
               EL(1, "if (a->%s != %s)", m->name, m->type->cnull);

             EL(2, "return EINA_FALSE;");
          }

        NL;
        EL(1, "return EINA_TRUE;");
        EL(0, "}");
        NL;
     }
}

static void
gen_type_print(Azy_Typedef *t,
               int           def)
{
   Eina_List *l;
   Azy_Struct_Member *m;

   if (def)
     {
        if (t->type == TD_STRUCT)
          EL(0, "void %s_print(const char *pre, int indent, %s a);", t->cname, t->ctype, t->ctype);
        else if (t->type == TD_ARRAY)
          EL(0, "void %s(const char *pre, int indent, Eina_List *a);", t->print_func);
        return;
     }

   if (t->type == TD_STRUCT)
     {
        EL(0, "void %s_print(const char *pre, int indent, %s a)", t->cname, t->ctype);
        EL(0, "{");
        EL(1, "int i;");
        EL(1, "if (!a)");
        EL(2, "return;");
        EL(1, "if (!pre) pre = \"\\t\";");

        EINA_LIST_FOREACH(t->struct_members, l, m)
          {
             EL(1, "for (i = 0; i < indent; i++)");
             EL(2, "printf(\"%%s\", pre);");
             if (m->type->print_func)
               {
                  EL(1, "printf(\"%s:\\n\");", m->name);
                  EL(1, "%s(pre, indent + 1, a->%s);", m->type->print_func, m->name);
                  EL(1, "printf(\"\\n\");");
               }
             else
               {
                  if (m->type->ctype == b)
                    EL(1, "printf(\"%s: %s\\n\", (a->%s) ? \"yes\" : \"no\");", m->name, m->type->fmt_str, m->name);
                  else
                    EL(1, "printf(\"%s: %s\\n\", a->%s);", m->name, m->type->fmt_str, m->name);
               }
          }
        EL(0, "}");
        NL;
     }
   else if (t->type == TD_ARRAY)
     {
        EL(0, "void %s(const char *pre, int indent, %s a)", t->print_func, t->ctype);
        EL(0, "{");
        EL(1, "Eina_List *l;");
        if ((t->item_type->ctype == i) || (t->item_type->ctype == b))
          EL(1, "int *t;");
        else
          EL(1, "%s t;", t->item_type->ctype);
        NL;
        EL(1, "if (!a)");
        EL(2, "return;");
        EL(1, "EINA_LIST_FOREACH(a, l, t)");
        if (t->item_type->print_func)
          EL(2, "%s(pre, indent + 1, t);", t->item_type->print_func);
        else
          {
               EL(1, "{");
               EL(2, "int i;");
               EL(2, "for (i = 0; i < indent; i++)");
               EL(3, "printf(\"%%s\", pre);");
               if (t->item_type->ctype == i)
                 EL(2, "printf(\"%s, \", (intptr_t)t);", t->item_type->fmt_str);
               else if (t->item_type->ctype == b)
                 EL(2, "printf(\"%s, \", ((intptr_t)t) ? \"yes\" : \"no\");", t->item_type->fmt_str);
               else
                 EL(2, "printf(\"%s, \", t);", t->item_type->fmt_str);
               EL(1, "}");
          }
        EL(0, "}");
        NL;
     }
}

static void
gen_type_copyfree(Azy_Typedef *t,
                   Eina_Bool   def,
                   Eina_Bool   _static)
{
   Eina_List *l;
   Azy_Struct_Member *m;

   if (def)
     {
        if (t->fcheader) return;
        if (t->type == TD_STRUCT)
          EL(0, "%s %s_new();", t->ctype, t->cname);

        if (t->type == TD_STRUCT || t->type == TD_ARRAY)
          {
             EL(0, "%svoid %s(%s val);", (_static) ? "static " : "", t->free_func, t->ctype);
             EL(0, "%s%s %s(%s orig);", (_static) ? "static " : "", t->ctype, t->copy_func, t->ctype);
          }
        t->fcheader = EINA_TRUE;
        return;
     }

   if (t->fcfunc) return;
   if (t->type == TD_STRUCT)
     {
        /* new */
         EL(0, "%s%s %s_new()", (_static) ? "static " : "", t->ctype, t->cname);
         EL(0, "{");
         EL(1, "return calloc(1, sizeof(%s));", t->cname);
         EL(0, "}");
         NL;

         /* free */
         EL(0, "%svoid %s(%s val)", (_static) ? "static " : "", t->free_func, t->ctype);
         EL(0, "{");
         EL(1, "if (!val)");
         EL(2, "return;");
         NL;

         EINA_LIST_FOREACH(t->struct_members, l, m)
           {
              if (m->type->free_func)
                EL(1, "%s(val->%s);", m->type->free_func, m->name);
           }

         EL(1, "free(val);");
         EL(0, "}");
         NL;

         /* copy */
         EL(0, "%s%s %s(%s orig)", (_static) ? "static " : "", t->ctype, t->copy_func, t->ctype);
         EL(0, "{");
         EL(1, "%s copy;", t->ctype);
         NL;
         EL(1, "if (!orig)");
         EL(2, "return NULL;");
         NL;
         EL(1, "copy = %s_new();", t->cname);

         EINA_LIST_FOREACH(t->struct_members, l, m)
           {
              if (m->type->copy_func)
                EL(1, "copy->%s = %s(orig->%s);", m->name,
                   m->type->copy_func,
                   m->name);
              else
                EL(1, "copy->%s = orig->%s;", m->name, m->name);
           }

         NL;
         EL(1, "return copy;");
         EL(0, "}");
         NL;
     }
   else if (t->type == TD_ARRAY)
     {
        /* free */
         EL(0, "%svoid %s(%s val)", (_static) ? "static " : "", t->free_func, t->ctype);
         EL(0, "{");
         if (t->item_type->free_func)
           EL(1, "%s t;", t->item_type->ctype);
         EL(1, "if (!val)");
         EL(2, "return;");

         if (t->item_type->free_func)
           {
              EL(1, "EINA_LIST_FREE(val, t)");
              EL(2, "%s(t);", t->item_type->free_func);
           }
         else
           EL(1, "eina_list_free(val);");
         EL(0, "}");
         NL;

         /* copy */
         EL(0, "%s%s %s(%s orig)", (_static) ? "static " : "", t->ctype, t->copy_func, t->ctype);
         EL(0, "{");
         EL(1, "%s copy = NULL;", t->ctype);
         NL;
         EL(1, "while (orig)");
         EL(1, "{");

         if (t->item_type->copy_func)
           EL(2, "copy = eina_list_prepend(copy, %s((%s)orig->data));",
              t->item_type->copy_func, t->item_type->ctype);
         else
           EL(2, "copy = eina_list_prepend(copy, orig->data);");

         EL(
           2, "orig = orig->next;");
         EL(1, "}");
         NL;
         EL(1, "return eina_list_reverse(copy);");
         EL(0, "}");
         NL;
     }
   t->fcfunc = EINA_TRUE;
}

static void
gen_type_defs(Eina_List *types)
{
   Eina_List *j, *l;
   Azy_Typedef *t;

   EINA_LIST_FOREACH(types, j, t)
     {
        Azy_Struct_Member *m;

        if (t->type == TD_STRUCT)
          {
             if (t->doc)
               EL(0, "%s", t->doc);

             EL(0, "typedef struct %s", t->cname);
             EL(0, "{");

             EINA_LIST_FOREACH(t->struct_members, l, m)
               {
                  EL(1, "%s %s; /* %s */", m->type->ctype, m->name,
                     (m->type->type == TD_ARRAY) ? m->type->cname : "");
               }

             EL(0, "} %s;", t->cname);
             NL;
          }
     }
}

static void
gen_marshalizers(Azy_Server_Module *s, Eina_Bool header)
{
   Eina_List *j;
   Azy_Typedef *t;

   if (!s)
     {
        EINA_LIST_FOREACH(azy->types, j, t)
          {
             if (t->type == TD_ANY)
               continue;

             gen_type_marshalizers(t, header, EINA_FALSE);
          }
        return;
     }
   EINA_LIST_FOREACH(s->types, j, t)
     {
        if (t->type == TD_ANY)
          continue;

        gen_type_marshalizers(t, header, EINA_TRUE);
     }
}

static void
gen_errors_header(Azy_Server_Module *s)
{
   Eina_List *j;
   Eina_List *errs = s ? s->errors : azy->errors;
   Azy_Error_Code *e;

   if (!errs)
     return;

   EINA_LIST_FOREACH(errs, j, e)
     {
        EL(0, "extern Eina_Error %s;", e->cenum, e->code);
     }
   NL;

   if (s)
     EL(0, "void azy_err_init_%s%s%s(void);", name, sep, s->name);
   else
     EL(0, "void azy_err_init(void);");
}

static void
gen_errors_impl(Azy_Server_Module *s)
{
   Eina_List *j;
   Eina_List *errs = s ? s->errors : azy->errors;
   Azy_Error_Code *e;

   if (!errs)
     return;


   if (!s)
     {
        EINA_LIST_FOREACH(azy->modules, j, s)
          if (s->errors)
            EL(0, "extern void azy_err_init_%s%s%s(void);", name, sep, s->name);
     }

   EINA_LIST_FOREACH(errs, j, e)
     {
        EL(0, "Eina_Error %s;", e->cenum, e->code);
        EL(0, "const char %s_str[] = \"%s\";", e->cenum, e->msg);
        NL;
     }

   if (s)
     EL(0, "void azy_err_init_%s%s%s(void)", name, sep, s->name);
   else
     EL(0, "void azy_err_init(void)");
   EL(0, "{");

   if (!s)
     {
        EINA_LIST_FOREACH(azy->modules, j, s)
          if (s->errors)
            EL(1, "azy_err_init_%s%s%s();", name, sep, s->name);
     }
   EINA_LIST_FOREACH(errs, j, e)
     EL(1, "%s = eina_error_msg_static_register(%s_str);", e->cenum, e->cenum);

   EL(0, "}");
   NL;
}

static void
gen_common_headers(void)
{
   Eina_List *j;
   Azy_Typedef *t;
   
   OPEN("%s/%s%sCommon.h", out_dir, name, sep);

   EL(0, "#ifndef %s_Common_H", (azy->name) ? azy->name : "AZY");
   EL(0, "#define %s_Common_H", (azy->name) ? azy->name : "AZY");
   NL;

   EL(0, "#include <Azy.h>");
   NL;
   gen_errors_header(NULL);
   gen_type_defs(azy->types);

   EINA_LIST_FOREACH(azy->types, j, t)
     {
        gen_type_copyfree(t, EINA_TRUE, EINA_FALSE);
        gen_type_eq(t, EINA_TRUE);
        gen_type_print(t, EINA_TRUE);
        gen_type_isnull(t, EINA_TRUE);
     }

   NL;
   gen_marshalizers(NULL, EINA_TRUE);

   EL(0, "#endif");
   fclose(f);
}

static void
gen_common_impl(Azy_Server_Module *s)
{
   if (!s)
     {
        Eina_List *j;
        Azy_Typedef *t;

        OPEN("%s/%s%sCommon.c", out_dir, name, sep);

        EL(0, "#include \"%s%sCommon.h\"", name, sep);
        EL(0, "#include <string.h>");
        NL;

        EINA_LIST_FOREACH(azy->types, j, t)
          {
             gen_type_copyfree(t, EINA_FALSE, EINA_FALSE);
             gen_type_eq(t, EINA_FALSE);
             gen_type_print(t, EINA_FALSE);
             gen_type_isnull(t, EINA_FALSE);
          }
        
        gen_errors_impl(NULL);
        gen_marshalizers(NULL, EINA_FALSE);
     }
   else
     {
        OPEN("%s/%s%s%s.c", out_dir, name, sep, s->name);
        EL(0, "#include \"%s%sCommon.h\"", name, sep);
        EL(0, "#include <string.h>");
        NL;
        gen_errors_impl(s);
     }
   fclose(f);
}

static void
gen_server_headers(Azy_Server_Module *s)
{
   Eina_List *j, *k;
   Azy_Method_Param *p;
   Azy_Method *method;
   
   OPEN("%s/%s%s%s.azy_server_stubs.h", out_dir, name, sep, s->name);

   EL(0, "#ifndef %s_%s_STUBS_H", (azy->name) ? azy->name : "AZY", s->name);
   EL(0, "#define %s_%s_STUBS_H", (azy->name) ? azy->name : "AZY", s->name);
   NL;

   EL(0, "#include <Azy.h>");
   EL(0, "#include \"%s%sCommon.h\"", name, sep);
   if (s->stub_header)
     {
        EL(0, "%s", s->stub_header);
        NL;
     }
   NL;

   if (s->doc)
     EL(0, "%s", s->doc);
   else
     {
        EL(0,
           "/** Implementation specific module data.");
        EL(0, " */ ");
     }

   EL(0, "typedef struct %s%s%s_Module", name, sep, s->name);
   EL(0, "{");
   /* FIXME: does this need to be output at all if there is no attrs stub? */
   EL(0, "%s", s->stub_attrs ? s->stub_attrs : "");
   EL(0, "} %s%s%s_Module;", name, sep, s->name);
   NL;

   EL(0, "#define %s%s%s_module_data_get(Azy_Server_Module) (%s%s%s_Module*)azy_server_module_data_get((Azy_Server_Module))",
      name, sep, s->name, name, sep, s->name);

   if (s->stub_init && s->stub_init[0])
     {
        EL(0, "/** Module constructor.");
        EL(0, " * ");
        EL(0, " * @param module Module object.");
        EL(0, " * ");
        EL(0, " * @return EINA_TRUE if all is ok.");
        EL(0, " */ ");
        EL(0,
           "Eina_Bool %s%s%s_module_init(Azy_Server_Module *module);",
           name, sep, s->name);
        NL;
     }

   if (s->stub_shutdown && s->stub_shutdown[0])
     {
        EL(0, "/** Module destructor.");
        EL(0, " * ");
        EL(0, " * @param _module Module object.");
        EL(0, " */ ");
        EL(0,
           "void %s%s%s_module_shutdown(Azy_Server_Module* _module);",
           name, sep, s->name);
        NL;
     }

   if (s->stub_pre && s->stub_pre[0])
     {
        EL(0, "/** Pre-call hook.");
        EL(0, " * ");
        EL(0, " * @param _module Module object.");
        EL(0, " * @param _content Call object.");
        EL(0, " * ");
        EL(0,
           " * @return EINA_TRUE if you want to continue execution of the call.");
        EL(0, " */ ");
        EL(0,
           "Eina_Bool %s%s%s_module_pre(Azy_Server_Module* _module, Azy_Content* _content);",
           name, sep, s->name);
        NL;
     }

   if (s->stub_post && s->stub_post[0])
     {
        EL(0, "/** Post-call hook.");
        EL(0, " * ");
        EL(0, " * @param _module Module object.");
        EL(0, " * @param _content Call object.");
        EL(0, " * ");
        EL(0,
           " * @return EINA_TRUE if you want to continue execution of the call.");
        EL(0, " */ ");
        EL(0,
           "Eina_Bool %s%s%s_module_post(Azy_Server_Module* _module, Azy_Content* _content);",
           name, sep, s->name);
        NL;
     }

   if (s->stub_fallback && s->stub_fallback[0])
     {
        EL(0,
           "/** Fallback hook. (for undefined methods)");
        EL(0, " * ");
        EL(0, " * @param _module Module object.");
        EL(0, " * @param _content Call object.");
        EL(0, " * ");
        EL(0,
           " * @return EINA_TRUE if you handled the call.");
        EL(0, " */ ");
        EL(0,
           "Eina_Bool %s%s%s_module_fallback(Azy_Server_Module* _module, Azy_Content* _content);",
           name, sep, s->name);
        NL;
     }

   if (s->stub_download && s->stub_download[0])
     {
        EL(0, "/** Download hook.");
        EL(0, " * ");
        EL(0, " * @param _module Module object.");
        EL(0, " * ");
        EL(0,
           " * @return EINA_TRUE if you want to continue execution of the call.");
        EL(0, " */ ");
        EL(0,
           "Eina_Bool %s%s%s_module_download(Azy_Server_Module* _module);",
           name, sep, s->name);
        NL;
     }

   if (s->stub_upload && s->stub_download[0])
     {
        EL(0, "/** Upload hook.");
        EL(0, " * ");
        EL(0, " * @param _module Module object.");
        EL(0, " * ");
        EL(0,
           " * @return EINA_TRUE if you want to continue execution of the call.");
        EL(0, " */ ");
        EL(0,
           "Eina_Bool %s%s%s_module_upload(Azy_Server_Module* _module);",
           name, sep, s->name);
        NL;
     }

   EINA_LIST_FOREACH(s->methods, j, method)
     {
        if (method->doc && method->doc[0])
          EL(0, "%s", method->doc);
        else
          {
             EL(0, "/** ");
             EL(0, " * ");
             EL(0,
                " * @param _module Module object.");

             EINA_LIST_FOREACH(method->params, k, p)
               {
                  EL(0, " * @param %s", p->name);
               }

             EL(0, " * ");
             EL(0, " * @return A valid call id, or 0 on failure");
             EL(0, " */ ");
          }

        E(0, "%s %s%s%s_module_%s(Azy_Server_Module* _module",
          method->return_type->ctype, name, sep, s->name,
          method->name);

        EINA_LIST_FOREACH(method->params, k, p)
          {
             E(0, ", %s %s", p->type->ctype, p->name);
          }

        EL(0, ", Azy_Content* _error);");
        NL;
     }

   EL(0, "#endif");
   fclose(f);


   OPEN("%s/%s%s%s.azy_server.h", out_dir, name, sep, s->name);
   EL(0, "#ifndef %s_%s_AZY_SERVER_H", (azy->name) ? azy->name : "AZY", s->name);
   EL(0, "#define %s_%s_AZY_SERVER_H", (azy->name) ? azy->name : "AZY", s->name);
   NL;

   EL(0, "#include <Azy.h>");
   EL(0, "#include \"%s%s%s.azy_server_stubs.h\"", name, sep, s->name);
   NL;

   EL(0, "Azy_Server_Module_Def* %s%s%s_module_def();",
      name, sep, s->name);
   NL;

   EL(0, "#endif");
   fclose(f);
}

static void
gen_server_impl(Azy_Server_Module *s)
{
   Eina_List *j, *k;
   Azy_Method *method;
   Azy_Method_Param *p;

   OPEN("%s/%s%s%s.azy_server.c", out_dir, name, sep, s->name);

   EL(0, "#include \"%s%s%s.azy_server.h\"", name, sep, s->name);
   EL(0, "#include \"%s%s%s.azy_server_stubs.h\"", name, sep, s->name);
   NL;


   EL(0, "static Azy_Server_Module_Def *module = NULL;");
   NL;
   
   EINA_LIST_FOREACH(s->methods, j, method)
     {
        int n = 0;

        EL(0, "static Eina_Bool method_%s(Azy_Server_Module* _module, Azy_Content* _content)", method->name);
        EL(0, "{");
        // forward declarations
        EL(1, "Eina_Bool _retval = EINA_FALSE;");
        EL(1, "%s _nreturn_value = %s;",
           method->return_type->ctype,
           method->return_type->cnull);
        EL(1, "Azy_Value *_return_value;");

        EINA_LIST_FOREACH(method->params, k, p)
          {
             EL(1, "%s %s = %s;", p->type->ctype,
                p->name,
                p->type->cnull);
          }

        NL;
        EL(1, "EINA_SAFETY_ON_NULL_RETURN_VAL(_module, EINA_FALSE);");
        EL(1, "EINA_SAFETY_ON_NULL_RETURN_VAL(_content, EINA_FALSE);");

        NL;
        EL(1,
           "if (eina_list_count(azy_content_params_get(_content)) != %d) /* number of parameters taken by call */",
           eina_list_count(method->params));
        EL(1, "{");
        EL(2,
           "azy_content_error_faultmsg_set(_content, -1, \"Invalid number of parameters passed to stub function!\");");
        EL(2, "goto out;");
        EL(1, "}");

        EINA_LIST_FOREACH(method->params, k, p)
          {
             NL;
             EL(1,
                "if (!%s(azy_content_param_get(_content, %d), &%s))",
                p->type->demarch_name, n++,
                p->name);
             EL(1, "{");
             EL(2,
                "azy_content_error_faultmsg_set(_content, -1, \"Stub parameter value demarshalization failed. (%s:%s)\");",
                method->name, p->name);
             EL(2, "goto out;");
             EL(1, "}");
          }

// call stub
        NL;
        E(1, "_nreturn_value = %s%s%s_module_%s(_module", name, sep, s->name, method->name);

        EINA_LIST_FOREACH(method->params, k, p)
          {
             E(0, ", %s", p->name);
          }

        EL(0, ", _content);");

// check for errors
        EL(1, "if (azy_content_error_is_set(_content))");
        EL(2, "goto out;");

// prepare retval
        NL;
        EL(1, "_return_value = %s(_nreturn_value);", method->return_type->march_name);
        EL(1, "if (!_return_value)");
        EL(1, "{");
        EL(2, "azy_content_error_faultmsg_set(_content, -1, \"Stub return value marshalization failed. (%s)\");", method->name);
        EL(2, "goto out;");
        EL(1, "}");
        NL;
        EL(1, "azy_content_retval_set(_content, _return_value);");
        EL(1, "_retval = EINA_TRUE;");

// free native types and return
        NL;
        EL(0, "out:");

        if (method->return_type->free_func)
          EL(1, "%s(_nreturn_value);", method->return_type->free_func);

        EINA_LIST_FOREACH(method->params, k, p)
          {
             if ((!p->pass_ownership) && p->type->free_func)
               EL(1, "%s(%s);", p->type->free_func, p->name);
          }

        EL(1, "return _retval;");
        EL(0, "}");
        NL;
     }

   NL;

   EL(0, "Azy_Server_Module_Def *\n%s%s%s_module_def(void)", name, sep, s->name);
   EL(0, "{");
   EL(1, "Azy_Server_Module_Method *method;");
   NL;
   EL(1, "if (module) return module;");
   NL;
   EL(1, "module = azy_server_module_def_new(\"%s%s%s\");", name, sep, s->name);
   EL(1, "EINA_SAFETY_ON_NULL_RETURN_VAL(module, NULL);");
   NL;
   
   E(1, "azy_server_module_def_init_shutdown_set(module, ");
   GET_NAME(init);
   E(0, ", ");
   GET_NAME(shutdown);
   EL(0, ");");

   E(1, "azy_server_module_def_pre_post_set(module, ");
   GET_NAME(pre);
   E(0, ", ");
   GET_NAME(post);
   EL(0, ");");

   E(1, "azy_server_module_def_download_upload_set(module, ");
   GET_NAME(download);
   E(0, ", ");
   GET_NAME(upload);
   EL(0, ");");

   E(1, "azy_server_module_def_fallback_set(module, ");
   GET_NAME(fallback);
   EL(0, ");");
   
   EL(1, "azy_server_module_size_set(module, sizeof(%s%s%s_Module));", name, sep, s->name);
   EINA_LIST_FOREACH(s->methods, j, method)
     {
        EL(1, "method = azy_server_module_method_new(\"%s\", method_%s);", method->name, method->name);
        EL(1, "EINA_SAFETY_ON_NULL_GOTO(method, error);");
        EL(1, "azy_server_module_def_method_add(module, method);");
     }
   NL;
   EL(1, "return module;");
   EL(0, "error:");
   EL(2, "azy_server_module_def_free(module);");
   EL(1, "return NULL;");
   EL(0, "}");
   fclose(f);


   /* ************************STUBS************************* */
   OPEN("%s/%s%s%s.azy_server_stubs.c", out_dir, name, sep, s->name);
   EL(0, "#include \"%s%s%s.azy_server_stubs.h\"", name, sep, s->name);
   NL;

   if (s->stub_header)
     {
        STUB(s->stub_header);
        NL;
     }

   if (s->stub_init)
     {
        EL(0, "Eina_Bool %s%s%s_module_init(Azy_Server_Module* _module)",
           name, sep, s->name);
        EL(0, "{");

        if (strstr(s->stub_init, "data_"))
          EL(1, "%s%s%s_module* data_ = azy_server_module_data_get(_module);", name, sep, s->name);
        else
          EL(1, "(void)_module;");

        STUB(s->stub_init);
        EL(1, "return EINA_TRUE;");
        EL(0, "}");
        NL;
     }

   if (s->stub_shutdown)
     {
        EL(0,
           "void %s%s%s_module_shutdown(Azy_Server_Module* _module)",
           name, sep, s->name);
        EL(0, "{");

        if (strstr(s->stub_shutdown, "data_"))
          EL(1, "%s%s%s_module* data_ = azy_server_module_data_get(_module);",
             name, sep, s->name);
        else
          EL(1, "(void)_module;");

        STUB(s->stub_shutdown);
        EL(0, "}");
        NL;
     }

   if (s->stub_pre)
     {
        EL(0, "Eina_Bool %s%s%s_module_pre(Azy_Server_Module* _module, Azy_Content* _content)", name, sep, s->name);
        EL(0, "{");

        EL(1, "(void)_content;");
        if (strstr(s->stub_pre, "data_"))
          EL(1, "%s%s%s_module* data_ = azy_server_module_data_get(_module);",
             name, sep, s->name);
        else
          EL(1, "(void)_module;");

        STUB(s->stub_pre);
        EL(1, "return EINA_TRUE;");
        EL(0, "}");
        NL;
     }

   if (s->stub_post)
     {
        EL(0,
           "Eina_Bool %s%s%s_module_post(Azy_Server_Module* _module, Azy_Content* _content)",
           name, sep, s->name);
        EL(0, "{");

        EL(1, "(void)_content;");
        if (strstr(s->stub_post, "data_"))
          EL(1,
             "%s%s%s_module* data_ = azy_server_module_data_get(_module);",
             name, sep, s->name);
        else
          EL(1, "(void)_module;");

        STUB(s->stub_post);
        EL(1, "return EINA_TRUE;");
        EL(0, "}");
        NL;
     }

   if (s->stub_fallback)
     {
        EL(0, "Eina_Bool %s%s%s_module_fallback(Azy_Server_Module* _module, Azy_Content* _content)",
           name, sep, s->name);
        EL(0, "{");

        if (strstr(s->stub_fallback, "data_"))
          EL(1, "%s%s%s_module* data_ = azy_server_module_data_get(_module);",
             name, sep, s->name);
        else
          EL(1, "(void)_module;");

        STUB(s->stub_fallback);
        EL(1, "return EINA_TRUE;");
        EL(0, "}");
        NL;
     }

   if (s->stub_download)
     {
        EL(0,
           "Eina_Bool %s%s%s_module_download(Azy_Server_Module* _module)",
           name, sep, s->name);
        EL(0, "{");

        if (strstr(s->stub_download, "data_"))
          EL(1,
             "%s%s%s_module* data_ = azy_server_module_data_get(_module);",
             name, sep, s->name);
        else
          EL(1, "(void)_module;");

        EL(1,
           "Azy_Net* _net = azy_server_module_net_get(_module);");
        STUB(s->stub_download);
        EL(1, "return EINA_FALSE;");
        EL(0, "}");
        NL;
     }

   if (s->stub_upload)
     {
        EL(0,
           "Eina_Bool %s%s%s_module_upload(Azy_Server_Module* _module)",
           name, sep, s->name);
        EL(0, "{");

        if (strstr(s->stub_upload, "data_"))
          EL(1,
             "%s%s%s_module* data_ = azy_server_module_data_get(_module);",
             name, sep, s->name);
        else
          EL(1, "(void)_module;");

        EL(1, "Azy_Net* _net = azy_server_module_net_get(_module);");
        STUB(s->stub_upload);
        EL(1, "return EINA_FALSE;");
        EL(0, "}");
        NL;
     }

   EINA_LIST_FOREACH(s->methods, j, method)
     {
        E(0, "%s %s%s%s_module_%s(Azy_Server_Module* _module",
          method->return_type->ctype, name, sep, s->name,
          method->name);

        EINA_LIST_FOREACH(method->params, k, p)
          {
             E(0, ", %s %s", p->type->ctype, p->name);
          }

        EL(0, ", Azy_Content* _error)");
        EL(0, "{");

/* attempt to evade even more compile warnings at the expense of slightly slower runtime.
* worth iiiiiiiiiiiiiiiiiiiiit
*/
        if ((method->stub_impl) && (strstr(method->stub_impl, "data_")))
          EL(1, "%s%s%s_Module* data_ = azy_server_module_data_get(_module);", name, sep, s->name);
        else
          EL(1, "(void)_module;");

        EL(1, "%s retval = %s;", method->return_type->ctype, method->return_type->cnull);

        if (method->stub_impl)
          {
             EL(1, "(void)_error;");
             STUB(method->stub_impl);
          }
        else
          EL(1, "azy_content_error_faultmsg_set(_error, -1, \"Method is not implemented. (%s)\");",
             method->name);

        EL(1, "return retval;");
        EL(0, "}");
        NL;
     }
   fclose(f);
}

static void
gen_client_headers(Azy_Server_Module *s)
{
   Eina_List *j, *k;
   Azy_Method *method;
   Azy_Method_Param *p;

   OPEN("%s/%s%s%s.azy_client.h", out_dir, name, sep, s->name);
   EL(0, "#ifndef %s_%s_AZY_CLIENT_H", (azy->name) ? azy->name : "AZY", s->name);
   EL(0, "#define %s_%s_AZY_CLIENT_H", (azy->name) ? azy->name : "AZY", s->name);
   NL;

   EL(0, "#include <Azy.h>");
   EL(0, "#include \"%s%sCommon.h\"", name, sep);
   NL;

   EINA_LIST_FOREACH(s->methods, j, method)
     {
        EL(0, "/** ");
        EL(0, " * ");
        EL(0, " * @param _client Client object.");

        EINA_LIST_FOREACH(method->params, k, p)
          {
             EL(0, " * @param %s", p->name);
          }

        EL(0, " * @param _error Error content (cannot be NULL).");
        EL(0, " *");
        EL(0, " * @return ");
        EL(0, " */ ");

        E(0, "Azy_Client_Call_Id %s%s%s_%s(Azy_Client* _client", name, sep, s->name, method->name);

        EINA_LIST_FOREACH(method->params, k, p)
          {
             E(0, ", %s%s %s", !strcmp(p->type->ctype, "char*") ? "const " : "", p->type->ctype, p->name);
          }

        EL(0, ", Azy_Content *_error, const void *data);");
        NL;
     }

   EL(0, "#endif");
   fclose(f);
}

static void
gen_client_impl(Azy_Server_Module *s)
{

   Eina_List *j, *k;
   Azy_Method *method;
   Azy_Typedef *t;
   Azy_Method_Param *p;
   
   OPEN("%s/%s%s%s.azy_client.c", out_dir, name, sep, s->name);

   EL(0, "#include \"%s%s%s.azy_client.h\"", name, sep, s->name);
   NL;

   gen_marshalizers(s, EINA_TRUE);
   EINA_LIST_FOREACH(s->types, k, t)
     gen_type_copyfree(t, EINA_TRUE, EINA_TRUE);
   NL;
   gen_marshalizers(s, EINA_FALSE);
   EINA_LIST_FOREACH(s->types, k, t)
     gen_type_copyfree(t, EINA_FALSE, EINA_TRUE);
   NL;
   EINA_LIST_FOREACH(s->methods, j, method)
     {
        E(0, "Azy_Client_Call_Id %s%s%s_%s(Azy_Client* _client", name, sep, s->name, method->name);

        EINA_LIST_FOREACH(method->params, k, p)
          {
             E(0, ", %s%s %s", !strcmp(p->type->ctype, "char*") ? "const " : "", p->type->ctype, p->name);
          }

        EL(0, ", Azy_Content *_error, const void *data)");
        EL(0, "{");
          EL(1, "Azy_Client_Call_Id _retval = 0;");
      //  EL(1, "%s _retval = %s;", method->return_type->ctype, method->return_type->cnull);

        if (method->params)
          EL(1, "Azy_Value *_param_value;");

        EL(1, "Azy_Content* _content;");
        EL(1, "Azy_Net *_net;");
        EL(1, "Azy_Net_Transport tr = AZY_NET_TRANSPORT_XML;");
        NL;
        EL(1, "EINA_SAFETY_ON_NULL_RETURN_VAL(_client, _retval);");
        EL(1, "EINA_SAFETY_ON_NULL_RETURN_VAL(_error, _retval);");
        NL;
        EL(1, "_content = azy_content_new(\"%s%s%s.%s\");", name, sep, s->name, method->name);
        EL(1, "if (!_content) return 0;");
        EL(1, "if (data) azy_content_data_set(_content, data);");

        EINA_LIST_FOREACH(method->params, k, p)
          {
             NL;
             EL(1, "_param_value = %s(%s);", p->type->march_name, p->name);
             EL(1, "if (!_param_value)");
             EL(1, "{");
             EL(2, "azy_content_error_faultmsg_set(_error, AZY_CLIENT_ERROR_MARSHALIZER, "
                    "\"Call parameter value marshalization failed (param=%s).\");", p->name);
             EL(2, "azy_content_free(_content);");
             EL(2, "return _retval;");
             EL(1, "}");
             EL(1, "azy_content_param_add(_content, _param_value);");
          }

        NL;
        EL(1, "_net = azy_client_net_get(_client);");
        EL(1, "if (azy_net_transport_get(_net) != AZY_NET_TRANSPORT_UNKNOWN)");
        EL(2, "tr = azy_net_transport_get(_net);");
        EL(1, "_retval = azy_client_call(_client, _content, tr, (Azy_Content_Cb)%s);", method->return_type->demarch_name);
        EL(1, "EINA_SAFETY_ON_TRUE_GOTO(!_retval, error);");
        if (method->return_type->free_func)
          EL(1, "azy_client_callback_free_set(_client, _retval, (Ecore_Cb)%s);", method->return_type->free_func);
/*
        EL(1, "{");
        EL(2,"if (!%s(azy_content_retval_get(_content), %s&_retval))",
           method->return_type->demarch_name, (!strcmp(method->return_type->demarch_name, "azy_value_bool_get")) ? "(int*)" : "");
        EL(3,
           "azy_content_error_faultmsg_set(_error, AZY_CLIENT_ERROR_MARSHALIZER, \"Call return value demarshalization failed.\");");
        EL(1, "}");
*/
        NL;
        EL(0, "error:");
        EL(1, "azy_content_free(_content);");
        EL(1, "return _retval;");
        EL(0, "}");
        NL;
     }
   fclose(f);
}

static void
azy_write(void)
{
   Azy_Server_Module *s;
   Eina_List *l;

   if (common_headers)
     gen_common_headers();

   if (common_impl)
     gen_common_impl(NULL);

   EINA_LIST_FOREACH(azy->modules, l, s)
     {
        if (client_headers)
          gen_client_headers(s);

        if (client_impl)
          gen_client_impl(s);

        if (server_headers)
          gen_server_headers(s);

        if (server_impl)
          gen_server_impl(s);

        if (common_impl)
          gen_common_impl(s);
     }
#if 0
   if (azy_gen)
     {
        OPEN("%s/%s.azy", out_dir, azy->name);

        EL(0, "/* Generated AZY */");
        NL;

        EL(0, "namespace %s;", azy->name);
        NL;

        EINA_LIST_FOREACH(azy->errors, j, e)
          {
             EL(0, "error %-22s = %d;", e->name, e->code);
          }

        EINA_LIST_FOREACH(azy->types, j, t)
          {
             if (t->type == TD_STRUCT)
               {
                  NL;
                  EL(0, "struct %s", t->name);
                  EL(0, "{");

                  EINA_LIST_FOREACH(t->struct_members, k, m)
                    {
                       EL(1, "%-24s %s;",
                          azy_typedef_azy_name(m->type), m->name);
                    }

                  EL(0, "}");
               }
          }

        EINA_LIST_FOREACH(azy->modules, j, s)
          {
             NL;
             EL(0, "module %s", s->name);
             EL(0, "{");

             EINA_LIST_FOREACH(s->errors, j, e)
               {
                  EL(1, "error %-18s = %d;", e->name, e->code);
               }

             if (s->errors)
               NL;

             EINA_LIST_FOREACH(s->types, j, t)
               {
                  if (t->type == TD_STRUCT)
                    {
                       EL(1, "struct %s", t->name);
                       EL(1, "{");

                       EINA_LIST_FOREACH(t->struct_members, k, m)
                         {
                            EL(2, "%-20s %s;", azy_typedef_azy_name(m->type), m->name);
                         }

                       EL(1, "}");
                       NL;
                    }
               }

             s->methods = eina_list_sort(s->methods, eina_list_count(s->methods), (Eina_Compare_Cb)azy_method_compare);

             EINA_LIST_FOREACH(s->methods, j, method)
               {
                  E(1, "%-24s %-25s(",
                    azy_typedef_azy_name(method->return_type), method->name);

                  EINA_LIST_FOREACH(method->params, k, p)
                    {
                       if (k == method->params)
                         EL(0, "%-15s %s%s", azy_typedef_azy_name(p->type), p->name, k->next ? "," : ");");
                       else
                         EL(0, "%-55s%-15s %s%s", "", azy_typedef_azy_name(p->type), p->name, k->next ? "," : ");");
                    }

                  if (!method->params)
                    EL(0, ");");
               }

             EL(0, "}");
          }
     }

   /* end */

   fclose(f);
#endif
}

int
main(int argc, char *argv[])
{
   Eina_Bool debug = EINA_FALSE;
   Eina_Bool exit_option = EINA_FALSE;
   Eina_Bool err;
   int args;
   char *modes = "all";

   Ecore_Getopt_Value values[] =
   {
      ECORE_GETOPT_VALUE_STR(modes),
      ECORE_GETOPT_VALUE_STR(out_dir),
      ECORE_GETOPT_VALUE_BOOL(debug),
      ECORE_GETOPT_VALUE_BOOL(exit_option),
      ECORE_GETOPT_VALUE_BOOL(exit_option),
      ECORE_GETOPT_VALUE_BOOL(exit_option),
      ECORE_GETOPT_VALUE_BOOL(exit_option)
   };

   ecore_init();
   ecore_app_args_set(argc, (const char **)argv);
   args = ecore_getopt_parse(&opts, values, argc, argv);
   if (args < 0)
     return 1;

   azy_file = argv[args];

   if (exit_option)
     return 0;

   if (!azy_file)
     {
        printf("You must specify the .azy file.\n");
        return 1;
     }

   if (debug) azyParserTrace(stdout, "azy_parser: ");
   err = EINA_FALSE;
   azy = azy_parse_file(azy_file, &err);
   if ((!azy) || (err))
     {
        printf("Error parsing file!\n");
        exit(1);
     }
   azy_process(azy);
   sep = (azy->name) ? "_" : "";
   name = (azy->name) ? azy->name : "";

   client_headers = strstr(modes, "all") || strstr(modes, "client-headers");
   client_impl = strstr(modes, "all") || strstr(modes, "client-impl");
   common_headers = strstr(modes, "all") || strstr(modes, "common-headers");
   common_impl = strstr(modes, "all") || strstr(modes, "common-impl");
   server_impl = strstr(modes, "all") || strstr(modes, "server-impl");
   server_headers = strstr(modes, "all") || strstr(modes, "server-headers");
   azy_gen = strstr(modes, "azy") || 0;

   if ((!client_headers) && (!client_impl) && (!server_impl) && (!server_headers) && (!azy_gen))
     {
        printf("You have not specified a valid method!\n");
        exit(1);
     }

   i = eina_stringshare_add("int");
   b = eina_stringshare_add("Eina_Bool");
   azy_write();

   if (debug)
     printf("azy-parser: Done!!\n");
   return 0;
}

