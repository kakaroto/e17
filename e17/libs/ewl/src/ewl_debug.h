
/*\
|*|
|*| Macros used to print debugging information.
|*|
\*/

#ifndef __EWL_DEBUG_H__
#define __EWL_DEBUG_H__

#ifdef _DEBUG

#define DENTER_FUNCTION \
{ \
	extern Ewl_Options ewl_options; \
	if (ewl_options.debug_level >= 15) \
	  { \
		fprintf(stderr, " --> %s:%i\tEntering %s();\n", \
			__FILE__, __LINE__, __FUNCTION__); \
	  } \
}

#define DLEAVE_FUNCTION \
{ \
	extern Ewl_Options ewl_options; \
	if (ewl_options.debug_level >= 15) \
	  { \
		fprintf(stderr, "<--  %s:%i\tLeaving  %s();\n", \
			__FILE__, __LINE__, __FUNCTION__); \
	  } \
}

#define DRETURN \
{ \
	extern Ewl_Options ewl_options; \
	if (ewl_options.debug_level >= 15) \
	  { \
		fprintf(stderr, "<--  %s:%i\tReturn in %s();\n", \
			__FILE__, __LINE__, __FUNCTION__); \
	  } \
	return; \
}

#define DRETURN_PTR(ptr) \
{ \
	extern Ewl_Options ewl_options; \
	if (ewl_options.debug_level >= 15) \
	  { \
		fprintf(stderr, "<--  %s:%i\tReturning ptr %p in %s();\n", \
			__FILE__, __LINE__, ptr, __FUNCTION__); \
	  } \
	return ptr; \
}

#define DRETURN_FLOAT(num) \
{ \
	extern Ewl_Options ewl_options; \
	if (ewl_options.debug_level >= 15) \
	  { \
		fprintf(stderr, "<--  %s:%i\tReturning float %f in %s();\n", \
			__FILE__, __LINE__, (float) num, __FUNCTION__); \
	  } \
	return num; \
}

#define DRETURN_INT(num) \
{ \
	extern Ewl_Options ewl_options; \
	if (ewl_options.debug_level >= 15) \
	  { \
		fprintf(stderr, "<--  %s:%i\tReturning int %i in %s();\n", \
			__FILE__, __LINE__, (int) num, __FUNCTION__); \
	  } \
	return num; \
}

#define D(lvl, str) \
{ \
	extern Ewl_Options ewl_options; \
	if (lvl <= ewl_options.debug_level) \
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


#else

#define DENTER_FUNCTION
#define DLEAVE_FUNCTION
#define DRETURN return;
#define DRETURN_PTR(ptr) return ptr;
#define DRETURN_INT(num) return num;
#define D(lvl, str)
#define DERROR(str)
#define DWARNING(str)
#define DSUMMARY

#endif /* _DEBUG */

#define DCHECK_PARAM_PTR(str, ptr) \
{ \
	if (!ptr) \
	  { \
		fprintf(stderr, "NULL param %s in %s\n", str, __FUNCTION__); \
		return; \
	  } \
}

#define DCHECK_PARAM_PTR_RET(str, ptr, ret) \
{ \
	if (!ptr) \
	  { \
		fprintf(stderr, "NULL param %s in %s.. returning %p\n", \
				str, __FUNCTION__, (void *) ret); \
		return ret; \
	  } \
}

#define DCHECK_PARAM_INT_RET(str, ptr, ret) \
{ \
        if (!ptr) \
          { \
                fprintf(stderr, "NULL param %s in %s.. returning %i\n", \
                                str, __FUNCTION__, (int) ret); \
                return ret; \
          } \
}

#endif				/* __EWL_DEBUG_H__ */
