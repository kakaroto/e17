/*
 * Copyright (c) 2010 ProFUSION
 * Copyright (c) 2010 Lucas De Marchi <lucas.demarchi@profusion.mobi>
 *
 * This file is based on standard.h distributed with coccinelle. Add macros with
 * caution to this file.
 */
/* ****************************************************************************
 * Prelude, this file is used with -macro_file_builtins option of the C parser
 * ****************************************************************************/

/* This file contains:
 *   - macros found in <.h>
 *   - macros found in ".h"
 *     but where we cant detect that it will be a "bad macro"
 *   - hints, cf below.
 *
 * A "bad macro" is a macro using free variables or when expanded
 * that influence the control-flow of the code. In those cases it
 * is preferable to expand the macro so that the coccinelle engine
 * has a more accurate representation of what is going on.
 *
 *
 *
 *
 * old: this file was also containing what is below but now we
 * try to expand on demand the macro found in the c file, so those cases
 * are not needed any more:
 *   - macros found in .c; macros that cannot be parsed.
 *     In the future should be autodetected
 *     (not so easy to do same for macros in .h cos require to access .h file)
 *   - macros found in .c; macros correctly parsed
 *     but where we cant detect that it will be a "bad macro"
 *
 * Some of those macros could be deleted and the C code rewritten because
 * they are "bad" macros.
 *
 * todo? perhaps better if could enable/disable some of those expansions
 * as different software may use conflicting macros.
 *
 *
 * can maybe have a look in sparse/lib.c to see a list of default #define
 * handled builtin apparently by gcc.
 */

// ****************************************************************************
// Hints
// ****************************************************************************

/* Cooperation with parsing_hack.ml: some body of macros in this file, such
 * as MACROSTATEMENT, are considered as magic strings.
 * I can't just expand those macros into some 'whatever();' because I need
 * to generate a TMacroStmt for solving some ambiguities in the grammar
 * for the toplevel stuff I think.
 * Right now a set of special strings are used as "hints" to the parser
 * to help it parse code. Those hints are specified in parsing_hack.ml:
 *
 *   - YACFE_ITERATOR
 *   - YACFE_DECLARATOR
 *   - YACFE_STRING
 *   - YACFE_STATEMENT, or MACROSTATEMENT
 *   - YACFE_ATTRIBUTE
 *   - YACFE_IDENT_BUILDER
 */

// ****************************************************************************
// Test macros
// ****************************************************************************

// #define FOO(a, OP, b) a OP b
// #define FOO(a,b) fn(a,b)
#define FOO_METH_TEST(a) YACFE_IDENT_BUILDER

//#define FOO YACFE_DECLARATOR

// ****************************************************************************
// Generic macros
// ****************************************************************************

// this is defined by windows compiler, and so can not be found via a macro
// after a -extract_macros
// update: now handled in lexer, simplify stuff
//#define __stdcall /*could: YACFE_ATTRIBUTE*/
//#define __declspec(a)

//#define WINAPI
//#define CALLBACK


// ----------------------------------------------------------------------------
// Alias keywords
// ----------------------------------------------------------------------------
// pb, false positive, can also be a #define cst and use as 'case CONST:'
//#define CONST const


#define STATIC static
#define _static static

#define  noinline

#define  STATIC_INLINE static inline
#define  __EXTERN_INLINE extern inline

#define  IFDEBUG if

// ----------------------------------------------------------------------------
// linkage
// ----------------------------------------------------------------------------

#define  fastcall
#define  asmlinkage

/* cf gcc-linux.h
 * A trick to suppress uninitialized variable warning without generating any
 * code
 */
#define uninitialized_var(x) x = x
// as in u16 uninitialized_var(ioboard_type);	/* GCC be quiet */

// loop, macro without ';', single macro. ex: DEBUG()

// TODO should find the definition because we don't use 'x' and so
// may lose code sites with coccinelle. If expand correctly, will
// still don't transform correctly but at least will detect the place.


#define ASSERT(x) MACROSTATEMENT
#define CHECK_NULL(x) MACROSTATEMENT

#define DEBUG(x) MACROSTATEMENT
#define DBG(x) MACROSTATEMENT

// ----------------------------------------------------------------------------
// EFL
// ----------------------------------------------------------------------------
// It's better to separate this to specific projects and call each one with it's
// own macros.

// libcheck
#define END_TEST
#define fail_if(x) if(x) return
#define START_TEST(x) void x##(void)

#define DECL(_x) \
static void _x ()\

// attributes
#define EAPI
#define __EXPORT__
#define EINA_ARG_NONNULL(...)
#define EINA_PURE
#define EINA_WARN_UNUSED_RESULT
#define EINTERN
#define __UNUSED__
#define UNUSED
#define ensure_unused
#define ensure_restrict
#define __NORETURN__
#define __restrict__
#define restrict __restrict__
#define __EINA_UNLIKELY__ unlikely

#define EINA_INLIST Eina_Inlist __in_list
#define ECORE_MAGIC Ecore_Magic __magic
#define EINA_MAGIC Eina_Magic __magic;
#define EINA_RBTREE Eina_Rbtree __rbtree

// eina/src/lib/eina_main.c
#define LOCK() MACROSTATEMENT
#define UNLOCK() MACROSTATEMENT
#define UNLOCK_FORCE() MACROSTATEMENT
# define PTHREAD_MUTEX_INITIALIZER \
  { { 0, 0, 0, 0, 0, 0, { 0, 0 } } }

// eina/src/lib/eina_stringshare.c
#define STRINGSHARE_LOCK_SMALL() MACROSTATEMENT
#define STRINGSHARE_UNLOCK_SMALL() MACROSTATEMENT
#define STRINGSHARE_LOCK_BIG() MACROSTATEMENT
#define STRINGSHARE_UNLOCK_BIG() MACROSTATEMENT

// list iterators
#define EINA_LIST_FOREACH(list, l, data) for (l = list, data = eina_list_data_get(l); l; l = eina_list_next(l), data = eina_list_data_get(l))
#define EINA_LIST_REVERSE_FOREACH(list, l, data) for (l = eina_list_last(list), data = eina_list_data_get(l); l; l = eina_list_prev(l), data = eina_list_data_get(l))
#define EINA_LIST_FOREACH_SAFE(list, l, l_next, data) for (l = list, l_next = eina_list_next(l), data = eina_list_data_get(l); l; l = l_next, l_next = eina_list_next(l), data = eina_list_data_get(l))
#define EINA_LIST_REVERSE_FOREACH_SAFE(list, l, l_prev, data) for (l = eina_list_last(list), l_prev = eina_list_prev(l), data = eina_list_data_get(l); l; l = l_prev, l_prev = eina_list_prev(l), data = eina_list_data_get(l))
#define EINA_LIST_FREE(list, data) for (data = eina_list_data_get(list); list; list = eina_list_remove_list(list, list), data = eina_list_data_get(list))

#define EINA_INLIST_FOREACH(list, l) for (l = NULL, l = (list ? _EINA_INLIST_CONTAINER(l, list) : NULL); l; l = (EINA_INLIST_GET(l)->next ? _EINA_INLIST_CONTAINER(l, EINA_INLIST_GET(l)->next) : NULL))
#define EINA_INLIST_REVERSE_FOREACH(list, l) for (l = NULL, l = (list ? _EINA_INLIST_CONTAINER(l, list->last) : NULL); l; l = (EINA_INLIST_GET(l)->prev ? _EINA_INLIST_CONTAINER(l, EINA_INLIST_GET(l)->prev) : NULL))

#define EINA_ARRAY_ITER_NEXT(array, index, item, iterator)		\
  for (index = 0, iterator = (array)->data; \
       (index < eina_array_count_get(array)) && ((item = *((iterator)++))); \
       ++(index))

// ecore_evas
#define IFC(_ee, _fn)  if (_ee->engine.func->_fn) {_ee->engine.func->_fn
#define IFE            return;}

// debugging
#define __FILE__ ""
#define __LINE__ ""
#define __FUNCTION__ ""

//error checking  ecore_exe.c
#define E_IF_NO_ERRNO(result, foo, ok) \
  while (((ok) = _ecore_exe_check_errno( (result) = (foo), __FILE__, __LINE__)) == -1)   sleep(1); \
  if (ok)

#define E_NO_ERRNO(result, foo, ok) \
  while (((ok) = _ecore_exe_check_errno( (result) = (foo), __FILE__, __LINE__)) == -1)   sleep(1)

#define E_IF_NO_ERRNO_NOLOOP(result, foo, ok) \
  if (((ok) = _ecore_exe_check_errno( (result) = (foo), __FILE__, __LINE__)))


#define IFC(_ee, _fn)  if (_ee->engine.func->_fn) {_ee->engine.func->_fn
#define IFE            return;}

// emotion VLC
#define ASSERT_EV(ev) MACROSTATEMENT
#define ASSERT_EV_M(ev) MACROSTATEMENT
#define ASSERT_EV_MP(ev) MACROSTATEMENT
#define ASSERT_EV_VLC(ev) MACROSTATEMENT
#define CATCH(ev) MACROSTATEMENT

// PROTO/shellelementary
#define SH_API

// e_config_data.h
#define E_CONFIG_DD_NEW(str, typ) \
   e_config_descriptor_new(str, sizeof(typ))
#define E_CONFIG_DD_FREE(eed) if (eed) { eet_data_descriptor_free((eed)); (eed) = NULL; }
#define E_CONFIG_VAL(edd, type, member, dtype) EET_DATA_DESCRIPTOR_ADD_BASIC(edd, type, #member, member, dtype)
#define E_CONFIG_SUB(edd, type, member, eddtype) EET_DATA_DESCRIPTOR_ADD_SUB(edd, type, #member, member, eddtype)
#define E_CONFIG_LIST(edd, type, member, eddtype) EET_DATA_DESCRIPTOR_ADD_LIST(edd, type, #member, member, eddtype)
#define E_CONFIG_HASH(edd, type, member, eddtype) EET_DATA_DESCRIPTOR_ADD_HASH(edd, type, #member, member, eddtype)

// exalt
//

#define EXALT_FCT_NAME exalt_wireless

#define EXALT_GET(attribut,type)                        \
    EXALT_GET1(EXALT_FCT_NAME,attribut,type)
/// @cond
#define EXALT_GET1(exalt_file_name,attribut,type)       \
    EXALT_GET2(exalt_file_name,attribut,type)

#define EXALT_GET2(exalt_file_name,attribut,type)       \
    type exalt_file_name##_##attribut##_get(            \
            EXALT_STRUCT_TYPE *s)                       \
    {                                                   \
        EXALT_ASSERT_RETURN(s!=NULL);                   \
        return s->attribut;                             \
    }

#define EXALT_SET(attribut,type)                        \
    EXALT_SET1(EXALT_FCT_NAME, attribut, type)

/// @cond
#define EXALT_SET1(exalt_file_name,attribut,type)       \
    EXALT_SET2(exalt_file_name,attribut,type)

#define EXALT_SET2(exalt_file_name,attribut,type)       \
    void exalt_file_name##_##attribut##_set(            \
            EXALT_STRUCT_TYPE *s,                       \
            type attribut)                              \
    {                                                   \
        EXALT_ASSERT_RETURN_VOID(s!=NULL);              \
        s->attribut=attribut;                           \
    }

#define EXALT_STRING_SET(attribut)

/// @cond
#define EXALT_STRING_SET1(exalt_file_name,attribut)     \
    EXALT_STRING_SET2(exalt_file_name,attribut)

#define EXALT_STRING_SET2(exalt_file_name,attribut)     \
    void exalt_file_name##_##attribut##_set(            \
            EXALT_STRUCT_TYPE *s,                       \
            const char* attribut)                       \
    {                                                   \
        EXALT_ASSERT_RETURN_VOID(s!=NULL);              \
        EXALT_FREE(s->attribut);                        \
        if(attribut!=NULL)                              \
            s->attribut = strdup(attribut);             \
        else                                            \
            s->attribut=NULL;                           \
    }


#define EXALT_IS(attribut,type)                         \
    EXALT_IS1(EXALT_FCT_NAME,attribut,type)
/// @cond
#define EXALT_IS1(exalt_file_name,attribut,type)        \
    EXALT_IS2(exalt_file_name,attribut,type)

#define EXALT_IS2(exalt_file_name,attribut,type)        \
    type exalt_file_name##_##attribut##_is(             \
            EXALT_STRUCT_TYPE *s)                       \
    {                                                   \
        EXALT_ASSERT_RETURN(s!=NULL);                   \
        return s->attribut;                             \
    }

// e_dbus
#define DBUS_TYPE_ARRAY_AS_STRING ""
#define DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING ""
#define DBUS_TYPE_STRING_AS_STRING ""
#define DBUS_TYPE_STRING_AS_STRING ""
#define DBUS_DICT_ENTRY_END_CHAR_AS_STRING ""



// embryo
#define SWITCH(x) switch (x) {
#define SWITCHEND }
#define CASE(x) case x:
#define BREAK break


// system headers
#define __attribute_pure__
#define __THROW
#define __nonnull(x)
#define __const const
#define __attribute_malloc__
#define __attribute_warn_unused_result__
#define __wur
#define __restrict
#define __extern_always_inline
#define __extern_inline
#define __BEGIN_DECLS
#define __BEGIN_NAMESPACE_STD
