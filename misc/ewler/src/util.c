#include <stdarg.h>

#include "ewler.h"

static int done = false;

void
ewler_fatal( const char *fmt, ... )
{
	va_list vargs;

	fputs( "FATAL: ", stderr );

	va_start( vargs, fmt );
	vfprintf( stderr, fmt, vargs );
	va_end( vargs );
	fputs( "\n", stderr );

	ewler_shutdown();
};

void
ewler_error( const char *fmt, ... )
{
	va_list vargs;

	fputs( "ERROR: ", stderr );

	va_start( vargs, fmt );
	vfprintf( stderr, fmt, vargs );
	va_end( vargs );
	fputs( "\n", stderr );
}

char *
strip( char *s )
{
	static char buf[255];
	int len;

	while( isspace( *s ) )
		++s;	

	len = strcspn( s, " \t\n\v\r" );

	if( len > 254 )
		return s;
	else {
		memcpy( buf, s, len );
		buf[len] = '\0';
	}

	return buf;
}

void
ewler_shutdown( void )
{
	done = true;
}

int
ewler_is_done( void )
{
	return done;
}
