/* internal debugging routines for ewl */
#ifndef _EWLDEBUG_H_
#define _EWLDEBUG_H_ 1

#include <stdio.h>

static unsigned char _Ewl_Debug = 0;
static unsigned char _Ewl_Func_Debug = 0;

typedef enum _EwlDebugType
{
	EWL_NO_ERROR,
	EWL_GENERIC_ERROR,
	EWL_FUNC_BGN,
	EWL_FUNC_END,
	EWL_NULL_ERROR,
	EWL_NULL_STRING_ERROR,
	EWL_NULL_WIDGET_ERROR,
	EWL_OUT_OF_BOUNDS_ERROR,
	EWL_CALLBACK_NOT_FOUND_ERROR,
	EWL_FILE_NOT_FOUND_ERROR,
	EWL_WIDGET_NOT_FOUND_ERROR,
	EWL_GRAB_ERROR
} EwlDebugType;

#define FUNC_BGN(a) if (ewl_debug_func_is_active()) \
                    ewl_debug(a,EWL_FUNC_BGN, NULL)
#define FUNC_END(a) if (ewl_debug_func_is_active()) \
                    ewl_debug(a,EWL_FUNC_END, NULL)

void ewl_debug(char *func, EwlDebugType db, char *data);
void ewl_fatal(char *str);
void ewl_debug_enable();
void ewl_debug_disable();
char ewl_debug_is_active();

void ewl_debug_func_enable();
void ewl_debug_func_disable();
char ewl_debug_func_is_active();

/* make -pedantic shut up about my static var */
static void __depricated_die_pedantic_die()
{
	if (_Ewl_Debug) fprintf(stderr,"DON'T CALL THIS FUNCTION!!\n");
	if (_Ewl_Func_Debug) fprintf(stderr,"DON'T CALL THIS FUNCTION!!\n");
	if (0) __depricated_die_pedantic_die();
	return;
}

#endif /* _EWLDEBUG_H_ */
