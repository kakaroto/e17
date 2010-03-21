%{
#include "Eon.h"
#include "eon_private.h"

#include <stdio.h>
#include <string.h>

#define YYSTYPE char *
 
void yyerror(const char *str)
{
        fprintf(stderr,"error: %s\n",str);
}
 
int yywrap()
{
        return 1;
} 
  
%}

%token WORD OBRACE EBRACE SEMICOLON EQUAL QUOTE STRING

%%
commands:
	|
	commands command SEMICOLON
	;

command:
	object
	|
	property value
	;

property:
	WORD EQUAL
	{
		/* TODO check for the property */
		/* TODO get the type */
		/* how to store the type and pass it to the value grammar?? */ 
		printf("Property %s\n", $1);
	}
	;
value:
	object
	|
	STRING
	{
		printf("Value %s\n", $1);
	}
	;

object_start:
	WORD OBRACE
	{
		Ekeko_Object *o;

		/* check if the object is part of a property if so
		 * set the property and check the type of object
		 */

		/* check if there's already another object, if so
		 * append as child
		 */
		/* any other case is the root case, just create the object */
		printf("Object start %s\n", $1);
		o = eon_document_object_new(eon_parser_doc, $1);
		if (o)
			eina_array_push(eon_parser_stack, o);
		printf("Object created? %p\n", o);
	}
	;

object_end:
	commands EBRACE
	{
		printf("Object end\n");
	}
	;

object:
	object_start object_end
	;


%%
