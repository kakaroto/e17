#include "util.h"

char *e_string_dup(char *str)
{
	char *newstr;
	FUNC_BGN("e_string_dup");

	if (!str)	{
		ewl_debug("e_string_dup",EWL_NULL_STRING_ERROR,"str");
		FUNC_END("e_string_dup");
		return NULL;
	}
	if (!strlen(str))	{
		FUNC_END("e_string_dup");
		return NULL;
	}

	newstr = malloc(strlen(str)+1);
	if (!newstr)	{
		FUNC_END("e_string_dup");
		return NULL;
	}
	sprintf(newstr,"%s",str);
	FUNC_END("e_string_dup");
	return newstr;
}

EwlBool e_check_bool_string(char *str)
{
	EwlBool r = FALSE;
	FUNC_BGN("e_check_bool_string");
	if (!str)	{
		ewl_debug("e_check_bool_string", EWL_NULL_ERROR, "str");
	} else if (!strncasecmp(str,"yes", 3)||!strncasecmp(str,"true", 4)||
	           !strncasecmp(str,"1", 1)||!strncasecmp(str,"active", 6)) {
		r = TRUE;
	}
	FUNC_END("e_check_bool_string");
	return r;
}
