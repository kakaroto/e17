#include <Ewl.h>

#include "callback.h"

Ecore_Hash *callbacks;

void
ewler_callbacks_init( void )
{
	callbacks = ecore_hash_new( ecore_direct_hash, ecore_direct_compare );
}

Ecore_List *
ewler_callbacks_create( Ewl_Widget *w )
{
  Ecore_List *l;

	l = ecore_list_new();

	ecore_hash_set( callbacks, w, l );

	return l;
}

Ecore_List *
ewler_callbacks_get( Ewl_Widget *w )
{
	Ecore_List *l;

	l = ecore_hash_get( callbacks, w );

	return l;
}

void
ewler_callbacks_destroy( Ewl_Widget *w )
{
	Ecore_List *l;

	l = ecore_hash_remove( callbacks, w );

	ecore_list_destroy( l );
}

int
ewler_callback_add( Ewl_Widget *w, char *handler, int callback )
{
	Ewler_Callback *cb;
	Ecore_List *l;

	if( !w || !handler )
		return -1;

	cb = NEW(Ewler_Callback, 1);
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
ewler_callback_del( Ewl_Widget *w, char *handler, int callback )
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
			FREE( cb->handler );
			FREE( cb );
			return 0;
		}
		ecore_list_next( l );
	}

	return -1;
}

int
ewler_callback_replace( Ewl_Widget *w, Ewler_Callback *old_cb,
												char *handler, int callback )
{
	Ewler_Callback *cb;
	Ecore_List *l;

	l = ewler_callbacks_get( w );
	if( !w || !handler )
		return -1;

	ecore_list_goto_first( l );

	while( (cb = ecore_list_next( l )) )
		if( !strcmp( old_cb->handler, cb->handler ) &&
				old_cb->callback == cb->callback ) {
			FREE( cb->handler );

			cb->handler = strdup( handler );
			cb->callback = callback;
			return 0;
		}

	return -1;
}

Ewler_Callback *
ewler_callback_get( Ewl_Widget *w, char *handler, int callback )
{
	Ewler_Callback *cb;
	Ecore_List *l;

	l = ewler_callbacks_get( w );
	if( !w || !handler )
		return NULL;

	ecore_list_goto_first( l );

	while( (cb = ecore_list_next( l )) )
		if( !strcmp( handler, cb->handler ) &&
				callback == cb->callback ) {
			return cb;
		}

	return NULL;
}
