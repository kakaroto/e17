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
	Etcher_Action prog_actions;
	Etcher_Transition transition_type;
	Etcher_Part_Type part_type;
	Etcher_Image_Type image_type;
	Etcher_Text_Effect text_effect;
	Etcher_Aspect_Preference aspect_pref;
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
%token SINUSOIDAL ACCELERATE DECELERATE IMAGE RECT SWALLOW
%token NONE PLAIN OUTLINE SOFT_OUTLINE SHADOW SOFT_SHADOW USER
%token OUTLINE_SHADOW OUTLINE_SOFT_SHADOW VERTICAL HORIZONTAL BOTH

%type <string> STRING 
%type <val> FLOAT
%type <prog_actions> action_type
%type <transition_type> transition_type
%type <part_type> part_type
%type <image_type> image_type
%type <text_effect> effect_type
%type <aspect_pref> aspect_pref_type

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
collections:  COLLECTIONS OPEN_BRACE collection_statement CLOSE_BRACE
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

image: IMAGE COLON STRING image_type SEMICOLON {
		printf("got image '%s' of type %s\n", $3, $4);
	}
	| IMAGE COLON STRING image_type FLOAT SEMICOLON {
		printf("got image '%s' of type %d (%f)\n", $3, $4, $5);
	}
	;

image_type: RAW { $$ = ETCHER_IMAGE_TYPE_RAW; }
	| COMP { $$ = ETCHER_IMAGE_TYPE_COMP; }
	| LOSSY { $$ = ETCHER_IMAGE_TYPE_LOSSY; }
	| USER { $$ = ETCHER_IMAGE_TYPE_EXTERNAL; }
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

program_cmd: name
	| program_signal
	| program_source
	| program_in
	| program_action
	| program_transition
	| program_target
	| program_after
	| script
	;

name: NAME COLON STRING SEMICOLON {
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
	| ACTION COLON action_type STRING STRING SEMICOLON {
		printf("action %d %s %s\n", $3, $4, $5);
	}
	| ACTION COLON action_type FLOAT FLOAT SEMICOLON {
		printf("action %d %f %f\n", $3, $4, $5);
	}
	| ACTION COLON action_type SEMICOLON {
		printf("action %d\n", $3);
	}
	;

action_type: SIGNAL_EMIT { $$ = ETCHER_ACTION_SIGNAL_EMIT; }
	| STATE_SET { $$ = ETCHER_ACTION_STATE_SET; }
	| ACTION_STOP { $$ = ETCHER_ACTION_STOP; }
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

collection_statement: group
	| group collection_statement
	;

group: GROUP OPEN_BRACE group_foo CLOSE_BRACE
	;

group_foo: 
	| group_preamble group_foo
	| group_body group_foo
	;

group_body: data
	| script
	| parts
	| programs
	;

script: SCRIPT {
		printf("script\n--%s\n--\n", yylval.string);
	}
	;

group_preamble: group_preamble_entry
	| group_preamble_entry group_preamble
	;

group_preamble_entry: name
	| min
	| max
	;

min: MIN COLON FLOAT FLOAT SEMICOLON {
		printf("min %f %f\n", $3, $4);
	}
	;

max: MAX COLON FLOAT FLOAT SEMICOLON {
		printf("max %f %f\n", $3, $4);
	}
	;

parts: PARTS OPEN_BRACE parts_statement CLOSE_BRACE
	;

parts_statement: part
	| part parts_statement
	;

part: PART OPEN_BRACE part_foo CLOSE_BRACE
	;

part_foo: 
	| part_preamble part_foo
	| part_body part_foo
	;

part_preamble: part_preamble_entry
	| part_preamble_entry part_preamble

part_preamble_entry: name
	| type
	| effect
	| mouse_events
	| repeat_events
	| clip_to
	| color_class
	| text_class
    ; 

type: TYPE COLON part_type SEMICOLON {
		printf("type %d\n", $3);
	}
	;

part_type: IMAGE { $$ = ETCHER_PART_TYPE_IMAGE; }
	| RECT { $$ = ETCHER_PART_TYPE_RECT; }
	| TEXT { $$ = ETCHER_PART_TYPE_TEXT; }
	| SWALLOW { $$ = ETCHER_PART_TYPE_SWALLOW; }
	;

effect: EFFECT COLON effect_type SEMICOLON {
		printf("effect %d\n", $3);
	}
	;

effect_type: NONE { $$ = ETCHER_TEXT_EFFECT_NONE; }
	| PLAIN { $$ = ETCHER_TEXT_EFFECT_PLAIN; }
	| OUTLINE { $$ = ETCHER_TEXT_EFFECT_OUTLINE; }
	| SOFT_OUTLINE { $$ = ETCHER_TEXT_EFFECT_SOFT_OUTLINE; }
	| SHADOW { $$ = ETCHER_TEXT_EFFECT_SHADOW; }
	| SOFT_SHADOW { $$ = ETCHER_TEXT_EFFECT_SOFT_SHADOW; }
	| OUTLINE_SOFT_SHADOW { $$ = ETCHER_TEXT_EFFECT_OUTLINE_SOFT_SHADOW; }
	;

mouse_events: MOUSE_EVENTS COLON FLOAT SEMICOLON {
		printf("mouse event %f\n", $3);
	}
	;

repeat_events: REPEAT_EVENTS COLON FLOAT SEMICOLON {
		printf("repeat events %d\n", $3);
	}
	;

clip_to: CLIP_TO COLON STRING SEMICOLON {
		printf("clip to (%s)\n", $3);
	}
	;

color_class: COLOR_CLASS COLON STRING SEMICOLON {
		printf("color class %s\n", $3);
	}
	;

text_class: TEXT_CLASS COLON STRING SEMICOLON {
		printf("text class %s\n", $3);
	}
	;

part_body: part_body_entry
	| part_body_entry part_body
	;

part_body_entry: dragable
	| description
	;

dragable: DRAGABLE OPEN_BRACE dragable_statement CLOSE_BRACE
	;

dragable_statement: dragable_body
	| dragable_body dragable_statement
	;

dragable_body: x
	| y
	| confine
	;

x: X COLON FLOAT FLOAT FLOAT SEMICOLON {
		printf("x %f %f %f\n", $3, $4, $5);
	}
	;

y: Y COLON FLOAT FLOAT FLOAT SEMICOLON {
		printf("y %f %f %f\n", $3, $4, $5);
	}
	;

confine: CONFINE COLON STRING SEMICOLON {
		printf("confine %s\n", $3);
	}
	;

description: DESCRIPTION OPEN_BRACE desc_foo CLOSE_BRACE
	;

desc_foo:
	| desc_preamble desc_foo
	| desc_body desc_foo
	;

desc_preamble: desc_preamble_entry
	| desc_preamble_entry desc_preamble
	;

desc_preamble_entry: state
	| visible
	| align
	| min
	| max
	| step
	| aspect
	| aspect_preference
	;

state: STATE COLON STRING FLOAT SEMICOLON {
		printf("state %s %f\n", $3, $4);
	}
	;

visible: VISIBLE COLON FLOAT SEMICOLON {
		printf("visible %f\n", $3);
	}
	;

align: ALIGN COLON FLOAT FLOAT SEMICOLON {
		printf("align %f %f\n", $3, $4);
	}
	;

step: STEP COLON FLOAT FLOAT SEMICOLON {
		printf("step %f %f\n", $3, $4);
	}
	;

aspect: ASPECT COLON FLOAT FLOAT SEMICOLON {
		printf("aspect %f %f\n", $3, $4);
	}
	;

aspect_preference: ASPECT_PREFERENCE COLON aspect_pref_type SEMICOLON {
		printf("aspect_preference %d\n", $3);
	}
	;

aspect_pref_type: NONE { $$ = ETCHER_ASPECT_PREFERENCE_NONE; }
	| VERTICAL { $$ = ETCHER_ASPECT_PREFERENCE_VERTICAL; }
	| HORIZONTAL { $$ = ETCHER_ASPECT_PREFERENCE_HORIZONTAL; }
	| BOTH { $$ = ETCHER_ASPECT_PREFERENCE_BOTH; }
	;

desc_body: desc_body_entry
	| desc_body_entry desc_body
	;

desc_body_entry: rel1
	| rel2
	| image
	| border
	| fill
	| color_class
	| color
	| color2
	| color3
	| text
	;

rel1: REL1 OPEN_BRACE rel_statement CLOSE_BRACE
	;

rel2: REL2 OPEN_BRACE rel_statement CLOSE_BRACE
	;

rel_statement: rel_body
	| rel_body rel_statement
	;

rel_body: relative
	| offset
	| to
	| to_x
	| to_y
	;

relative: RELATIVE COLON FLOAT FLOAT SEMICOLON {
		printf("relative %f %f\n", $3, $4);
	}
	;

offset: OFFSET COLON FLOAT FLOAT SEMICOLON {
		printf("offset %f %f\n", $3, $4);
	}
	;

to: TO COLON STRING SEMICOLON {
		printf("to %s\n", $3);
	}
	;

to_x: TO_X COLON STRING SEMICOLON {
		printf("to_x %s\n", $3);
	}
	;

to_y: TO_Y COLON STRING SEMICOLON {
		printf("to_y %s\n", $3);
	}
	;

image: IMAGE OPEN_BRACE image_statement CLOSE_BRACE
	;

image_statement: image_body
	| image_body image_statement
	;

image_body: normal
	| tween
	;

normal: NORMAL COLON STRING SEMICOLON {
		printf("normal %s\n", $3);
	}
	;

tween: TWEEN COLON STRING SEMICOLON {
		printf("tween %s\n", $3);
	}
	;

border: BORDER COLON FLOAT FLOAT FLOAT FLOAT SEMICOLON {
		printf("border %f %f %f %f\n", $3, $4, $5, $6);
	}
	;

fill: FILL OPEN_BRACE fill_statement CLOSE_BRACE
	;

fill_statement: fill_body
	| fill_body fill_statement
	;

fill_body: smooth
	| origin
	| size
	;

smooth: SMOOTH COLON FLOAT SEMICOLON {
		printf("smooth %f\n", $3);
	}
	;

origin: ORIGIN OPEN_BRACE origin_statement CLOSE_BRACE
	;

origin_statement: origin_body
	| origin_statement origin_body
	;

origin_body: relative
	| offset
	;

size: SIZE OPEN_BRACE origin_statement CLOSE_BRACE
	;

color_class: COLOR_CLASS COLON STRING SEMICOLON {
		printf("colour class %s\n", $3);
	}
	;

color: COLOR COLON FLOAT FLOAT FLOAT FLOAT SEMICOLON {
		printf("color %f %f %f %f\n", $3, $4, $5, $6);
	}
	;

color2: COLOR2 COLON FLOAT FLOAT FLOAT FLOAT SEMICOLON {
		printf("color2 %f %f %f %f\n", $3, $4, $5, $6);
	}
	;
		
color3: COLOR3 COLON FLOAT FLOAT FLOAT FLOAT SEMICOLON {
		printf("color3 %f %f %f %f\n", $3, $4, $5, $6);
	}
	;

text: TEXT OPEN_BRACE text_statement CLOSE_BRACE
	;

text_statement: text_body
	| text_body text_statement
	;

text_body: text_entry
	| text_class
	| font_entry
	| size_entry
	| fit
	| min
	| align
	;

text_entry: TEXT COLON STRING SEMICOLON {
		printf("text (%s)\n", $3);
	}
	;

text_class: TEXT_CLASS COLON STRING SEMICOLON {
		printf("text_class %s\n", $3);
	}
	;

font_entry: FONT COLON STRING SEMICOLON {
		printf("font %s\n", $3);
	}
	;

size_entry: SIZE COLON FLOAT SEMICOLON {
		printf("size %f\n", $3);
	}
	;

fit: FIT COLON FLOAT FLOAT SEMICOLON {
		printf("fit %f %f\n", $3, $4);
	}
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


