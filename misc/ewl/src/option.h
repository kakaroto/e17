#ifndef _EWL_OPTION_H_
#define _EWL_OPTION_H_ 1

#include "includes.h"
#include "ll.h"

typedef struct _EwlOption EwlOption;
struct _EwlOption	{
	EwlLL           ll;
	char            is_flag;
	char           *short_string;
	char           *long_string;
	char           *desc;
	char           (*cb)(int argc, char *argv[]);
};

static EwlOption                                 *_Ewl_Options = NULL;

void ewl_option_add(char *short_string, char *long_string, char *desc, 
                    char (*cb)(int argc, char *argv[]));
void ewl_option_parse(int *argc, char ***argv);
void ewl_option_list();

/* make -pedantic shut up about my static global var */
static void __depricated_die_pedantic_die_Ewl_Options()
{
	if (_Ewl_Options) fprintf(stderr,"DON'T CALL THIS FUNCTION!!\n");
	if (0) __depricated_die_pedantic_die_Ewl_Options();
	return;
}

#endif /* _EWL_OPTION_ */
