#ifndef CALLBACK_H
#define CALLBACK_H

typedef struct Ewler_Callback Ewler_Callback;

struct Ewler_Callback {
	char *handler;
	int callback;
};

void ewler_callbacks_init( void );
Ecore_List *ewler_callbacks_create( Ewl_Widget *w );
Ecore_List *ewler_callbacks_get( Ewl_Widget *w );
void ewler_callbacks_destroy( Ewl_Widget *w );
int ewler_callback_add( Ewl_Widget *w, char *handler, int callback );
int ewler_callback_del( Ewl_Widget *w, char *handler, int callback );
int ewler_callback_replace( Ewl_Widget *w, Ewler_Callback *cb,
														char *handler, int callback );
Ewler_Callback *ewler_callback_get( Ewl_Widget *w,
																		char *handler, int callback );

#endif
