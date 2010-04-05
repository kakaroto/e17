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
		Ekeko_Object *o;

		o = eina_array_last_get(eon_parser_stack);
		if (o)
		{
			eon_parser_property_id = ekeko_object_property_get(o, $1);
			if (eon_parser_property_id)
			{
				eon_parser_property_type = ekeko_object_property_type_get(o,
						eon_parser_property_id);
				printf("property %s found\n", eon_parser_property_id);
			}
			else
			{
				printf("Object %p does not have the property '%s'\n", o, $1);
			}
		}
		else 
		{
			printf("Not an object!\n");
		}
	}
	;
value:
	object
	|
	STRING
	{
		Ekeko_Object *o;
		Ekeko_Value value;

		printf("=> Value %s\n", $1);
		o = eina_array_last_get(eon_parser_stack);
		ekeko_value_string_from(&value, eon_parser_property_type, $1);
		ekeko_object_property_value_set(o, eon_parser_property_id,
				&value);
		eon_parser_property_id = NULL;
	}
	;

object_start:
	WORD OBRACE
	{
		Ekeko_Object *parent;
		Ekeko_Object *o;

		parent = eina_array_last_get(eon_parser_stack);
		o = eon_document_object_new(eon_parser_doc, $1);
		if (o)
		{
			printf("Object created? %p\n", o);
			eina_array_push(eon_parser_stack, o);
			/* check if the object is part of a property if so
			 * set the property and check the type of object
			 */
			if (eon_parser_property_id)
			{
				Ekeko_Value value;

				printf("=> Object %s\n", $1);
				ekeko_value_object_from(&value, o);
				ekeko_object_property_value_set(parent, eon_parser_property_id,
						&value);
				eon_parser_property_id = NULL;
			}
			/* check if there's already another object, if so
			 * append as child
			 */
			else if (parent)
			{
				ekeko_object_child_append(parent, o);
			}
		}
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
