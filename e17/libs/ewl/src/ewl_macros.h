
#ifndef __EWL_MACROS_H
#define __EWL_MACROS_H

#define DPRINT(lvl, format, args...) { \
	extern Ewl_Options ewl_options; \
	if (ewl_options.debug_level >= lvl) { \
		fprintf(stderr, "--> "); \
		fprintf(stderr, format, args); \
		fprintf(stderr, "\n"); \
	} \
}

#define DERROR(format, args...) { \
		fprintf(stderr, "***** EWL Internal Error ***** :\n" \
						"\tThis program encountered a error in:\n\n" \
						"\t%s();\n\n" \
						"\tLeaving this error msg:\n\n", __FUNCTION__); \
		fprintf(stderr, format, args); \
}

#define NEW(dat, num) malloc(sizeof(dat) * (num))


#undef FREE
#define FREE(dat) \
	DPRINT(12, "About to FREE %i bytes", sizeof(dat)); \
	free(dat); dat = NULL;
	

#undef IF_FREE
#define IF_FREE(dat) \
	DPRINT(12, "About to IF_FREE %i bytes", sizeof(dat)); \
	if (dat) free(dat); dat = NULL;

#endif
