#include "option.h"


void ewl_option_add(char *short_string, char *long_string, char *desc, 
                    char (*cb)(int argc, char *argv[]))
{
	char      *temp = malloc(8);
	EwlOption *o = NULL;
	FUNC_BGN("ewl_option_add");

	o = malloc(sizeof(EwlOption));
	if (!o)	{
		ewl_debug("ewl_option_add", EWL_NULL_ERROR,"o");
		FUNC_END("ewl_option_add");
		return;
	}

	strncpy(temp,short_string,7);
	o->short_string = temp;
	temp = malloc(64);
	strncpy(temp,long_string,63);
	o->long_string = temp;
	temp = malloc(256);
	strncpy(temp,desc,255);
	o->desc = temp;
	o->cb = cb;

	_Ewl_Options = (EwlOption*) ewl_ll_insert((EwlLL*) _Ewl_Options,
	                                          (EwlLL*) o);
	FUNC_END("ewl_option_add");
	return;
}

void ewl_option_parse(int *argc, char ***argv)
{
	/*int    i = 0,
           ac = 0;
	char **av = NULL;
	
	for (i=0; i<*argc; i++)	{
		if ((*argv)[i])
	}*/
	return;
}

static char _cb_print_eol(EwlLL *node, EwlData *data)
{
	EwlOption *o = (EwlOption*) node;
	fprintf(stderr,"-%s, --%s: %s\n",o->short_string,o->long_string,o->desc);
	return 1;
}

void ewl_option_list()
{
	ewl_ll_foreach((EwlLL*)_Ewl_Options, _cb_print_eol, NULL);
	return;
}
