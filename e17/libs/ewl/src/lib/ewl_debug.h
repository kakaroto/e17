#ifndef EWL_DEBUG_H
#define EWL_DEBUG_H

#include "ewl_misc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DLEVEL_UNSTABLE 1
#define DLEVEL_TESTING 10
#define DLEVEL_STABLE 20

void ewl_print_warning(void);
void ewl_segv(void);
void ewl_backtrace(void);

// #define DEBUG 1

#ifdef DEBUG

#define DENTER_FUNCTION(lvl) \
{ \
	if (ewl_config.debug.enable && (ewl_config.debug.level >= (lvl))) \
	  { \
		char *indent = ewl_debug_get_indent(); \
		ewl_config.debug.indent_lvl ++; \
		fprintf(stderr, "%s--> %s:%i\tEntering %s();\n", \
			indent, __FILE__, __LINE__, __func__); \
		FREE(indent); \
	  } \
}

#define DLEAVE_FUNCTION(lvl) \
{ \
	if (ewl_config.debug.enable && (ewl_config.debug.level >= (lvl))) \
	  { \
		char *indent; \
	        ewl_config.debug.indent_lvl --; \
		indent = ewl_debug_get_indent(); \
		fprintf(stderr, "%s<--  %s:%i\tLeaving  %s();\n", \
			indent, __FILE__, __LINE__, __func__); \
		FREE(indent); \
	  } \
}

#define DRETURN(lvl) \
{ \
	DLEAVE_FUNCTION(lvl); \
	if (ewl_config.debug.enable && (ewl_config.debug.level >= (lvl))) \
	  { \
		char *indent; \
		indent = ewl_debug_get_indent(); \
		fprintf(stderr, "%s<--  %s:%i\tReturn in %s();\n", \
			indent, __FILE__, __LINE__, __func__); \
		FREE(indent); \
	  } \
	return; \
}

#define DRETURN_PTR(ptr, lvl) \
{ \
	DLEAVE_FUNCTION(lvl); \
	if (ewl_config.debug.enable && (ewl_config.debug.level >= (lvl))) \
	  { \
		char *indent; \
		indent = ewl_debug_get_indent(); \
		fprintf(stderr, "%s<--  %s:%i\tReturning %p in %s();\n", \
			indent, __FILE__, __LINE__, (void *) (ptr), __func__); \
		FREE(indent); \
	  } \
	return (void *)(ptr); \
}

#define DRETURN_FLOAT(num, lvl) \
{ \
	DLEAVE_FUNCTION(lvl); \
	if (ewl_config.debug.enable && (ewl_config.debug.level >= (lvl))) \
	  { \
		char *indent; \
		indent = ewl_debug_get_indent(); \
		fprintf(stderr, "%s<--  %s:%i\tReturning %f in %s();\n", \
			indent, __FILE__, __LINE__, (float) (num), __func__); \
		FREE(indent); \
	  } \
	return num; \
}

#define DRETURN_INT(num, lvl) \
{ \
	DLEAVE_FUNCTION(lvl); \
	if (ewl_config.debug.enable && (ewl_config.debug.level >= (lvl))) \
	  { \
		char *indent; \
		indent = ewl_debug_get_indent(); \
		fprintf(stderr, "%s<--  %s:%i\tReturning %i in %s();\n", \
			indent, __FILE__, __LINE__, (int) (num), __func__); \
		FREE(indent); \
	  } \
	return num; \
}

#define DWARNING(fmt) \
{ \
	ewl_print_warning(); \
	fprintf(stderr, "\tIn function:\n\n" \
			"\t%s();\n\n", __func__); \
	fprintf(stderr, fmt); \
	ewl_backtrace(); \
	ewl_segv(); \
}

#define DCHECK_PARAM_PTR(str, ptr) \
{ \
	if (!(ptr)) \
	  { \
		ewl_print_warning(); \
		fprintf(stderr, "\tThis program is calling:\n\n" \
				"\t%s();\n\n" \
				"\tWith the parameter:\n\n" \
				"\t%s\n\n" \
				"\tbeing NULL. Please fix your program.\n", \
				__func__, str); \
		ewl_backtrace(); \
		ewl_segv(); \
		return; \
	  } \
}

#define DCHECK_PARAM_PTR_RET(str, ptr, ret) \
{ \
	if (!(ptr)) \
	  { \
		ewl_print_warning(); \
		fprintf(stderr, "\tThis program is calling:\n\n" \
				"\t%s();\n\n" \
				"\tWith the parameter:\n\n" \
				"\t%s\n\n" \
				"\tbeing NULL. Please fix your program.\n", \
				__func__, str); \
		ewl_backtrace(); \
		ewl_segv(); \
		return ret; \
	  } \
}

#define DCHECK_TYPE(str, ptr, type) \
{ \
	if (!ewl_widget_type_is(EWL_WIDGET(ptr), type)) \
	{ \
		ewl_print_warning(); \
		fprintf(stderr, "\tThis program is calling:\n\n" \
				"\t%s();\n\n" \
				"\tWith the paramter:\n\n" \
				"\t%s\n\n" \
				"\tas the wrong type. (%s) instead of (%s).\n" \
				"\tPlease fix your program.\n", \
				__func__, str, EWL_WIDGET(ptr)->inheritance, type); \
		ewl_backtrace(); \
		ewl_segv(); \
	} \
}

#define DCHECK_TYPE_RET(str, ptr, type, ret) \
{ \
	if (!ewl_widget_type_is(EWL_WIDGET(ptr), type)) \
	{ \
		ewl_print_warning(); \
		fprintf(stderr, "\tThis program is calling:\n\n" \
				"\t%s();\n\n" \
				"\tWith the paramter:\n\n" \
				"\t%s\n\n" \
				"\tas the wrong type. (%s) instead of (%s).\n" \
				"\tPlease fix your program.\n", \
				__func__, str, EWL_WIDGET(ptr)->inheritance, type); \
		ewl_backtrace(); \
		ewl_segv(); \
		return ret; \
	} \
}

#else

#define DENTER_FUNCTION(lvl) {}
#define DLEAVE_FUNCTION(lvl) {}
#define DRETURN(lvl) return
#define DRETURN_PTR(ptr, lvl) return (void *)(ptr)
#define DRETURN_FLOAT(num, lvl) return num
#define DRETURN_INT(num, lvl) return num
#define DWARNING(fmt) {}
#define DCHECK_PARAM_PTR(str, ptr) \
{ \
	if (!(ptr)) { \
		return; \
	} \
}
#define DCHECK_PARAM_PTR_RET(str, ptr, ret) \
{ \
	if (!(ptr)) { \
		return ret; \
	} \
}
#define DCHECK_TYPE(str, ptr, type) {}
#define DCHECK_TYPE_RET(str, ptr, type, ret) {}
#endif

#define DERROR(fmt) \
{ \
	ewl_print_warning(); \
	fprintf(stderr, "\tIn function:\n\n" \
			"\t%s();\n\n", __func__); \
	fprintf(stderr, fmt); \
}

#endif

