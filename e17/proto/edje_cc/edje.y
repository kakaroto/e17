%{
#include <stdio.h>
#include <string.h>

	#define YYDEBUG 1

	void yyerror(const char *s);
	void parse_error(void);

	extern int lnum;
	extern int col;
%}

%union {
	char *string;
	float val;
}

%token STRING FLOAT
%token ACTION AFTER ALIGN ASPECT ASPECT_PREFERENCE BORDER
%token CLIP_TO COLLECTIONS COLOR COLOR2 COLOR3 COLOR_CLASS
%token CONFINE DATA DESCRIPTION DRAGABLE EFFECT FILL FIT
%token FONT FONTS GROUP GROUPS IMAGE IMAGES IN ITEM MAX MIN MOUSE_EVENTS
%token NAME NORMAL OFFSET ORIGIN PART PARTS PROGRAM PROGRAMS
%token REL1 REL2 RELATIVE REPEAT_EVENTS SCRIPT SIGNAL SIZE
%token SMOOTH SOURCE STATE STEP TARGET TEXT TEXT_CLASS TO
%token TO_X TO_Y TRANSITION TWEEN TYPE VISIBLE X Y
%token OPEN_BRACE CLOSE_BRACE RAW COMP LOSSY
%token COLON QUOTE SEMICOLON

%type <string> STRING lossless_type lossy_type
%type <val> FLOAT

%%


edjes: /* blank */ | 
       images edjes |
       fonts edjes |
       collections edjes |
       data edjes |
       error {
       	   parse_error();
           yyerrok;
           yyclearin;
       }
       ;
collections:  COLLECTIONS OPEN_BRACE group CLOSE_BRACE
	;

fonts:  FONTS OPEN_BRACE font_statement CLOSE_BRACE
	;

font_statement: font
	| font font_statement
	;

font: FONT COLON STRING STRING SEMICOLON {
		printf("got font '%s' named (%s)\n", $3, $4);
	}
	;

images:  IMAGES OPEN_BRACE image_statement CLOSE_BRACE
	;

image_statement: image
	| image image_statement
	;

image: IMAGE COLON STRING lossless_type SEMICOLON {
		printf("got image '%s' of type %s\n", $3, $4);
	}
	| IMAGE COLON STRING lossy_type FLOAT SEMICOLON {
		printf("got image '%s' of type %s (%f)\n", $3, $4, $5);
	}
	;

lossy_type: LOSSY { $$ = strdup("lossy"); }

lossless_type: RAW { $$ = strdup("raw"); }
	| COMP { $$ = strdup("comp"); }
	;

data:  DATA OPEN_BRACE data_statement CLOSE_BRACE 
	;

data_statement: item 
	| item data_statement
	;

item: ITEM COLON STRING STRING SEMICOLON {
		printf("got item %s :: %s\n", $3, $4);
	}
	;

statement: STRING 
	|
	;
group:  statement 
	;

%%

void yyerror(const char *str) {
	fprintf(stderr, "yyerror: %s\n", str);
}
int yywrap() {
	return 1;
}

void parse_error(void) {
	fprintf(stderr, "file: %s, line: %d, column: %d\n\n",
                                     __FILE__, lnum, col);
	exit(-1);
}


