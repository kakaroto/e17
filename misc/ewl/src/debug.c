#include "debug.h"

void ewl_debug(char *func, EwlDebugType db, char *data)
{
	if (!_Ewl_Debug)
		return;

	switch (db)	{
		case EWL_NO_ERROR:
			break;
		case EWL_FUNC_BGN:
			fprintf(stderr,"EWL_DEBUG: %s BEGIN\n", func);
			break;
		case EWL_FUNC_END:
			fprintf(stderr,"EWL_DEBUG: %s END\n", func);
			break;
		case EWL_NULL_ERROR:
			fprintf(stderr,"EWL_DEBUG: %s==NULL in %s()\n",data,func);
			break;
		case EWL_OUT_OF_BOUNDS_ERROR:
			fprintf(stderr,"EWL_DEBUG: %s out of bounds in %s()\n",data,func);
			break;
		case EWL_NULL_WIDGET_ERROR:
			fprintf(stderr,"EWL_DEBUG: %s==NULL in %s()\n",data,func);
			break;
		case EWL_FILE_NOT_FOUND_ERROR:
			fprintf(stderr,"EWL_DEBUG: %s not found in %s()\n",data,func);
			break;
		case EWL_WIDGET_NOT_FOUND_ERROR:
			fprintf(stderr,"EWL_DEBUG: %s not found in %s()\n",data,func);
			break;
		case EWL_GRAB_ERROR:
			fprintf(stderr,"EWL_ERROR: Couldn't Grab EwlWidget %s in %s()\n", data, func);
			break;
		case EWL_GENERIC_ERROR:
			fprintf(stderr,"%s(): %s\n", func, data);
			break;
		default:
			fprintf(stderr,"EWL_DEBUG: noerror\n");
	};

	return;
}

void ewl_fatal(char *str)
{
	fprintf(stderr,"EWL_FATAL: %s\n", str?str:"");
	exit(-1);
}

void ewl_debug_enable()
{
	_Ewl_Debug = 1;
	fprintf(stderr,"EWL_DEBUG: debugging output enabled\n");
	return;
}

void ewl_debug_disable()
{
	_Ewl_Debug = 0;
	fprintf(stderr,"EWL_DEBUG: debugging output disabled\n");
	return;
}

char ewl_debug_is_active()
{
	return _Ewl_Debug;
}

void ewl_debug_func_enable()
{
	_Ewl_Func_Debug = 1;
	fprintf(stderr,"EWL_DEBUG: function debugging output enabled\n");
	return;
}

void ewl_debug_func_disable()
{
	_Ewl_Func_Debug = 0;
	fprintf(stderr,"EWL_DEBUG: function debugging output disabled\n");
	return;
}

char ewl_debug_func_is_active()
{
	return _Ewl_Func_Debug;
}
