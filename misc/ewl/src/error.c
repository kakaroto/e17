
#include "error.h"

void ewl_fatal(char *s)
{
	fprintf(stderr,"EWL FATAL ERROR: %s\n", s);
	exit(-1);
}
