#ifndef EWLER_H
#define EWLER_H

#include "config.h"

#define ALLOC(t) (t *) malloc( sizeof( t ) )
#define CALLOC(t, n) (t *) calloc( sizeof( t ), n )

void ewler_menu_init( Ewl_Widget *main_layout );
void ewler_tools_init( Ewl_Widget *main_layout );
void ewler_populate_tools( void );
void ewler_error_dialog( const char *fmt, ... );
void ewler_open_file( char *filename );

char *tool_get_name( void );
void tool_clear_name( void );

#endif
