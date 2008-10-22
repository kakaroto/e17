%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Engrave.h"
#include "engrave_parse.h"

#define YYDEBUG 1
	int yylex(void);
	void yyerror(const char *s);
	void parse_error(void);

	extern char *engrave_cur_file;
	extern int engrave_lnum;
	extern int engrave_col;

	static int section;

%}

%union {
	char *string;
	float val;
	Engrave_Action prog_actions;
	Engrave_Transition transition_type;
	Engrave_Part_Type part_type;
	Engrave_Image_Type image_type;
	Engrave_Text_Effect text_effect;
	Engrave_Aspect_Preference aspect_pref;
	Engrave_Fill_Type fill_type_type;
	Engrave_Pointer_Mode pointer_mode;
}

%token BASE
%token STRING FLOAT
%token ACTION AFTER ALIGN ASPECT ASPECT_PREFERENCE BORDER MIDDLE
%token CLIP_TO COLLECTIONS COLOR COLOR2 COLOR3 COLOR_CLASS COLOR_CLASSES
%token CONFINE DATA DESCRIPTION DRAGABLE EFFECT FILL FIT TILE SCALE
%token FONT FONTS GROUP GROUPS IMAGE TEXTBLOCK IMAGES IN ITEM MAX MIN FIXED MOUSE_EVENTS
%token NAME NORMAL OFFSET ORIGIN PART PARTS PROGRAM PROGRAMS ALIAS
%token REL1 REL2 RELATIVE REPEAT_EVENTS SCRIPT SIGNAL SIZE GRADREL1 GRADREL2
%token SMOOTH SOURCE STATE STEP TARGET TEXT TEXT_CLASS TEXT_SOURCE TO
%token TO_X TO_Y TRANSITION TWEEN TYPE VISIBLE X Y
%token OPEN_BRACE CLOSE_BRACE RAW COMP LOSSY ACTION_SCRIPT
%token STYLES STYLE SBASE TAG ELIPSIS PRECISE_IS_INSIDE
%token COLON QUOTE SEMICOLON STATE_SET ACTION_STOP SIGNAL_EMIT
%token DRAG_VAL_SET DRAG_VAL_STEP DRAG_VAL_PAGE LINEAR
%token SINUSOIDAL ACCELERATE DECELERATE RECT SWALLOW GRADIENT TGROUP
%token NONE PLAIN OUTLINE SOFT_OUTLINE SHADOW SOFT_SHADOW FAR_SHADOW FAR_SOFT_SHADOW GLOW
%token OUTLINE_SHADOW OUTLINE_SOFT_SHADOW VERTICAL HORIZONTAL BOTH
%token SPECTRA SPECTRUM GRAD POINTER_MODE NOGRAB AUTOGRAB
%left MINUS PLUS
%left TIMES DIVIDE
%left NEG     /* negation--unary minus */
%token OPEN_PAREN CLOSE_PAREN DOT INHERIT
%token ON OFF ETRUE EFALSE PERCENT

%type <string> STRING 
%type <val> FLOAT
%type <prog_actions> action_type
%type <transition_type> transition_type
%type <part_type> part_type
%type <image_type> image_type
%type <text_effect> effect_type
%type <aspect_pref> aspect_pref_type
%type <val> exp boolean
%type <fill_type_type> fill_type_type;
%type <pointer_mode> pointer_mode grabmode;

%%

start: { section = BASE; } edjes
    ;

edjes: /* blank */
	| images edjes 
	| fonts edjes
	| spectra edjes
	| styles edjes
	| collections edjes
	| color_classes {section = BASE; } edjes
	| data edjes
	| error {
		parse_error();
		yyerrok;
		yyclearin;
	}
	;

collections:  COLLECTIONS OPEN_BRACE {section = GROUPS; } collection_statement CLOSE_BRACE semicolon_maybe { section = BASE; }
	;

color_classes: COLOR_CLASSES OPEN_BRACE color_classes_entry CLOSE_BRACE semicolon_maybe
	;

color_classes_entry: /* empty */
	| color_class_block color_classes_entry
	;

color_class_block: COLOR_CLASS OPEN_BRACE { 
	engrave_parse_file_color_class();
	section = COLOR_CLASS; 
	} color_class_statement CLOSE_BRACE semicolon_maybe  /* don't reset section here, it should be set from calling block */
	;

color_class_statement: /* empty */
	| name color_class_statement
	| color color_class_statement
	| color2 color_class_statement
	| color3 color_class_statement
	;

spectra: SPECTRA OPEN_BRACE { section = SPECTRA; } spectra_statement CLOSE_BRACE semicolon_maybe { section = BASE; }
	;

spectra_statement: /* empty */
	| spectra_statement spectrum
	;

spectrum: SPECTRUM OPEN_BRACE { 
		section = SPECTRUM; 
		engrave_parse_spectrum();
	} spectrum_statement CLOSE_BRACE semicolon_maybe { section = SPECTRA; }
	;

spectrum_statement: /* blank */
	| spectrum_name spectrum_statement 
	| spectrum_color spectrum_statement
	;

spectrum_name: NAME COLON STRING SEMICOLON {
		engrave_parse_spectrum_name($3);
	}
	;

spectrum_color: COLOR COLON exp exp exp exp exp SEMICOLON {
                engrave_parse_spectrum_color((int)$3, (int)$4, (int)$5, (int)$6, (int)$7);
	}
	;

fonts:  FONTS OPEN_BRACE { section = FONTS; } font_statement CLOSE_BRACE semicolon_maybe { section = BASE; }
	;

font_statement: /* empty */
	| font_statement font
	;

font: FONT COLON STRING STRING SEMICOLON {
                engrave_parse_font($3, $4);
	}
	;

styles: STYLES OPEN_BRACE { section = STYLES; } styles_statement CLOSE_BRACE semicolon_maybe { section = BASE; }
	;

styles_statement: 
	| styles_statement style
	;

style: STYLE OPEN_BRACE { engrave_parse_style(); section = STYLE; } style_statement CLOSE_BRACE semicolon_maybe { section = STYLES; }
	;

style_statement: /* empty */
	| style_statement style_entry
	;

style_entry: name
	| base
	| tag
	;

base: BASE COLON STRING SEMICOLON {
		engrave_parse_base($3);
	}
	;

images:  IMAGES OPEN_BRACE { section = IMAGES; } image_statement CLOSE_BRACE semicolon_maybe { section = BASE; }
	;

image_statement: /* empty */
	| image_statement image
	;

image: IMAGE COLON STRING image_type SEMICOLON {
                engrave_parse_image($3, $4, 0);
	}
	| IMAGE COLON STRING image_type exp SEMICOLON {
                engrave_parse_image($3, $4, $5);
	}
	| IMAGE COLON STRING image_type exp PERCENT SEMICOLON {
                engrave_parse_image($3, $4, $5);
	}
	;

image_type: RAW { $$ = ENGRAVE_IMAGE_TYPE_RAW; }
	| COMP { $$ = ENGRAVE_IMAGE_TYPE_COMP; }
	| LOSSY { $$ = ENGRAVE_IMAGE_TYPE_LOSSY; }
	;

/* don't set a section here yet (since BASE and GROUP have data sects) */
data:  DATA OPEN_BRACE data_statement CLOSE_BRACE semicolon_maybe
	;

data_statement: /* empty */
	| data_statement item
	;

item: ITEM COLON STRING STRING SEMICOLON {
                switch (section)
                {
                  case BASE:
                    engrave_parse_data($3, $4, 0);
                    break;
                  case GROUP:
                    engrave_parse_group_data($3, $4, 0);
                    break;
                  default:
                    break;
                }
	}
	| ITEM COLON STRING exp SEMICOLON {
                switch (section)
                {
                  case BASE:
                    engrave_parse_data($3, NULL, (int)$4);
                    break;
                  case GROUP:
                    engrave_parse_group_data($3, NULL, (int)$4);
                    break;
                  default:
                    break;
                }
	}
	;

programs: PROGRAMS OPEN_BRACE { section = PROGRAMS; } program_statement CLOSE_BRACE semicolon_maybe { section = BASE; }
	;

program_statement: /* empty */
	| program_statement program
	;

program: PROGRAM OPEN_BRACE { engrave_parse_program(); section = PROGRAM; } program_body CLOSE_BRACE semicolon_maybe { section = PROGRAMS; }
	;

program_body: /* blank */ 
	| program_body program_cmd
	| program_body SEMICOLON
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
                switch(section)
                {
                  case GROUP:
                    engrave_parse_group_name($3);
                    break;
                  case PART:
                    engrave_parse_part_name($3);
                    break;
                  case PROGRAM:
                    engrave_parse_program_name($3);
                    break;
		  case STYLE:
		    engrave_parse_style_name($3);
		  case COLOR_CLASS:
		    engrave_parse_color_class_name($3);
                  default:
                    break;
                }
	}
	;

base: SBASE COLON STRING SEMICOLON {
		engrave_parse_style_base($3);
	}
	;

tag: TAG COLON STRING STRING SEMICOLON {
		engrave_parse_style_tag($3, $4);
	}	
	;

program_signal: SIGNAL COLON STRING SEMICOLON {
                engrave_parse_program_signal($3);
	}
	;

program_source: SOURCE COLON STRING SEMICOLON {
                engrave_parse_program_source($3);
	}
	;

program_in: IN COLON exp exp SEMICOLON {
                engrave_parse_program_in($3, $4);
	}
	;

program_action: ACTION COLON action_type STRING exp SEMICOLON {
                engrave_parse_program_action($3, $4, NULL, $5, 0);
	}
	| ACTION COLON action_type STRING STRING SEMICOLON {
                engrave_parse_program_action($3, $4, $5, 0, 0);
	}
	| ACTION COLON action_type exp exp SEMICOLON {
                engrave_parse_program_action($3, NULL, NULL, $4, $5);
	}
	| ACTION COLON action_type SEMICOLON {
                engrave_parse_program_action($3, NULL, NULL, 0, 0);
	}
	;

action_type: SIGNAL_EMIT { $$ = ENGRAVE_ACTION_SIGNAL_EMIT; }
	| STATE_SET { $$ = ENGRAVE_ACTION_STATE_SET; }
	| ACTION_STOP { $$ = ENGRAVE_ACTION_STOP; }
	| DRAG_VAL_SET { $$ = ENGRAVE_ACTION_DRAG_VAL_SET; }
	| DRAG_VAL_STEP { $$ = ENGRAVE_ACTION_DRAG_VAL_STEP; }
	| DRAG_VAL_PAGE { $$ = ENGRAVE_ACTION_DRAG_VAL_PAGE; }
	| ACTION_SCRIPT { $$ = ENGRAVE_ACTION_SCRIPT; }
	;

program_transition: TRANSITION COLON transition_type exp SEMICOLON {
                engrave_parse_program_transition($3, $4);
	}
	;

transition_type: LINEAR { $$ = ENGRAVE_TRANSITION_LINEAR; }
	| SINUSOIDAL { $$ = ENGRAVE_TRANSITION_SINUSOIDAL; }
	| ACCELERATE { $$ = ENGRAVE_TRANSITION_ACCELERATE; }
	| DECELERATE { $$ = ENGRAVE_TRANSITION_DECELERATE; }
	;

program_target: TARGET COLON STRING SEMICOLON {
                engrave_parse_program_target($3);
	}
	;

program_after: AFTER COLON STRING SEMICOLON {
                engrave_parse_program_after($3);
	}
	;

collection_statement: /* empty */
	| collection_statement styles
	| collection_statement images
	| collection_statement group
	| collection_statement color_classes { section = GROUPS; } 
	;

group: GROUP OPEN_BRACE { engrave_parse_group(); section = GROUP; } group_foo CLOSE_BRACE semicolon_maybe { section = GROUPS; }
	;

group_foo: 
	| group_preamble group_foo
	| group_body group_foo
	;

group_body: data
	| script
	| parts
	| programs
	| color_classes { section = GROUP; }
	;

script: SCRIPT {
                switch (section)
                {
                  case GROUP:
                    engrave_parse_group_script(yylval.string);
                    break;
                  case PROGRAM:
                    engrave_parse_program_script(yylval.string);
                    break;
                  default:
                    break;
                }
                free(yylval.string);
                yylval.string = NULL;
	}
	;

group_preamble: group_preamble_entry
	| group_preamble_entry group_preamble
	;

group_preamble_entry: name
	| alias
	| min
	| max
	;

alias: ALIAS COLON STRING SEMICOLON {
		engrave_parse_group_alias($3);
	}
	;

min: MIN COLON exp exp SEMICOLON {
                switch(section)
                {
                  case GROUP:
                    engrave_parse_group_min((int)$3, (int)$4);
                    break;
                  case STATE:
                    engrave_parse_state_min((int)$3, (int)$4);
                    break;
                  case TEXT:
                    engrave_parse_state_text_min((int)$3, (int)$4);
                    break;
                  default: 
                    break;
                }
	}
	;

fixed: FIXED COLON exp exp SEMICOLON {
               engrave_parse_state_fixed((int)$3, (int)$4);
	}
	;



max: MAX COLON exp exp SEMICOLON {
                switch(section)
                {
                  case GROUP:
                    engrave_parse_group_max((int)$3, (int)$4);
                    break;
                  case STATE:
                    engrave_parse_state_max((int)$3, (int)$4);
                    break;
                  case TEXT:
                    engrave_parse_state_text_max((int)$3, (int)$4);
                    break;
                  default: 
                    break;
                }
	}
	;

parts: PARTS OPEN_BRACE { section = PARTS; } parts_statement CLOSE_BRACE semicolon_maybe { section = BASE; }
	;

parts_statement: /* empty */
	| parts_statement part
	| parts_statement program_statement
	| parts_statement programs
	| parts_statement color_classes { section = PARTS; }
	;

part: PART OPEN_BRACE { engrave_parse_part(); section = PART; } part_foo CLOSE_BRACE semicolon_maybe { section = PARTS; }
	;

part_foo: 
	| part_preamble part_foo
	| part_body part_foo
	;

part_preamble: part_preamble_entry
	| part_preamble_entry part_preamble
    ;

part_preamble_entry: name
	| type
	| part_source
	| effect
	| mouse_events
	| repeat_events
	| precise_is_inside
	| pointer_mode
	| clip_to
	| color_class
	| color_classes { section = PART; }
	| text_class
    ; 


part_source: SOURCE COLON STRING SEMICOLON {
		engrave_parse_part_source($3);
	}
	;

type: TYPE COLON part_type SEMICOLON {
                engrave_parse_part_type($3);
	}
	;

part_type: IMAGE { $$ = ENGRAVE_PART_TYPE_IMAGE; }
	| RECT { $$ = ENGRAVE_PART_TYPE_RECT; }
	| TEXT { $$ = ENGRAVE_PART_TYPE_TEXT; }
	| TEXTBLOCK { $$ = ENGRAVE_PART_TYPE_TEXTBLOCK; }
	| SWALLOW { $$ = ENGRAVE_PART_TYPE_SWALLOW; }
	| GRADIENT { $$ = ENGRAVE_PART_TYPE_GRADIENT; }
	| TGROUP { $$ = ENGRAVE_PART_TYPE_GROUP; }
	| STRING { /* edje accepts quoted part types. */
		if (!strcmp($1, "RECT"))
			$$ = ENGRAVE_PART_TYPE_RECT;
		else if (!strcmp($1, "IMAGE"))
			$$ = ENGRAVE_PART_TYPE_IMAGE;
		else if (!strcmp($1, "TEXT"))
			$$ = ENGRAVE_PART_TYPE_TEXT;
		else if (!strcmp($1, "TEXTBLOCK"))
			$$ = ENGRAVE_PART_TYPE_TEXTBLOCK;
		else if (!strcmp($1, "SWALLOW"))
			$$ = ENGRAVE_PART_TYPE_SWALLOW;
		else if (!strcmp($1, "GRADIENT"))
			$$ = ENGRAVE_PART_TYPE_GRADIENT;
		else if (!strcmp($1, "GROUP"))
			$$ = ENGRAVE_PART_TYPE_GROUP;
	}
	;

effect: EFFECT COLON effect_type SEMICOLON {
                engrave_parse_part_effect($3);
	}
	;

effect_type: NONE { $$ = ENGRAVE_TEXT_EFFECT_NONE; }
	| PLAIN { $$ = ENGRAVE_TEXT_EFFECT_PLAIN; }
	| OUTLINE { $$ = ENGRAVE_TEXT_EFFECT_OUTLINE; }
	| SOFT_OUTLINE { $$ = ENGRAVE_TEXT_EFFECT_SOFT_OUTLINE; }
	| SHADOW { $$ = ENGRAVE_TEXT_EFFECT_SHADOW; }
	| SOFT_SHADOW { $$ = ENGRAVE_TEXT_EFFECT_SOFT_SHADOW; }
	| OUTLINE_SOFT_SHADOW { $$ = ENGRAVE_TEXT_EFFECT_OUTLINE_SOFT_SHADOW; }
	| FAR_SHADOW { $$ = ENGRAVE_TEXT_EFFECT_FAR_SHADOW; }
	| FAR_SOFT_SHADOW { $$ = ENGRAVE_TEXT_EFFECT_FAR_SOFT_SHADOW; }
	| GLOW { $$ = ENGRAVE_TEXT_EFFECT_GLOW; }
	;

boolean: ON { $$ = 1; }
	| OFF { $$ = 0; }
	| ETRUE { $$ = 1; }
	| EFALSE { $$ = 0; }
	| exp {
		int i = $1;
		if ((i != 0) && (i != 1)) {
			printf("Invalid boolean %d at line %d\n", i, engrave_lnum);
   			i = 0;
		}
		$$ = i;
	}
	;

mouse_events: MOUSE_EVENTS COLON boolean SEMICOLON {
                engrave_parse_part_mouse_events((int)$3);
	}
	;

repeat_events: REPEAT_EVENTS COLON boolean SEMICOLON {
                engrave_parse_part_repeat_events((int)$3);
	}
	;

precise_is_inside: PRECISE_IS_INSIDE COLON boolean SEMICOLON {
                engrave_parse_part_precise_is_inside((int)$3);
	}
	;

pointer_mode: POINTER_MODE COLON grabmode SEMICOLON {
                engrave_parse_part_pointer_mode((int)$3);
	}
	;

grabmode: NOGRAB { $$ = ENGRAVE_POINTER_NOGRAB; }
	| AUTOGRAB { $$ = ENGRAVE_POINTER_AUTOGRAB; }
	;

clip_to: CLIP_TO COLON STRING SEMICOLON {
                engrave_parse_part_clip_to($3);
	}
	;

part_body: part_body_entry
	| part_body_entry part_body
	;

part_body_entry: dragable
	| description
	;

dragable: DRAGABLE OPEN_BRACE { section = DRAGABLE; } dragable_statement CLOSE_BRACE semicolon_maybe { section = PART; }
	;

dragable_statement: /* empty */
	| dragable_statement dragable_body
	;

dragable_body: x
	| y
	| confine
	;

x: X COLON exp exp exp SEMICOLON {
                engrave_parse_part_dragable_x((int)$3, (int)$4, (int)$5);
	}
	;

y: Y COLON exp exp exp SEMICOLON {
                engrave_parse_part_dragable_y((int)$3, (int)$4, (int)$5);
	}
	;

confine: CONFINE COLON STRING SEMICOLON {
                engrave_parse_part_dragable_confine($3);
	}
	;

description: DESCRIPTION OPEN_BRACE { engrave_parse_state(); section = STATE; } desc_foo CLOSE_BRACE semicolon_maybe { section = PART; }
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
	| inherit
	| align
	| min
	| max
	| fixed
	| step
	| aspect
	| aspect_preference
	| color_classes { section = STATE; }
	;

state: STATE COLON STRING exp SEMICOLON {
                engrave_parse_state_name($3, $4);
	}
	;

visible: VISIBLE COLON boolean SEMICOLON {
                engrave_parse_state_visible((int)$3);
	}
	;

inherit: INHERIT COLON STRING exp SEMICOLON {
                engrave_parse_state_inherit($3, $4);
    }
    ;


align: ALIGN COLON exp exp SEMICOLON {
                switch(section)
                {
                  case STATE:
                    engrave_parse_state_align($3, $4);
                    break;
                  case TEXT:
                    engrave_parse_state_text_align($3, $4);
                    break;
                  default:
                    break;
                }
	}
	;

step: STEP COLON exp exp SEMICOLON {
                engrave_parse_state_step($3, $4);
	}
	;

aspect: ASPECT COLON exp exp SEMICOLON {
                engrave_parse_state_aspect($3, $4);
	}
	;

aspect_preference: ASPECT_PREFERENCE COLON aspect_pref_type SEMICOLON {
                engrave_parse_state_aspect_preference($3);
	}
	;

aspect_pref_type: NONE { $$ = ENGRAVE_ASPECT_PREFERENCE_NONE; }
	| VERTICAL { $$ = ENGRAVE_ASPECT_PREFERENCE_VERTICAL; }
	| HORIZONTAL { $$ = ENGRAVE_ASPECT_PREFERENCE_HORIZONTAL; }
	| BOTH { $$ = ENGRAVE_ASPECT_PREFERENCE_BOTH; }
	;

desc_body: desc_body_entry
	| desc_body_entry desc_body
	;

desc_body_entry: rel1
	| rel2
	| desc_image
	| fill
	| color_class
	| color
	| color2
	| color3
	| text
	| gradient
	;

gradient: GRAD OPEN_BRACE {section = GRAD;} grad_statement CLOSE_BRACE semicolon_maybe { section = STATE;}
	;

grad_statement: /* empty */
	| grad_spectrum grad_statement
	| grad_type grad_statement
	| grad_rel1 grad_statement
	| grad_rel2 grad_statement
	;

grad_spectrum: SPECTRUM COLON STRING SEMICOLON {
		engrave_parse_state_gradient_spectrum($3);
	}
	;

grad_type: TYPE COLON STRING SEMICOLON {
		engrave_parse_state_gradient_type($3);
	}
	;

grad_rel1: REL1 OPEN_BRACE {section = GRADREL1;} rel_statement CLOSE_BRACE semicolon_maybe {section = GRAD;}
	| REL1 DOT {section = GRADREL1;} rel_body {section = GRAD;}
	;

grad_rel2: REL2 OPEN_BRACE {section = GRADREL2;} rel_statement CLOSE_BRACE semicolon_maybe {section = GRAD;}
	| REL2 DOT {section = GRADREL2;} rel_body {section = GRAD;}
	;

rel1: REL1 OPEN_BRACE {section = REL1;} rel_statement CLOSE_BRACE semicolon_maybe {section = STATE;}
	| REL1 DOT {section = REL1;} rel_body {section = STATE;}
	;

rel2: REL2 OPEN_BRACE {section = REL2;} rel_statement CLOSE_BRACE semicolon_maybe {section = STATE;}
	| REL2 DOT {section = REL2;} rel_body {section = STATE;}
	;

rel_statement: /* empty */ 
	| rel_statement rel_body
	;

rel_body: relative
	| offset
	| to
	| to_x
	| to_y
	;

relative: RELATIVE COLON exp exp SEMICOLON {
                switch(section)
                {
                  case REL1:
                    engrave_parse_state_rel1_relative($3, $4);
                    break;
                  case REL2:
                    engrave_parse_state_rel2_relative($3, $4);
                    break;
                  case ORIGIN:
                    engrave_parse_state_fill_origin_relative($3, $4);
                    break;
                  case SIZE:
                    engrave_parse_state_fill_size_relative($3, $4);
                    break;
		  case GRADREL1:
		    engrave_parse_state_gradient_rel1_relative($3, $4);
		    break;
		  case GRADREL2:
		    engrave_parse_state_gradient_rel2_relative($3, $4);
		    break;
                  default: 
                    break;
                }
	}
	;

offset: OFFSET COLON exp exp SEMICOLON {
                switch(section)
                {
                  case REL1:
                    engrave_parse_state_rel1_offset((int)$3, (int)$4);
                    break;
                  case REL2:
                    engrave_parse_state_rel2_offset((int)$3, (int)$4);
                    break;
                  case ORIGIN:
                    engrave_parse_state_fill_origin_offset((int)$3, (int)$4);
                    break;
                  case SIZE:
                    engrave_parse_state_fill_size_offset((int)$3, (int)$4);
                    break;
                  case GRADREL1:
                    engrave_parse_state_gradient_rel1_offset((int)$3, (int)$4);
                    break;
                  case GRADREL2:
                    engrave_parse_state_gradient_rel2_offset((int)$3, (int)$4);
                    break;
                  default: 
                    break;
                }
	}
	;

to: TO COLON STRING SEMICOLON {
                switch(section)
                {
                  case REL1:
                    engrave_parse_state_rel1_to($3);
                    break;
                  case REL2:
                    engrave_parse_state_rel2_to($3);
                    break;
                  default: 
                    fprintf(stderr, "Error: \"to\" not allowed here %d, %d",
                                                    engrave_lnum, engrave_col);
                }
	}
	;

to_x: TO_X COLON STRING SEMICOLON {
                switch(section)
                {
                  case REL1:
                    engrave_parse_state_rel1_to_x($3);
                    break;
                  case REL2:
                    engrave_parse_state_rel2_to_x($3);
                    break;
                  default: 
                    break;
                }
	}
	;

to_y: TO_Y COLON STRING SEMICOLON {
                switch(section)
                {
                  case REL1:
                    engrave_parse_state_rel1_to_y($3);
                    break;
                  case REL2:
                    engrave_parse_state_rel2_to_y($3);
                    break;
                  default: 
                    break;
                }
	}
	;

desc_image: IMAGE OPEN_BRACE { section = IMAGE; } image_state_statement CLOSE_BRACE semicolon_maybe { section = STATE; }
	| IMAGE DOT { section = IMAGE; } image_body { section = STATE; }
	;

image_state_statement: /* empty */ 
	| image_state_statement image_body
	;

image_body: normal
	| tween
	| border
	| middle
	;

middle: MIDDLE COLON exp SEMICOLON {
                engrave_parse_state_image_middle((int)$3);
	}
	;

normal: NORMAL COLON STRING SEMICOLON {
                engrave_parse_state_image_normal($3);
	}
	;

tween: TWEEN COLON STRING SEMICOLON {
                engrave_parse_state_image_tween($3);
	}
	;

border: BORDER COLON exp exp exp exp SEMICOLON {
                engrave_parse_image_border((int)$3, (int)$4, (int)$5, (int)$6);
	}
	;

fill: FILL OPEN_BRACE { section = FILL; } fill_statement CLOSE_BRACE semicolon_maybe { section = STATE; }
	| FILL DOT {section = FILL; } fill_body { section = STATE; }
	;

fill_statement: /* empty */
	| fill_statement fill_body
	;

fill_body: smooth
	| origin
	| size
	| fill_type
	;

fill_type: TYPE COLON fill_type_type SEMICOLON {
		engrave_parse_state_fill_type($3);
	}
	;

fill_type_type: SCALE { $$ = ENGRAVE_FILL_TYPE_SCALE; }
	| TILE { $$ = ENGRAVE_FILL_TYPE_TILE; } 
	;

smooth: SMOOTH COLON boolean SEMICOLON {
                engrave_parse_state_fill_smooth((int)$3);
	}
	;

origin: ORIGIN OPEN_BRACE { section = ORIGIN; } origin_statement CLOSE_BRACE semicolon_maybe { section = FILL; }
	| ORIGIN DOT { section = ORIGIN; } origin_body { section = FILL; }
	;

origin_statement: /* empty */
	| origin_statement origin_body
	;

origin_body: relative
	| offset
	;

size: SIZE OPEN_BRACE { section = SIZE; } origin_statement CLOSE_BRACE semicolon_maybe { section = FILL; }
	| SIZE DOT { section = SIZE; } origin_body { section = FILL; }
	;

color_class: COLOR_CLASS COLON STRING SEMICOLON {
                engrave_parse_state_color_class($3);
	}
	;

color: COLOR COLON exp exp exp exp SEMICOLON {
	switch(section)
	{
		case COLOR_CLASS:
			engrave_parse_color_class_color(1, (int)$3, (int)$4, (int)$5, (int)$6);
			break;
		default:
                	engrave_parse_state_color((int)$3, (int)$4, (int)$5, (int)$6);
	}
	}
	;

color2: COLOR2 COLON exp exp exp exp SEMICOLON {
       	switch(section)
	{
		case COLOR_CLASS:
			engrave_parse_color_class_color(2, (int)$3, (int)$4, (int)$5, (int)$6);
			break;
		default:
                	engrave_parse_state_color2((int)$3, (int)$4, (int)$5, (int)$6);
	}
	}
	;
		
color3: COLOR3 COLON exp exp exp exp SEMICOLON {
        switch(section)
	{
		case COLOR_CLASS:
			engrave_parse_color_class_color(3, (int)$3, (int)$4, (int)$5, (int)$6);
			break;
		default:
                	engrave_parse_state_color3((int)$3, (int)$4, (int)$5, (int)$6);
	}
	}
	;

text: TEXT OPEN_BRACE { section = TEXT; } text_statement CLOSE_BRACE semicolon_maybe { section = STATE; }
	| TEXT DOT { section = TEXT; } text_body { section = STATE; }
	;

semicolon_maybe: /* after braces, we can have semicolons. Is this how to solve this? */
	| SEMICOLON
	;

text_statement: /* empty */
	| text_statement text_body
	;

text_body: text_entry
	| text_class
	| text_source
	| text_style
	| font_entry
	| size_entry
	| fit
	| elipsis
	| min
	| max
	| align
	| source
	;

elipsis: ELIPSIS COLON exp SEMICOLON {
		engrave_parse_state_text_elipsis((int)$3);
	}
	;

text_style: STYLE COLON STRING SEMICOLON {
		engrave_parse_state_text_style($3);
	}
	;

source: SOURCE COLON STRING SEMICOLON {
		engrave_parse_state_text_source($3);
	}
	;

text_entry: TEXT COLON text_string SEMICOLON
	;

text_string: 
	| STRING {
                engrave_parse_state_text_text_add($1);
	}
	| text_string STRING {
                engrave_parse_state_text_text_add($2);
	}
	;

text_source: TEXT_SOURCE COLON STRING SEMICOLON {
                engrave_parse_state_text_text_source($3);
	}
	;

text_class: TEXT_CLASS COLON STRING SEMICOLON {
                engrave_parse_state_text_text_class($3);
	}
	;

font_entry: FONT COLON STRING SEMICOLON {
                engrave_parse_state_text_font($3);
	}
	;

size_entry: SIZE COLON exp SEMICOLON {
                engrave_parse_state_text_size((int)$3);
	}
	;

fit: FIT COLON boolean boolean SEMICOLON {
                engrave_parse_state_text_fit((int)$3, (int)$4);
	}
	;

exp: FLOAT                          { $$ = $1;          }
	| exp PLUS exp                  { $$ = $1 + $3;     }
	| exp MINUS exp                 { $$ = $1 - $3;     }
	| exp TIMES exp                 { $$ = $1 * $3;     }
	| exp DIVIDE exp                { $$ = $1 / $3;     }
	| MINUS exp %prec NEG           { $$ = -$2;         }
	| OPEN_PAREN exp CLOSE_PAREN    { $$ = $2;          }
	;

%%

void
yyerror(const char *str)
{
	fprintf(stderr, "yyerror: %s\n", str);
}

void
parse_error(void)
{
	fprintf(stderr, "file: %s, line: %d, column: %d\n\n",
                engrave_cur_file, engrave_lnum, engrave_col);
}


