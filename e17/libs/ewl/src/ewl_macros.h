
#ifndef __EWL_MACROS_H
#define __EWL_MACROS_H

#define DPRINT(msg, lvl) { \
	if (lvl) \
		fprintf(stderr, "%s\n"); \
}

#define DERROR(format, args...) { \
		fprintf(stderr, "***** EWL Internal Error ***** :\n" \
						"\tThis program encountered a error in:\n\n" \
						"\t%s();\n\n" \
						"\tLeaving this error msg:\n\n", __FUNCTION__); \
		fprintf(stderr, format, args); \
}

#define NEW(dat, num) malloc(sizeof(dat) * (num))

#endif
