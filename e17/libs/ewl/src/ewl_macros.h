
/*\
|*|
|*| Convension macros used to provide some debug information.
|*|
\*/

#ifndef __EWL_MACROS_H__
#define __EWL_MACROS_H__

#undef NEW
#define NEW(dat, num) malloc(sizeof(dat) * (num));

#undef REALLOC
#define REALLOC(dat, type, num) \
{ \
	if (dat) \
	  { \
		dat = realloc(dat, sizeof(type) * num); \
	  } \
}

#undef FREE
#define FREE(dat) \
{ \
	D(12, "About to FREE %i bytes", sizeof(dat)); \
	free(dat); dat = NULL; \
}


#undef IF_FREE
#define IF_FREE(dat) \
{ \
	D(12, "About to IF_FREE %i bytes", sizeof(dat)); \
	if (dat) free(dat); dat = NULL; \
}

#endif				/* __EWL_MACROS_H__ */
