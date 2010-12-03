/*
 * Copyright 2006-2008 Ondrej Jirman <ondrej.jirman@zonio.net>
 * Copyright 2010 Mike Blumenkrantz <mike@zentific.com>
 */

#ifndef AZY_H
#define AZY_H

#include <Eina.h>

enum
{
   TD_BASE,
   TD_STRUCT,
   TD_ARRAY,
   TD_ANY,
};

typedef struct _Azy_Typedef       Azy_Typedef;
typedef struct _Azy_Struct_Member Azy_Struct_Member;
typedef struct _Azy_Method_Param  Azy_Method_Param;
typedef struct _Azy_Method        Azy_Method;
typedef struct _Azy_Server_Module Azy_Server_Module;
typedef struct _Azy_Model         Azy_Model;
typedef struct _Azy_Error_Code    Azy_Error_Code;

struct _Azy_Typedef
{
   int                 type; /* typedef node type */
   Eina_Bool           fcheader : 1;
   Eina_Bool           fcfunc : 1;
   Eina_Bool           mheader : 1;
   Eina_Bool           mfunc : 1;
   const char         *name; /* name of the type (for use in ZER) */
   const char         *cname; /* name of the type (for use in ZER) */
   const char         *ctype; /* C type name */
   const char         *cnull; /* null value in C for this type */
   Azy_Server_Module *module;         /* module owning this type */

   const char         *march_name;
   const char         *demarch_name;
   const char         *free_func;
   const char         *copy_func;
   const char         *eq_func;
   const char         *print_func;
   const char         *fmt_str;
   const char         *isnull_func;

   Eina_List          *struct_members; /* struct members list */
   Azy_Typedef       *item_type; /* array item type */
   const char         *doc;
};

struct _Azy_Struct_Member
{
   const char   *name;
   Azy_Typedef *type;
};

struct _Azy_Method_Param
{
   const char   *name;
   int           pass_ownership;
   Azy_Typedef *type;
};

struct _Azy_Method
{
   const char   *name;
   Eina_List    *params;
   Azy_Typedef *return_type;
   const char   *stub_impl;
   int           stub_impl_line;
   const char   *doc;
};

struct _Azy_Server_Module
{
   const char *name;

   Eina_List  *types;        /* module types */
   Eina_List  *methods;      /* methods */
   Eina_List  *errors;

   const char *stub_header;
   const char *stub_init;
   const char *stub_shutdown;
   const char *stub_attrs;
   const char *stub_pre;
   const char *stub_post;
   const char *stub_fallback;
   const char *stub_download;
   const char *stub_upload;
   int         stub_header_line;
   int         stub_init_line;
   int         stub_shutdown_line;
   int         stub_attrs_line;
   int         stub_pre_line;
   int         stub_post_line;
   int         stub_fallback_line;
   int         stub_download_line;
   int         stub_upload_line;

   const char *doc;
};

struct _Azy_Error_Code
{
   const char *name;
   const char *cenum;      /* C enum value */
   const char *msg;
   int         code;
   const char *doc;
};

struct _Azy_Model
{
   const char         *name;
   Eina_List          *errors;
   Eina_List          *modules;
   Eina_List          *types; /* global types */

   // parser helpers
   Azy_Server_Module *cur_module;
};

Azy_Model *azy_new(void);

Azy_Model *azy_parse_string(const char *str,
                              Eina_Bool  *err);
Azy_Model *azy_parse_file(const char *str,
                            Eina_Bool  *err);

Azy_Error_Code *azy_error_new(Azy_Model         *azy,
                                Azy_Server_Module *module,
                                const char         *name,
                                int                 code,
                                const char         *msg);

Azy_Typedef *azy_typedef_new_array(Azy_Model         *azy,
                                     Azy_Server_Module *module,
                                     Azy_Typedef       *item);
Azy_Typedef *azy_typedef_new_struct(Azy_Model         *azy,
                                      Azy_Server_Module *module,
                                      const char         *name);

int azy_method_compare(Azy_Method *m1,
                        Azy_Method *m2);

Azy_Typedef *azy_typedef_find(Azy_Model         *azy,
                                Azy_Server_Module *module,
                                const char         *name);
const char *azy_typedef_vala_name(Azy_Typedef *t);
const char *azy_typedef_azy_name(Azy_Typedef *t);

void        azy_process(Azy_Model *ctx);

const char *azy_stringshare_toupper(const char *str);

#endif
