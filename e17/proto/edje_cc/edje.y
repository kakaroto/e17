%{
#include <stdio.h>
#include <string.h>
#include "Etcher.h"

	#define YYDEBUG 1

	void yyerror(const char *s);
	void parse_error(void);

	extern int lnum;
	extern int col;

%}

%union {
	char *string;
	float val;
	Etcher_Program_Actions prog_actions;
	Etcher_Transition_Types transition_type;
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
%token COLON QUOTE SEMICOLON STATE_SET ACTION_STOP SIGNAL_EMIT
%token DRAG_VAL_SET DRAG_VAL_STEP DRAG_VAL_PAGE LINEAR
%token SINUSOIDAL ACCELERATE DECELERATE

%type <string> STRING lossless_type lossy_type 
%type <val> FLOAT
%type <prog_actions> action_type
%type <transition_type> transition_type

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

programs: PROGRAMS OPEN_BRACE program_statement CLOSE_BRACE
	;

program_statement: program
	| program program_statement
	;

program: PROGRAM OPEN_BRACE program_body CLOSE_BRACE
	;

program_body: /* blank */ 
	| program_cmd
	| program_cmd program_body
	;

program_cmd: program_name
	| program_signal
	| program_source
	| program_in
	| program_action
	| program_transition
	| program_target
	| program_after
	;

program_name: NAME COLON STRING SEMICOLON {
		printf("name: %s\n", $3);
	}
	;

program_signal: SIGNAL COLON STRING SEMICOLON {
		printf("signal: %s\n", $3);
	}
	;

program_source: SOURCE COLON STRING SEMICOLON {
		printf("source: %s\n", $3);
	}
	;

program_in: IN COLON FLOAT FLOAT SEMICOLON {
		printf("in %f %f\n", $3, $4);
	}
	;

program_action: ACTION COLON action_type STRING FLOAT SEMICOLON {
		printf("action %d %s %f\n", $3, $4, $5);
	}
	;

action_type: SIGNAL_EMIT { $$ = ETCHER_ACTION_SIGNAL_EMIT; }
	| STATE_SET { $$ = ETCHER_ACTION_STATE_SET; }
	| ACTION_STOP { $$ = ETCHER_ACTION_ACTION_STOP; }
	| DRAG_VAL_SET { $$ = ETCHER_ACTION_DRAG_VAL_SET; }
	| DRAG_VAL_STEP { $$ = ETCHER_ACTION_DRAG_VAL_STEP; }
	| DRAG_VAL_PAGE { $$ = ETCHER_ACTION_DRAG_VAL_PAGE; }
	;

program_transition: TRANSITION COLON transition_type FLOAT SEMICOLON {
		printf("transition %d %f\n", $3, $4);
	}
	;

transition_type: LINEAR { $$ = ETCHER_TRANSITION_LINEAR; }
	| SINUSOIDAL { $$ = ETCHER_TRANSITION_SINUSOIDAL; }
	| ACCELERATE { $$ = ETCHER_TRANSITION_ACCELERATE; }
	| DECELERATE { $$ = ETCHER_TRANSITION_DECELERATE; }
	;

program_target: TARGET COLON STRING SEMICOLON {
		printf("targeting %s\n", $3);
	}
	;

program_after: AFTER COLON STRING SEMICOLON {
		printf("after %s\n", $3);
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


