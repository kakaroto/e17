
#ifndef __EWL_DEBUG_H__
#define __EWL_DEBUG_H__

#define DLEVEL_UNSTABLE 1
#define DLEVEL_TESTING 10
#define DLEVEL_STABLE 20

inline void     ewl_print_warning();

#define DENTER_FUNCTION(lvl) \
{ \
	if (ewl_config.debug.enable && (ewl_config.debug.level >= lvl)) \
	  { \
		fprintf(stderr, " --> %s:%i\tEntering %s();\n", \
			__FILE__, __LINE__, __FUNCTION__); \
	  } \
}

#define DLEAVE_FUNCTION(lvl) \
{ \
	if (ewl_config.debug.enable && ewl_config.debug.level >= lvl) \
	  { \
		fprintf(stderr, "<--  %s:%i\tLeaving  %s();\n", \
			__FILE__, __LINE__, __FUNCTION__); \
	  } \
}

#define DRETURN(lvl) \
{ \
	if (ewl_config.debug.enable && ewl_config.debug.level >= lvl) \
	  { \
		fprintf(stderr, "<--  %s:%i\tReturn in %s();\n", \
			__FILE__, __LINE__, __FUNCTION__); \
	  } \
	return; \
}

#define DRETURN_PTR(ptr, lvl) \
{ \
	if (ewl_config.debug.enable && ewl_config.debug.level >= lvl) \
	  { \
		fprintf(stderr, "<--  %s:%i\tReturning %p in %s();\n", \
			__FILE__, __LINE__, ptr, __FUNCTION__); \
	  } \
	return ptr; \
}

#define DRETURN_FLOAT(num, lvl) \
{ \
	if (ewl_config.debug.enable && ewl_config.debug.level >= lvl) \
	  { \
		fprintf(stderr, "<--  %s:%i\tReturning %f in %s();\n", \
			__FILE__, __LINE__, (float) num, __FUNCTION__); \
	  } \
	return num; \
}

#define DRETURN_INT(num, lvl) \
{ \
	if (ewl_config.debug.enable && ewl_config.debug.level >= lvl) \
	  { \
		fprintf(stderr, "<--  %s:%i\tReturning %i in %s();\n", \
			__FILE__, __LINE__, (int) num, __FUNCTION__); \
	  } \
	return num; \
}

#define D(lvl, fmt) \
{ \
	if (ewl_config.debug.enable && ewl_config.debug.level >= lvl) \
	  { \
		fprintf(stderr, "<--> "); \
		fprintf(stderr, fmt); \
		fprintf(stderr, "\n"); \
	  } \
}

#define DERROR(fmt) \
{ \
	ewl_print_warning(); \
	fprintf(stderr, "\tIn function:\n\n" \
			"\t%s();\n\n", __FUNCTION__); \
	fprintf(stderr, fmt); \
}

#define DWARNING(fmt) \
{ \
	ewl_print_warning(); \
	fprintf(stderr, "\tIn function:\n\n" \
			"\t%s();\n\n", __FUNCTION__); \
	fprintf(stderr, fmt); \
}

#define DCHECK_PARAM_PTR(str, ptr) \
{ \
	if (!ptr) \
	  { \
		ewl_print_warning(); \
		fprintf(stderr, "\tThis program is calling:\n\n" \
				"\t%s();\n\n" \
				"\tWith the parameter:\n\n" \
				"\t%s\n\n" \
				"\tbeing NULL. Please fix your program.\n", \
				__FUNCTION__, str); \
		return; \
	  } \
}

#define DCHECK_PARAM_PTR_RET(str, ptr, ret) \
{ \
	if (!ptr) \
	  { \
		ewl_print_warning(); \
		fprintf(stderr, "\tThis program is calling:\n\n" \
				"\t%s();\n\n" \
				"\tWith the parameter:\n\n" \
				"\t%s\n\n" \
				"\tbeing NULL. Please fix your program.\n", \
				__FUNCTION__, str); \
		return ret; \
	  } \
}

#endif				/* __EWL_DEBUG_H__ */
