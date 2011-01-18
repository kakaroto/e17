#ifndef         ELIXIR_H__
# define        ELIXIR_H__

#ifdef EAPI
#undef EAPI
#endif
#ifdef _MSC_VER
# ifdef BUILDING_DLL
#  define EAPI __declspec(dllexport)
# else
#  define EAPI __declspec(dllimport)
# endif
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <jsapi.h>

#include <Eina.h>

#ifdef BUILD_MODULE_EVAS
# include <Evas.h>
#endif

#define ELIXIR_ID_KEY_FILE "/etc/elixir.keys"

/* Elixir module manipulation definition. */
#define ELIXIR_MODULE_API_VERSION       1

typedef enum elixir_security_level_e elixir_security_level_t;
typedef enum elixir_virtual_chroot_e elixir_virtual_chroot_t;
typedef enum elixir_check_validity_e elixir_check_validity_t;
typedef struct _Elixir_Module_Api Elixir_Module_Api;
typedef struct _Elixir_Sub_Module Elixir_Sub_Module;
typedef struct _Elixir_Module Elixir_Module;
typedef struct _Elixir_Loader Elixir_Loader;
typedef struct _Elixir_Security Elixir_Security;
typedef struct _Elixir_Runtime Elixir_Runtime;
typedef struct _Elixir_Script Elixir_Script;
typedef struct _Elixir_Loader_File Elixir_Loader_File;
typedef struct _Elixir_Loaded_File Elixir_Loaded_File;

enum elixir_security_level_e
{
   ELIXIR_GRANTED,
   ELIXIR_AUTH,
   ELIXIR_AUTH_REQUIRED
};

enum elixir_virtual_chroot_e {
  ELIXIR_VCHROOT_DATA_ONLY,
  ELIXIR_VCHROOT_NOEXEC,
  ELIXIR_VCHROOT_ALL
};

enum elixir_check_validity_e {
  ELIXIR_CHECK_OK,
  ELIXIR_CHECK_DUNO,
  ELIXIR_CHECK_NOK
};

struct _Elixir_Module_Api
{
   int                           version;
   elixir_security_level_t       security;
   const char                   *name;
   const char                   *author;
};

struct _Elixir_Sub_Module
{
   JSObject     *parent;
   JSObject     *item;
};

struct _Elixir_Module
{
   const Elixir_Module_Api *api;
   void                    *data;
   Eina_Bool                security : 1;
   struct
   {
      Eina_Bool (*open)(Elixir_Module* self, JSContext* cx, JSObject* root);
      Eina_Bool (*reset)(Elixir_Module* self, JSContext* cx);
      Eina_Bool (*close)(Elixir_Module* self, JSContext* cx);
   } func;
};

#define ELIXIR_SIGNATURE_SIZE sizeof (unsigned int) * 8
struct _Elixir_Loader
{
   const char *name;
   const char *author;

   const int min_param;
   const int max_param;

   struct
   {
      const Elixir_Loader_File *(*request)(int param, const char **params);
      Eina_Bool                 (*release)(const Elixir_Loader_File *file);
      const void               *(*get)(const Elixir_Loader_File *file, unsigned int *length);
      const void               *(*xget)(const Elixir_Loader_File *file, unsigned int *length);
      const char               *(*filename)(const Elixir_Loader_File *file);
      Eina_Bool                 (*id)(const Elixir_Loader_File *file, char sign[ELIXIR_SIGNATURE_SIZE]);
      const char               *(*section)(const Elixir_Loader_File *file);
   } func;
};

struct _Elixir_Security
{
   const char *name;
   const char *author;

   struct
   {
      elixir_check_validity_t (*check)(const char *sha1, int sha1_length,
				       const char *sign, int sign_length,
				       const char *cert, int cert_length);
   } func;
};

struct _Elixir_Loaded_File
{
   const Elixir_Loader *loader;
   const Elixir_Loader_File *file;
};

EAPI Eina_Bool elixir_modules_register(const Elixir_Module *em);
EAPI Eina_Bool elixir_modules_unregister(const Elixir_Module *em);

EAPI void elixir_security_set(elixir_security_level_t level);
EAPI elixir_security_level_t elixir_security_get(void);

EAPI int elixir_loader_init(int length, const char **authorized);
EAPI int elixir_loader_shutdown(void);
EAPI Eina_Bool elixir_loader_register(const Elixir_Loader *el);
EAPI Eina_Bool elixir_loader_unregister(const Elixir_Loader *el);

EAPI Elixir_Loaded_File *elixir_loader_load(int param, const char **params);
EAPI void elixir_loader_unload(const Elixir_Loaded_File *load);
EAPI const char *elixir_loader_filename(const Elixir_Loaded_File *load);
EAPI const char *elixir_loader_section(const Elixir_Loaded_File *load);
EAPI Eina_Bool elixir_loader_id(const Elixir_Loaded_File *load, char signature[ELIXIR_SIGNATURE_SIZE]);
EAPI const void *elixir_loader_content(const Elixir_Loaded_File *load, unsigned int *size);
EAPI const void *elixir_loader_compiled(const Elixir_Loaded_File *load, unsigned int *size);

EAPI Eina_Bool elixir_security_register(const Elixir_Security *es);
EAPI Eina_Bool elixir_security_unregister(const Elixir_Security *es);
EAPI int elixir_security_init(int length, const char **authorized);
EAPI int elixir_security_shutdown(void);
EAPI elixir_check_validity_t elixir_security_check(const char *sha1, unsigned int sha1_length,
						   const void *signature, unsigned int signature_length,
						   const void *certificate, unsigned int certificate_length);

EAPI void                 elixir_modules_init(void);
EAPI Elixir_Module       *elixir_modules_find(const char* name);
EAPI int                  elixir_modules_load(Elixir_Module* em, JSContext* cx, JSObject* root);
EAPI int                  elixir_modules_unload(Elixir_Module* em, JSContext* cx);
EAPI int                  elixir_modules_shutdown(void);

/* Elixir setup and execution helper. */
struct _Elixir_Runtime
{
   JSContext            *cx;
   JSRuntime            *rt;
   JSObject             *root;
   Elixir_Module        *em;
   unsigned              clone:1;
};

struct _Elixir_Script
{
   Elixir_Runtime       *er;

   JSScript             *script;
   JSObject             *jscript;

   FILE                 *out;

   Elixir_Loaded_File   *file;

   unsigned int		 push : 1;
};

EAPI Elixir_Runtime *elixir_init(void);
EAPI Elixir_Runtime *elixir_clone(JSContext *cx, JSObject *root);
EAPI Eina_Bool       elixir_reset(Elixir_Runtime *er);
EAPI void            elixir_shutdown(Elixir_Runtime *er);
EAPI void            elixir_maybe_shutdown(Elixir_Runtime *er);

EAPI void      elixir_void_register(JSContext *cx, void *data);
EAPI void      elixir_void_unregister(JSContext *cx, void *data);
EAPI void      elixir_increase_count(JSContext *cx);
EAPI void      elixir_decrease_count(JSContext *cx);
EAPI void      elixir_lock_cx(JSContext *cx);
EAPI void      elixir_unlock_cx(JSContext *cx);
EAPI void      elixir_suspended_gc(void);
EAPI Eina_Bool elixir_string_register(JSContext *cx, JSString **string);
EAPI Eina_Bool elixir_string_unregister(JSContext *cx, JSString **string);
EAPI Eina_Bool elixir_object_named_register(JSContext *cx, const char *name, JSObject **obj, void *data);
EAPI Eina_Bool elixir_object_unregister(JSContext *cx, JSObject **obj);
EAPI Eina_Bool elixir_rval_named_new(JSContext *cx, const char *name, JSClass *class, void *data, jsval *rval);
EAPI Eina_Bool elixir_rval_named_register(JSContext *cx, const char *name, jsval *rval);
EAPI Eina_Bool elixir_rval_delete(JSContext *cx, jsval *rval);

#define ELIXIR_DEFINE_TO_TOKEN(def) #def
#define ELIXIR_DEFINE_TO_STRING(def) ELIXIR_DEFINE_TO_TOKEN(def)

#define elixir_object_register(Cx, Obj, Data) elixir_object_named_register(Cx, (__FILE__ ":" ELIXIR_DEFINE_TO_STRING(__LINE__)), Obj, Data)
#define elixir_rval_new(Cx, Class, Data, Rval) elixir_rval_named_new(Cx, (__FILE__ ":" ELIXIR_DEFINE_TO_STRING(__LINE__)), Class, Data, Rval)
#define elixir_rval_register(Cx, Rval) elixir_rval_named_register(Cx, (__FILE__ ":" ELIXIR_DEFINE_TO_STRING(__LINE__)), Rval)

EAPI Elixir_Script      *elixir_script_file(Elixir_Runtime *er, int param, const char **params);
EAPI Elixir_Script      *elixir_script_template(Elixir_Runtime *er, int param, const char **params);
EAPI void                elixir_script_set_out(Elixir_Script *es, FILE *out);
EAPI FILE               *elixir_script_get_current_out();
EAPI Eina_Bool           elixir_script_run(Elixir_Script *es, jsval *rval);
EAPI char               *elixir_script_binary(Elixir_Script *es, unsigned int *size);
EAPI void                elixir_function_start(JSContext *cx);
EAPI Eina_Bool           elixir_function_run(JSContext *cx, JSFunction *callback, JSObject *parent, int argc, jsval *argv, jsval *rval);
EAPI void                elixir_function_stop(JSContext *cx);
EAPI void                elixir_function_gc(JSContext *cx);
EAPI Eina_Bool           elixir_function_suspended(JSContext *cx);
EAPI void                elixir_script_free(Elixir_Script *es);

EAPI char               *elixir_template(const unsigned char *file_data, unsigned int file_length, unsigned int *template_length);

/* Elixir javascript void wrapper. */
EAPI void               *elixir_void_new(JSContext *cx, JSObject *parent, jsval any, void *private);
EAPI jsval               elixir_void_free(void *data);
EAPI void                elixir_void_move(JSContext *replace, void *data);
EAPI JSObject           *elixir_void_get_parent(void *data);
EAPI JSObject		*elixir_void_set_parent(const void *data, JSObject *parent);
EAPI jsval               elixir_void_get_jsval(void *data);
EAPI void               *elixir_void_get_private(void *data);
EAPI void		 elixir_void_set_private(void *data, void *private);
EAPI JSContext          *elixir_void_get_cx(void *data);

/* Elixir parameters check. */
typedef enum elixir_parameter_type_e	elixir_parameter_type_t;
typedef struct elixir_parameter_s       elixir_parameter_t;
typedef struct elixir_parameters_s	elixir_parameters_t;
typedef struct elixir_value_s		elixir_value_t;

enum elixir_parameter_type_e
{
   JBOOLEAN = 0,
   JINT = 1,
   JDOUBLE = 2,
   JSTRING = 3,
   JOBJECT = 4,
   JANY = 5,
   JNULL = 6,
   JFUNCTION = 7
};

struct elixir_parameter_s
{
  const char                    *name;
  elixir_parameter_type_t	 type;
  const JSClass                 *class;
};

struct elixir_value_s
{
   union {
      int	   num;
      int	   bol;
      jsdouble	   dbl;
      JSString    *str;
      JSObject    *obj;
      JSFunction  *fct;
      jsval	   any;
   } v;
};

extern const elixir_parameter_t          int_parameter;
extern const elixir_parameter_t          boolean_parameter;
extern const elixir_parameter_t          string_parameter;
extern const elixir_parameter_t          double_parameter;
extern const elixir_parameter_t          jsobject_parameter;
extern const elixir_parameter_t          any_parameter;
extern const elixir_parameter_t          function_parameter;
extern const elixir_parameter_t          null_parameter;

extern elixir_parameter_t                color_parameter;
extern elixir_parameter_t                size_parameter;
extern elixir_parameter_t                geometry_parameter;
extern elixir_parameter_t                position_parameter;
extern elixir_parameter_t		 eet_parameter;

extern const elixir_parameter_t         *void_params[];
extern const elixir_parameter_t         *string_params[];
extern const elixir_parameter_t         *int_params[];
extern const elixir_parameter_t         *boolean_params[];
extern const elixir_parameter_t         *function_params[];
extern const elixir_parameter_t         *double_params[];
extern const elixir_parameter_t         *jsobject_params[];

EAPI int                     elixir_params_check_class(const JSClass *object, const JSClass *match);
EAPI elixir_parameter_type_t elixir_params_get_type(jsval arg);
EAPI int                     elixir_params_check(JSContext *cx,
                                                 const elixir_parameter_t **params, elixir_value_t *values,
                                                 uintN argc, jsval *argv);
EAPI int                     elixir_params_check_with_options(JSContext *cx,
                                                              const elixir_parameter_t *params[], elixir_value_t *values,
                                                              uintN argc, jsval *argv, unsigned int optional);
EAPI JSBool                  elixir_int_params_void(int (*func)(void),
                                                    JSContext *cx, uintN argc, jsval *vp);
EAPI JSBool                  elixir_void_params_void(void (*func)(void),
                                                     JSContext *cx, uintN argc, jsval *vp);

#define ELIXIR_FN(Func, nargs, flags, extra)		\
  JS_FN(#Func, elixir_##Func, nargs, extra + nargs, flags)

#define ELIXIR_FS(Func, nargs, flags, extra)		\
  JS_FS(#Func, elixir_##Func, nargs, flags, extra)

#define FAST_CALL_PARAMS(Func, Func_Params)		\
  static JSBool						\
  elixir_##Func(JSContext *cx, uintN argc, jsval *vp)	\
  {							\
     return Func_Params(Func, cx, argc, vp);		\
  }

#define FAST_CALL_PARAMS_CAST(Func, Func_Params)	\
  static JSBool						\
  elixir_##Func(JSContext *cx, uintN argc, jsval *vp)	\
  {							\
     return Func_Params(_elixir_##Func, cx, argc, vp);	\
  }

#define FAST_CALL_PARAMS_SPEC(Func, Func_Params, Params)		\
  static JSBool								\
  elixir_##Func(JSContext *cx, uintN argc, jsval *vp)			\
  {									\
     return Func_Params(Func, Params, cx, argc, vp);			\
  }

#define FAST_CALL_PARAMS_SPEC_CAST(Func, Func_Params, Params)		\
  static JSBool								\
  elixir_##Func(JSContext *cx, uintN argc, jsval *vp)			\
  {									\
     return Func_Params( _elixir_##Func, Params, cx, argc, vp);		\
  }

#define FAST_CALL_PARAMS_2SPEC(Func, Func_Params, Params1, Params2)	\
  static JSBool								\
  elixir_##Func(JSContext *cx, uintN argc, jsval *vp)			\
  {									\
     return Func_Params(Func, Params1, Params2, cx, argc, vp);		\
  }


#define CALL_PARAMS(Func, Func_Params)                                 \
  static JSBool                                                        \
  elixir_##Func(JSContext *cx, JSObject *obj,                          \
                uintN argc, jsval *argv,                               \
                jsval *rval)                                           \
  {                                                                    \
     (void) obj;                                                       \
     return Func_Params( #Func , Func, cx, argc, argv, rval);	       \
  }

#define CALL_PARAMS_STUPID_GCC(Func, Func_Params)	\
  static JSBool						\
  elixir_##Func(JSContext *cx, JSObject *obj,                          \
                uintN argc, jsval *argv,                               \
                jsval *rval)                                           \
  {                                                                    \
     (void) obj;                                                       \
     return Func_Params( #Func , _elixir_##Func, cx, argc, argv, rval);	\
  }

#define CALL_PARAMS_SPEC(Func, Func_Params, Params)                              \
  static JSBool                                                                  \
  elixir_##Func(JSContext *cx, JSObject *obj,                                    \
                uintN argc, jsval *argv,                                         \
                jsval *rval)                                                     \
  {                                                                              \
     (void) obj;                                                                 \
     return Func_Params( #Func , Func, Params, cx, argc, argv, rval);		 \
  }

#define CALL_PARAMS_SPEC_STUPID_GCC(Func, Func_Params, Params)                   \
  static JSBool                                                                  \
  elixir_##Func(JSContext *cx, JSObject *obj,                                    \
                uintN argc, jsval *argv,                                         \
                jsval *rval)                                                     \
  {                                                                              \
     (void) obj;                                                                 \
     return Func_Params( #Func , _elixir_##Func, Params, cx, argc, argv, rval);  \
  }

#define CALL_PARAMS_2SPEC(Func, Func_Params, Params1, Params2)                           \
  static JSBool                                                                          \
  elixir_##Func(JSContext *cx, JSObject *obj,                                            \
                uintN argc, jsval *argv,                                                 \
                jsval *rval)                                                             \
  {                                                                                      \
     (void) obj;                                                                         \
     return Func_Params( #Func , Func, Params1, Params2, cx, argc, argv, rval);          \
  }


/* Manipulation of JS object/properties. */
/* FIXME: Add function for deleting propertie/objects. */
EAPI JSObject           *elixir_object_create(Elixir_Runtime *er, JSObject *parent, const char *name,
                                              void* (*callback)(void *handle, const char **key, const char **value, void *context),
                                              void *context);
EAPI JSObject *elixir_object_get_object(JSContext *cx, JSObject* parent, const char* name);
EAPI Eina_Bool elixir_object_get_int(JSContext *cx, JSObject *obj, const char *name, int *value);
EAPI Eina_Bool elixir_object_get_str(JSContext *cx, JSObject *obj, const char *name, const char **value);
EAPI Eina_Bool elixir_object_get_dbl(JSContext *cx, JSObject *obj, const char *name, double *value);
EAPI Eina_Bool elixir_object_get_fct(JSContext *cx, JSObject *obj, const char *name, JSFunction **fct);

EAPI char               *elixir_get_string(JSContext *cx, jsval arg);
EAPI int                 elixir_get_int(JSContext *cx, jsval arg);
EAPI double              elixir_get_dbl(JSContext *cx, jsval arg);
EAPI JSObject           *elixir_get_obj(JSContext *cx, jsval arg);
EAPI JSFunction         *elixir_get_fct(JSContext *cx, jsval arg);

#define GET_ICOMPONENT(Cx, Obj, Value)				    \
  if (Value)							    \
    if (!elixir_object_get_int(Cx, Obj, #Value, Value))		    \
      return EINA_FALSE;

#define GET_SCOMPONENT(Cx, Obj, Value)				    \
  if (Value)							    \
    if (!elixir_object_get_str(Cx, Obj, #Value, Value))		    \
      return EINA_FALSE;

#define GET_DCOMPONENT(Cx, Obj, Value)				    \
  if (Value)							    \
    if (!elixir_object_get_dbl(Cx, Obj, #Value, Value))		    \
      return EINA_FALSE;

EAPI JSString           *elixir_dup(JSContext *cx, const char *str);
EAPI JSString           *elixir_ndup(JSContext *cx, const char *str, int length);
EAPI const char         *elixir_get_string_bytes(JSString *str, size_t *length);
EAPI Eina_Bool           elixir_add_str_prop(JSContext *cx, JSObject *obj,
                                             const char *name, const char *value);
EAPI Eina_Bool           elixir_add_int_prop(JSContext *cx, JSObject *obj,
                                             const char *name, int value);
EAPI Eina_Bool           elixir_add_bool_prop(JSContext *cx, JSObject *obj,
                                              const char *name, int value);
EAPI Eina_Bool           elixir_add_dbl_prop(JSContext *cx, JSObject *obj,
                                             const char *name, double value);
EAPI Eina_Bool           elixir_add_object_prop(JSContext *cx, JSObject *obj,
                                                const char *name, JSObject *value);

EAPI JSObject *elixir_new_color(JSContext *cx, int r, int g, int b, int a);
EAPI Eina_Bool elixir_extract_color(JSContext *cx, JSObject *obj, int *r, int *g, int *b, int *a);

EAPI JSObject *elixir_new_size(JSContext *cx, int w, int h);
EAPI Eina_Bool elixir_extract_size(JSContext *cx, JSObject *obj, int *w, int *h);

EAPI JSObject *elixir_new_geometry(JSContext *cx, int x, int y, int w, int h);
EAPI Eina_Bool elixir_extract_geometry(JSContext *cx, JSObject *obj, int *x, int *y, int *w, int *h);

EAPI JSObject *elixir_new_position(JSContext *cx, int x, int y);
EAPI Eina_Bool elixir_extract_position(JSContext *cx, JSObject *obj, int *x, int *y);

#ifdef BUILD_MODULE_EVAS
EAPI Eina_Bool evas_object_to_jsval(JSContext *cx, Evas_Object *obj, jsval *rval);
#endif

/* Must become useless. */
extern FILE*            out;

struct elixir_file_s
{
   const char*          type;
   const char*          class;

   int                  file_name_length;
   const char*          file_name;
   const char*          short_name;
   const char*          full_name;
   const char*          path;
   const char*          mime;

   int                  size;
};

EAPI Eina_Bool             elixir_file_init();
EAPI Eina_Bool             elixir_file_virtual_chroot_set(const char* directory);
EAPI void                  elixir_file_virtual_chroot_directive_set(elixir_virtual_chroot_t evct);
EAPI Eina_Bool             elixir_file_chroot_set(const char* directory);
EAPI const char           *elixir_file_virtual_chroot_get();
EAPI Eina_Bool             elixir_file_register(const char *extention, const char *name);
EAPI Eina_Bool             elixir_file_unregister(const char *extention, const char *name);
EAPI Eina_Bool             elixir_file_magic_register(const char *mime, const char *name);
EAPI Eina_Bool             elixir_file_magic_unregister(const char *mime, const char *name);
EAPI Eina_Bool             elixir_filter_register(const char *filter);
EAPI Eina_Bool             elixir_filter_unregister(const char *filter);
EAPI const char           *elixir_file_type(const char *filename);
EAPI char                 *elixir_file_canonicalize(const char *path);
EAPI char                 *elixir_exe_canonicalize(const char *filename);
EAPI struct elixir_file_s *elixir_file_stat(const char *file);
EAPI JSObject             *elixir_file_stat_object(JSContext *cx, struct elixir_file_s *file);
EAPI Eina_Bool             elixir_file_shutdown();

/* Debugging function stuff. */
typedef enum _Elixir_Debug_Mode Elixir_Debug_Mode;
enum _Elixir_Debug_Mode
{
   ELIXIR_DEBUG_STDERR,
   ELIXIR_DEBUG_STDOUT,
   ELIXIR_DEBUG_CONSOLE,
   ELIXIR_DEBUG_SYSLOG,
   ELIXIR_DEBUG_FILE,
   ELIXIR_DEBUG_NONE
};

EAPI extern void                (*elixir_debug_print)(const char *fmt, ...);
EAPI void                         elixir_debug_print_switch(Elixir_Debug_Mode mode, void *data);

EAPI JSObject *elixir_build_ptr(JSContext *cx, void *ret, JSClass *cls);
EAPI JSObject *elixir_return_ptr(JSContext *cx, jsval *vp, void *ret, JSClass *cls);
EAPI JSString *elixir_return_str(JSContext *cx, jsval *vp, const char *str);
EAPI JSString *elixir_return_strn(JSContext *cx, jsval *vp, const char *str, int length);
EAPI JSObject *elixir_return_array(JSContext *cx, jsval *vp);
EAPI JSObject *elixir_return_script(JSContext* cx, jsval *vp, Elixir_Script *es);

#define GET_PRIVATE_S(CX, OBJ, PTR)		\
  if (!OBJ)					\
    return JS_TRUE;				\
  PTR = JS_GetPrivate(CX, OBJ);			\
  if (!PTR)					\
    return JS_TRUE;

#define GET_PRIVATE(CX, OBJ, PTR)		\
  if (!OBJ)					\
    {						\
       JS_ReportError(CX, "Empty object !\n");	\
       return JS_FALSE;				\
    }						\
  PTR = JS_GetPrivate(CX, OBJ);			\
  if (!PTR)					\
    {						\
       JS_ReportError(CX, "Empty object !\n");	\
       return JS_FALSE;				\
    }

EAPI void                        elixir_class_init(void);
EAPI void                        elixir_class_shutdown(void);
EAPI JSClass                    *elixir_class_request(const char* class_name, const char *class_parent);
EAPI const char                 *elixir_class_request_parent_name(const JSClass *cl);
EAPI JSClass                    *elixir_class_request_parent(const JSClass *cl);

/* USID function. */
EAPI void                        elixir_id_init(void);
EAPI Eina_Bool                   elixir_id_compute(char sign[ELIXIR_SIGNATURE_SIZE], int length, const char *buffer);
EAPI int                         elixir_id_push(const Elixir_Loaded_File *file);
EAPI void                        elixir_id_pop(void);
EAPI void                        elixir_id_close(void);
EAPI const char                 *elixir_id_gid(void);
EAPI const char                 *elixir_id_cid(void);
EAPI const unsigned char        *elixir_id_content(unsigned int* size);
EAPI const char                 *elixir_id_filename(void);
EAPI const char                 *elixir_id_section(void);
EAPI void                        elixir_id_shutdown(void);

EAPI void                        elixir_context_add(const char *key, const void *data);
EAPI void                       *elixir_context_find(const char *key);
EAPI void                        elixir_context_delete(const char *key);

typedef struct _Elixir_Eet_Data Elixir_Eet_Data;
struct _Elixir_Eet_Data
{
   unsigned int count;
   void *data;
};

EAPI void elixir_eet_init(JSContext *cx, JSObject *parent);
EAPI void elixir_eet_shutdown(JSContext *cx, JSObject *parent);
EAPI Elixir_Eet_Data *elixir_to_eet_data(JSContext *cx, jsval data, const char *name, jsval descriptor, const char *cipher);
EAPI JSObject *elixir_from_eet_data(JSContext *cx, Elixir_Eet_Data *data, const char *cipher);
EAPI JSObject *elixir_eet_data_new(JSContext *cx, void *data, int length);

#ifndef strdupa
# define strdupa(str)							\
  ({									\
     size_t __len = strlen(str) + 1;					\
     char *__new = (char *) alloca(__len);				\
     (char*) memcpy (__new, str, __len);				\
  })
#endif

#ifndef strndupa
# define strndupa(str, len)						\
  ({									\
     size_t __len;							\
     char *__new;							\
     for (__len = 0; __len < len && str[__len] != '\0'; ++__len)	\
       ;								\
     __new = (char*) alloca(__len);					\
     (char*) memcpy (__new, str, __len);				\
  })
#endif

typedef struct _Elixir_Jsmap Elixir_Jsmap;
struct _Elixir_Jsmap
{
   jsval val;
   int type;
   void *data;
};

EAPI Eina_List *elixir_jsmap_add(Eina_List *list, JSContext *cx, jsval val, void *data, int type);
EAPI Eina_List *elixir_jsmap_del(Eina_List *list, JSContext *cx, jsval val, int type);
EAPI void *elixir_jsmap_find(Eina_List **list, jsval val, int type);
EAPI void elixir_jsmap_free(Eina_List *list, JSContext *cx);

EAPI unsigned int elixir_api_version_get(void);
EAPI void elixir_api_version_set(unsigned int version);

EAPI void elixir_thread_new(void);
EAPI void elixir_thread_del(void);

#endif          /* ELIXIR_H__ */
