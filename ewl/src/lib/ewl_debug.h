/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_DEBUG_H
#define EWL_DEBUG_H

#include "ewl_misc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DLEVEL_UNSTABLE 0
#define DLEVEL_TESTING 10
#define DLEVEL_STABLE 20

void ewl_print_warning(void);
void ewl_segv(void);
void ewl_backtrace(void);
int  ewl_cast_pointer_to_integer(void *ptr);

/**
 * The Ewl_Config_Cache structure
 */
typedef struct Ewl_Config_Cache Ewl_Config_Cache;

/**
 * @brief Contains cached Ewl configuration data
 */
struct Ewl_Config_Cache
{
        int level;                        /**< Debug level */
        unsigned char enable:1;                /**< Enable debugging */
        unsigned char segv:1;                /**< Segv on D* messages */
        unsigned char backtrace:1;        /**< Print backtrace on D* messages */
        unsigned char evas_render:1;        /**< Debug evas render calls */
        unsigned char gc_reap:1;        /**< Debug garbage collection */

        unsigned char print_signals:1;        /**< Print theme signals */
        unsigned char print_keys:1;        /**< Print theem keys */
};

extern Ewl_Config_Cache ewl_config_cache; /**< system debug data */

#if EWL_ENABLE_DEBUG

#define DEBUGGING(lvl) (ewl_config_cache.enable && (ewl_config_cache.level >= (lvl)))

#define DENTER_FUNCTION(lvl) \
do { \
        if (DEBUGGING(lvl)) \
          { \
                ewl_debug_indent_print(1); \
                fprintf(stderr, "--> %f - %s:%i\tEntering %s();\n", \
                        ecore_time_get(), __FILE__, __LINE__, __func__); \
          } \
} while (0)

#define DLEAVE_FUNCTION(lvl) \
do { \
        if (DEBUGGING(lvl)) \
          { \
                ewl_debug_indent_print(-1); \
                fprintf(stderr, "<--  %f - %s:%i\tLeaving  %s();\n", \
                        ecore_time_get(), __FILE__, __LINE__, __func__); \
          } \
} while (0)

#define DRETURN(lvl) \
do { \
        DLEAVE_FUNCTION(lvl); \
        if (DEBUGGING(lvl)) \
          { \
                ewl_debug_indent_print(0); \
                fprintf(stderr, "<-- %f - %s:%i\tReturn in %s();\n", \
                        ecore_time_get(), __FILE__, __LINE__, __func__); \
          } \
        return; \
} while (0)

#define DRETURN_PTR(ptr, lvl) \
do { \
        DLEAVE_FUNCTION(lvl); \
        if (DEBUGGING(lvl)) \
          { \
                ewl_debug_indent_print(0); \
                fprintf(stderr, "<-- %f - %s:%i\tReturning %p in %s();\n", \
                        ecore_time_get(), __FILE__, __LINE__, (void *) (ptr), __func__); \
          } \
        return ptr; \
} while (0)

#define DRETURN_FLOAT(num, lvl) \
do { \
        DLEAVE_FUNCTION(lvl); \
        if (DEBUGGING(lvl)) \
          { \
                ewl_debug_indent_print(0); \
                fprintf(stderr, "<-- %f - %s:%i\tReturning %f in %s();\n", \
                        ecore_time_get(), __FILE__, __LINE__, (float) (num), __func__); \
          } \
        return num; \
} while (0)

#define DRETURN_INT(num, lvl) \
do { \
        DLEAVE_FUNCTION(lvl); \
        if (DEBUGGING(lvl)) \
          { \
                ewl_debug_indent_print(0); \
                fprintf(stderr, "<-- %f - %s:%i\tReturning %i in %s();\n", \
                        ecore_time_get(), __FILE__, __LINE__, (int) (num), __func__); \
          } \
        return num; \
} while (0)

#define DWARNING(fmt, args...) \
do { \
        ewl_print_warning(); \
        fprintf(stderr, "\tIn function:\n\n" \
                        "\t%s();\n\n", __func__); \
        fprintf(stderr, fmt, ## args); \
        fprintf(stderr, "\n"); \
        ewl_backtrace(); \
        ewl_segv(); \
} while (0)

#define DCHECK_PARAM_PTR(ptr) \
do { \
        if ((ptr) == NULL) \
          { \
                ewl_print_warning(); \
                fprintf(stderr, "\tThis program is calling:\n\n" \
                                "\t%s();\n\n" \
                                "\tWith the parameter:\n\n" \
                                "\t%s\n\n" \
                                "\tbeing NULL. Please fix your program.\n", \
                                __func__, # ptr); \
                ewl_backtrace(); \
                ewl_segv(); \
                return; \
          } \
} while (0)

#define DCHECK_PARAM_PTR_RET(ptr, ret) \
do { \
        if ((ptr) == NULL) \
          { \
                ewl_print_warning(); \
                fprintf(stderr, "\tThis program is calling:\n\n" \
                                "\t%s();\n\n" \
                                "\tWith the parameter:\n\n" \
                                "\t%s\n\n" \
                                "\tbeing NULL. Please fix your program.\n", \
                                __func__, # ptr); \
                ewl_backtrace(); \
                ewl_segv(); \
                return ret; \
          } \
} while (0)

#define DCHECK_TYPE(ptr, type) \
do { \
        if (ptr && !ewl_widget_type_is(EWL_WIDGET(ptr), type)) \
        { \
                ewl_print_warning(); \
                fprintf(stderr, "\tThis program is calling:\n\n" \
                                "\t%s();\n\n" \
                                "\tWith the paramter:\n\n" \
                                "\t%s\n\n" \
                                "\tas the wrong type. (%s) instead of (%s).\n" \
                                "\tPlease fix your program.\n", \
                                __func__, # ptr, \
                                (EWL_WIDGET(ptr)->inheritance ? \
                                        EWL_WIDGET(ptr)->inheritance : \
                                        "NULL") , type); \
                ewl_backtrace(); \
                ewl_segv(); \
                return; \
        } \
} while (0)

#define DCHECK_TYPE_RET(ptr, type, ret) \
do { \
        if (ptr && !ewl_widget_type_is(EWL_WIDGET(ptr), type)) \
        { \
                ewl_print_warning(); \
                fprintf(stderr, "\tThis program is calling:\n\n" \
                                "\t%s();\n\n" \
                                "\tWith the paramter:\n\n" \
                                "\t%s\n\n" \
                                "\tas the wrong type. (%s) instead of (%s).\n" \
                                "\tPlease fix your program.\n", \
                                __func__, # ptr, \
                                (EWL_WIDGET(ptr)->inheritance ? \
                                        EWL_WIDGET(ptr)->inheritance : \
                                        "NULL") , type); \
                ewl_backtrace(); \
                ewl_segv(); \
                return ret; \
        } \
} while (0)

#else

#define DENTER_FUNCTION(lvl) {}
#define DLEAVE_FUNCTION(lvl) {}
#define DRETURN(lvl) return
#define DRETURN_PTR(ptr, lvl) return (ptr)
#define DRETURN_FLOAT(num, lvl) return num
#define DRETURN_INT(num, lvl) return num
#define DWARNING(fmt, args...) {}
#define DCHECK_PARAM_PTR(ptr) \
do { \
        if (!(ptr)) { \
                return; \
        } \
} while (0)
#define DCHECK_PARAM_PTR_RET(ptr, ret) \
do { \
        if (!(ptr)) { \
                return ret; \
        } \
} while (0)
#define DCHECK_TYPE(ptr, type) {}
#define DCHECK_TYPE_RET(ptr, type, ret) {}
#endif

#define DERROR(fmt) \
do { \
        ewl_print_warning(); \
        fprintf(stderr, "\tIn function:\n\n" \
                        "\t%s();\n\n", __func__); \
        fprintf(stderr, fmt); \
} while (0)

#endif

