
#ifndef __EWL_DEBUG_H__
#define __EWL_DEBUG_H__

#define DENTER_FUNCTION \
{ \
	if (ewl_config.debug.enable && ewl_config.debug.level >= 15) \
	  { \
		fprintf(stderr, " --> %s:%i\tEntering %s();\n", \
			__FILE__, __LINE__, __FUNCTION__); \
	  } \
}

#define DLEAVE_FUNCTION \
{ \
	if (ewl_config.debug.enable && ewl_config.debug.level >= 15) \
	  { \
		fprintf(stderr, "<--  %s:%i\tLeaving  %s();\n", \
			__FILE__, __LINE__, __FUNCTION__); \
	  } \
}

#define DRETURN \
{ \
	if (ewl_config.debug.enable && ewl_config.debug.level >= 15) \
	  { \
		fprintf(stderr, "<--  %s:%i\tReturn in %s();\n", \
			__FILE__, __LINE__, __FUNCTION__); \
	  } \
	return; \
}

#define DRETURN_PTR(ptr) \
{ \
	if (ewl_config.debug.enable && ewl_config.debug.level >= 15) \
	  { \
		fprintf(stderr, "<--  %s:%i\tReturning %p in %s();\n", \
			__FILE__, __LINE__, ptr, __FUNCTION__); \
	  } \
	return ptr; \
}

#define DRETURN_FLOAT(num) \
{ \
	if (ewl_config.debug.enable && ewl_config.debug.level >= 15) \
	  { \
		fprintf(stderr, "<--  %s:%i\tReturning %f in %s();\n", \
			__FILE__, __LINE__, (float) num, __FUNCTION__); \
	  } \
	return num; \
}

#define DRETURN_INT(num) \
{ \
	if (ewl_config.debug.enable && ewl_config.debug.level >= 15) \
	  { \
		fprintf(stderr, "<--  %s:%i\tReturning %i in %s();\n", \
			__FILE__, __LINE__, (int) num, __FUNCTION__); \
	  } \
	return num; \
}

#define D(lvl, str) \
{ \
	if (lvl <= ewl_config.debug.enable && ewl_config.debug.level) \
	  { \
		printf("<--> "); \
		printf str; \
		printf("\n"); \
	  } \
}

#define DERROR(str) \
{ \
	fprintf(stderr, (str)); \
}

#define DWARNING(str) \
{ \
	fprintf(stderr, (str)); \
}

#define DCHECK_PARAM_PTR(str, ptr) \
{ \
	if (!ptr) \
	  { \
		fprintf(stderr, "***** Ewl Developer Warning ***** :\n" \
				"\tThis program is calling:\n\n" \
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
		fprintf(stderr, "***** Ewl Developer Warning ***** :\n" \
				"\tThis program is calling:\n\n" \
				"\t%s();\n\n" \
				"\tWith the parameter:\n\n" \
				"\t%s\n\n" \
				"\tbeing NULL. Please fix your program.\n", \
				__FUNCTION__, str); \
		return ret; \
	  } \
}

#endif /* __EWL_DEBUG_H__ */
