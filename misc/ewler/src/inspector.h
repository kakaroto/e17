#ifndef INSPECTOR_H
#define INSPECTOR_H

int inspector_visible( void );
void inspector_hide( void );
void inspector_show( void );
void ewler_inspector_init( void );
void inspector_update( void );
void inspector_reset( void );
Ewler_Form *inspector_get_form( void );
void inspector_set_form( Ewler_Form *form );

#endif
