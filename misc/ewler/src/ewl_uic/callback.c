#include <Ewl.h>

#include "callback.h"

Ecore_Hash *callbacks;

static char *callback_strings[] =
{
	"EWL_CALLBACK_EXPOSE", 
	"EWL_CALLBACK_REALIZE", 
	"EWL_CALLBACK_UNREALIZE", 
	"EWL_CALLBACK_SHOW", 
	"EWL_CALLBACK_HIDE", 
	"EWL_CALLBACK_DESTROY", 
	"EWL_CALLBACK_DELETE_WINDOW", 
	"EWL_CALLBACK_CONFIGURE", 
	"EWL_CALLBACK_REPARENT", 
	"EWL_CALLBACK_KEY_DOWN", 
	"EWL_CALLBACK_KEY_UP", 
	"EWL_CALLBACK_MOUSE_DOWN", 
	"EWL_CALLBACK_MOUSE_UP", 
	"EWL_CALLBACK_MOUSE_MOVE", 
	"EWL_CALLBACK_MOUSE_WHEEL", 
	"EWL_CALLBACK_FOCUS_IN", 
	"EWL_CALLBACK_FOCUS_OUT", 
	"EWL_CALLBACK_SELECT", 
	"EWL_CALLBACK_DESELECT", 
	"EWL_CALLBACK_CLICKED", 
	"EWL_CALLBACK_DOUBLE_CLICKED", 
	"EWL_CALLBACK_HILITED", 
	"EWL_CALLBACK_VALUE_CHANGED", 
	"EWL_CALLBACK_STATE_CHANGED", 
	"EWL_CALLBACK_APPEARANCE_CHANGED", 
	"EWL_CALLBACK_WIDGET_ENABLE", 
	"EWL_CALLBACK_WIDGET_DISABLE", 
	"EWL_CALLBACK_PASTE", 
	NULL
};

static int callback_values[] =
{
	EWL_CALLBACK_EXPOSE, 
	EWL_CALLBACK_REALIZE, 
	EWL_CALLBACK_UNREALIZE, 
	EWL_CALLBACK_SHOW, 
	EWL_CALLBACK_HIDE, 
	EWL_CALLBACK_DESTROY, 
	EWL_CALLBACK_DELETE_WINDOW, 
	EWL_CALLBACK_CONFIGURE, 
	EWL_CALLBACK_REPARENT, 
	EWL_CALLBACK_KEY_DOWN, 
	EWL_CALLBACK_KEY_UP, 
	EWL_CALLBACK_MOUSE_DOWN, 
	EWL_CALLBACK_MOUSE_UP, 
	EWL_CALLBACK_MOUSE_MOVE, 
	EWL_CALLBACK_MOUSE_WHEEL, 
	EWL_CALLBACK_FOCUS_IN, 
	EWL_CALLBACK_FOCUS_OUT, 
	EWL_CALLBACK_SELECT, 
	EWL_CALLBACK_DESELECT, 
	EWL_CALLBACK_CLICKED, 
	EWL_CALLBACK_DOUBLE_CLICKED, 
	EWL_CALLBACK_HILITED, 
	EWL_CALLBACK_VALUE_CHANGED, 
	EWL_CALLBACK_STATE_CHANGED, 
	EWL_CALLBACK_APPEARANCE_CHANGED, 
	EWL_CALLBACK_WIDGET_ENABLE, 
	EWL_CALLBACK_WIDGET_DISABLE, 
	EWL_CALLBACK_PASTE, 
	-1
};

void
ewler_callbacks_init( void )
{
	callbacks = ecore_hash_new( ecore_direct_hash, ecore_direct_compare );
}

Ecore_List *
ewler_callbacks_create( Ecore_List *w )
{
  Ecore_List *l;

	l = ecore_list_new();

	ecore_hash_set( callbacks, w, l );

	return l;
}

Ecore_List *
ewler_callbacks_get( Ecore_List *w )
{
	Ecore_List *l;

	l = ecore_hash_get( callbacks, w );

	return l;
}

void
ewler_callbacks_destroy( Ecore_List *w )
{
	Ecore_List *l;

	l = ecore_hash_remove( callbacks, w );

	ecore_list_destroy( l );
}

int
ewler_callback_add( Ecore_List *w, char *handler, int callback )
{
	Ewler_Callback *cb;
	Ecore_List *l;

	if( !w || !handler )
		return -1;

	cb = (Ewler_Callback *) malloc( sizeof(Ewler_Callback) );
	if( !cb )
		return -1;

	cb->handler = strdup( handler );
	cb->callback = callback;

	l = ewler_callbacks_get( w );
	if( !l )
		return -1;

	ecore_list_append( l, cb );
	return 0;
}

int
ewler_callback_del( Ecore_List *w, char *handler, int callback )
{
	Ewler_Callback *cb;
	Ecore_List *l;

	l = ewler_callbacks_get( w );
	if( !w || !handler )
		return -1;

	ecore_list_goto_first( l );

	while( (cb = ecore_list_current( l )) ) {
		if( !strcmp( handler, cb->handler ) &&
				callback == cb->callback ) {
			ecore_list_remove( l );
			free( cb->handler );
			free( cb );
			return 0;
		}
		ecore_list_next( l );
	}

	return -1;
}


int
ewler_callback_value( char *callback )
{
	int i;

	for( i=0;callback_strings[i];i++ )
		if( !strcmp( callback_strings[i], callback ) )
			return callback_values[i];

	return -1;
}

char *
ewler_callback_string( int callback )
{
	int i;

	for( i=0;callback_strings[i];i++ )
		if( callback_values[i] == callback )
			return callback_strings[i];

	return NULL;
}
