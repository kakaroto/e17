#ifndef CALLBACK_H
#define CALLBACK_H

typedef struct Ewler_Callback Ewler_Callback;

struct Ewler_Callback {
	char *handler;
	int callback;
};

void ewler_callbacks_init( void );
Ecore_List *ewler_callbacks_create( Ecore_List *w );
Ecore_List *ewler_callbacks_get( Ecore_List *w );
void ewler_callbacks_destroy( Ecore_List *w );
int ewler_callback_add( Ecore_List *w, char *handler, int callback );

#endif
